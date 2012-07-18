/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                 *
*                                                                           *
* Copyright (c) 2008-2009 GASS team. See Contributors.txt for details.      *
*                                                                           *
* GASS is free software: you can redistribute it and/or modify              *
* it under the terms of the GNU Lesser General Public License as published  *
* by the Free Software Foundation, either version 3 of the License, or      *
* (at your option) any later version.                                       *
*                                                                           *
* GASS is distributed in the hope that it will be useful,                   *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU Lesser General Public License for more details.                       *
*                                                                           *
* You should have received a copy of the GNU Lesser General Public License  *
* along with GASS. If not, see <http://www.gnu.org/licenses/>.              *
*****************************************************************************/

#include "Core/System/GASSISystem.h"
#include "Core/System/GASSSystemFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/Serialize/GASSIXMLSerialize.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/MessageSystem/GASSBaseMessage.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Scene/GASSScene.h"

#include "tinyxml.h"
#include "tbb/parallel_for_each.h"
#include <tbb/tick_count.h>

namespace GASS
{
	SimSystemManager::SimSystemManager() : m_SimulationPaused(false), 
		m_SimulationUpdateInterval(1.0/60.0),
		m_SimulationTimeToProcess(0),
		m_MaxSimSteps(19),
		m_SimulateRealTime(true),
		m_LastNumSimulationSteps(0)
	{
		m_SystemMessageManager = MessageManagerPtr(new MessageManager());
	}

	SimSystemManager::~SimSystemManager()
	{

	}

	void SimSystemManager::Init()
	{
		LogManager::getSingleton().stream() << "SimSystemManager Initialization Started";
		MessagePtr init_msg(new InitSystemMessage());
		m_SystemMessageManager->SendImmediate(init_msg);
		LogManager::getSingleton().stream() << "SimSystemManager Initialization Completed";
	}	

	struct SystemUpdateInvoker 
	{
		SystemUpdateInvoker(double delta_time) :m_DeltaTime(delta_time)
		{}
		void operator()(SystemPtr& system) const {system->Update(m_DeltaTime);}
		double m_DeltaTime;
	};


	

#define DPRINT(mess) SendImmediate(MessagePtr( new DebugPrintMessage(mess)));

	void SimSystemManager::Update(float delta_time)
	{
		//m_Stats.clear();
		tbb::tick_count start_time = tbb::tick_count::now();


		//PRE_SIM_BUCKET
		UpdateMap::iterator iter = m_UpdateBuckets.find(PRE_SIM_BUCKET);
		if(iter != m_UpdateBuckets.end())
		{
			tbb::tick_count bucket_start_time = tbb::tick_count::now();
				
			if(iter->second.size() == 1) //single system
			{
					iter->second.at(0)->Update(delta_time);
			}
			else //do parallel update
			{
					tbb::parallel_for_each(iter->second.begin(),iter->second.end(),SystemUpdateInvoker(delta_time));
			}
			

			tbb::tick_count bucket_update_end_time = tbb::tick_count::now();

			BucketProfileData profileData;
			profileData.UpdateTime = (bucket_update_end_time- bucket_start_time).seconds();
			//wait to advance time
			SyncMessages(0);
			tbb::tick_count bucket_end_time = tbb::tick_count::now();
			profileData.SyncTime = (bucket_end_time- bucket_update_end_time).seconds();
			m_Stats[PRE_SIM_BUCKET] = profileData;
			
		}

		if (!m_SimulationPaused)
		{
			StepSimulation(delta_time);
		}


		//POST_SIM_BUCKET
		iter = m_UpdateBuckets.find(POST_SIM_BUCKET);
		if(iter != m_UpdateBuckets.end())
		{
			tbb::tick_count bucket_start_time = tbb::tick_count::now();

			if(iter->second.size() == 1) //single system
			{
				iter->second.at(0)->Update(delta_time);
			}
			else //do parallel update
			{
				tbb::parallel_for_each(iter->second.begin(),iter->second.end(),SystemUpdateInvoker(delta_time));
			}

			tbb::tick_count bucket_update_end_time = tbb::tick_count::now();
			BucketProfileData profileData;
			profileData.UpdateTime = (bucket_update_end_time- bucket_start_time).seconds();
			profileData.SyncTime = 0;
			m_Stats[POST_SIM_BUCKET] = profileData;

		}
		tbb::tick_count end_time = tbb::tick_count::now();

		//plot data

		double tot_time = 0;
		double tot_sync_time =0;
		double tot_update_time =0;


		if(m_LastNumSimulationSteps > 0)
		{
			std::stringstream ss;
			ss.precision(3);
			std::map<int,BucketProfileData>::iterator stat_iter = m_Stats.begin();
			while(stat_iter  != m_Stats.end())
			{
				ss << "Bucket:" << stat_iter->first << " Update:" << stat_iter->second.UpdateTime << " Sync:" << stat_iter->second.SyncTime <<  " Update vs Sync:" << 100*stat_iter->second.UpdateTime / (stat_iter->second.UpdateTime + stat_iter->second.SyncTime) <<  "\n";

				if(stat_iter->first != POST_SIM_BUCKET && stat_iter->first != PRE_SIM_BUCKET)
				{
					tot_time += stat_iter->second.SyncTime + stat_iter->second.UpdateTime;
					tot_sync_time += stat_iter->second.SyncTime;
					tot_update_time += stat_iter->second.UpdateTime+stat_iter->second.SyncTime;
				}
				stat_iter++;
			}

			stat_iter = m_Stats.begin();
			while(stat_iter  != m_Stats.end())
			{
				if(stat_iter->first != POST_SIM_BUCKET && stat_iter->first != PRE_SIM_BUCKET)
				{
					ss << "Bucket:" << stat_iter->first << " Tot:" <<  100*(stat_iter->second.AvgUpdateTime/(double) stat_iter->second.Count  + stat_iter->second.SyncTime)/tot_update_time << " Update:" << 100*stat_iter->second.UpdateTime/tot_update_time << " Sync:" << 100*stat_iter->second.SyncTime/tot_sync_time << "\n";
				}
				stat_iter++;
			}

			DPRINT(ss.str());
		}
			
		
	}





	void SimSystemManager::StepSimulation(double delta_time)
	{
		if(m_SimulationUpdateInterval >0) //fixed simulation rate
		{
			if(m_SimulateRealTime) //time slice to achive SimulationUpdateInterval
			{
				m_SimulationTimeToProcess += delta_time;
				int num_steps = (int) (m_SimulationTimeToProcess / m_SimulationUpdateInterval);
				int clamp_num_steps = num_steps;
				if(num_steps > m_MaxSimSteps) 
					clamp_num_steps = m_MaxSimSteps;

				

				for (int i = 0; i < clamp_num_steps; ++i)
				{
					UpdateSimulation(m_SimulationUpdateInterval);


				}
				m_SimulationTimeToProcess -= m_SimulationUpdateInterval * num_steps;
				m_LastNumSimulationSteps = clamp_num_steps;
				//std::stringstream ss;
				//ss << "SimulationTimeToProcess:" << m_SimulationTimeToProcess;
				//DPRINT(ss.str())
			}
			else
				UpdateSimulation(m_SimulationUpdateInterval);
		}
		else 
			UpdateSimulation(delta_time);
	}

	void SimSystemManager::UpdateSimulation(double delta_time)
	{
		double message_delta_time = delta_time;
		UpdateMap::iterator iter = m_UpdateBuckets.begin();
		for(;iter != m_UpdateBuckets.end(); iter++)
		{
			if(!(iter->first == POST_SIM_BUCKET || iter->first == PRE_SIM_BUCKET))
			{
				tbb::tick_count bucket_start_time = tbb::tick_count::now();

				if(iter->second.size() == 1) //single system
				{
					iter->second.at(0)->Update(delta_time);
				}
				else //do parallel update
				{
					tbb::parallel_for_each(iter->second.begin(),iter->second.end(),SystemUpdateInvoker(delta_time));
				}


				tbb::tick_count bucket_update_end_time = tbb::tick_count::now();

				BucketProfileData profileData;
				profileData.UpdateTime = (bucket_update_end_time- bucket_start_time).seconds();

				//sync
				SyncMessages(message_delta_time);
				//only step message time once
				message_delta_time = 0;

				tbb::tick_count bucket_end_time = tbb::tick_count::now();
				profileData.SyncTime = (bucket_end_time- bucket_update_end_time).seconds();
				m_Stats[iter->first].SyncTime = profileData.SyncTime;
				m_Stats[iter->first].UpdateTime = profileData.UpdateTime;

				m_Stats[iter->first].AvgSyncTime += profileData.SyncTime;
				m_Stats[iter->first].AvgUpdateTime += profileData.UpdateTime;

				m_Stats[iter->first].Count++;
				

			}
		}
	}

	

	void SimSystemManager::SyncMessages(double delta_time)
	{
		m_SystemMessageManager->Update(delta_time);
		//update all scene messages managers
		SimEngine::SceneIterator iter = SimEngine::Get().GetScenes();
		while(iter.hasMoreElements())
		{
			ScenePtr scene = iter.getNext();
			scene->SyncMessages(delta_time);
		}
	}


	int SimSystemManager::RegisterForMessage(const MessageType &type, MessageFuncPtr callback, int priority)
	{
		return m_SystemMessageManager->RegisterForMessage(type, callback, priority); 
	}

	void SimSystemManager::UnregisterForMessage(const MessageType &type,  MessageFuncPtr callback)
	{
		m_SystemMessageManager->UnregisterForMessage(type,  callback);
	}

	void SimSystemManager::PostMessage( MessagePtr message )
	{
		m_SystemMessageManager->PostMessage(message);
	}

	void SimSystemManager::SendImmediate( MessagePtr message )
	{
		m_SystemMessageManager->SendImmediate(message);
	}

	void SimSystemManager::ClearMessages()
	{
		m_SystemMessageManager->Clear();
	}
}
