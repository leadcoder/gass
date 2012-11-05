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



#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSRunTimeController.h"
#include "Sim/GASSTaskNode.h"
#include "Sim/GASSSimEngine.h"
#include "Core/Utils/GASSException.h"
#include <tbb/task_scheduler_init.h>
#include <tbb/spin_mutex.h>

namespace GASS
{
	RunTimeController::RunTimeController() : m_SimulationPaused(false),
		m_SimulateRealTime(false),
		m_StepSimulationRequest(false),
		m_RequestDeltaTime(0)
	{

	}

	RunTimeController::~RunTimeController()
	{	

	}

	void RunTimeController::Init(int num_threads)
	{
		//int nthread = tbb::task_scheduler_init::automatic;
		int  default_num_t = tbb::task_scheduler_init::default_num_threads();
		if(num_threads == -1)
			num_threads = default_num_t;
		
		m_Scheduler = new tbb::task_scheduler_init(num_threads);
		//m_TasksRoot = new( tbb::task::allocate_root() ) tbb::empty_task;
		LogManager::getSingleton().stream() << "RunTimeController initialized with "  << num_threads  << " threads";

		m_SimulationTaskNode = TaskNodePtr(new TaskNode());

		//Hack to support asyncron request
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(RunTimeController::OnSimulationStepRequest,RequestTimeStepMessage,0));
		
	}

	void RunTimeController::Register(TaskNodeListenerPtr listener, const std::string task_node_name)
	{
		{
			//tbb::spin_mutex::scoped_lock lock(m_Mutex);
			TaskNode* node = m_SimulationTaskNode->GetNodeByName(task_node_name);
			if(node)
				node->Register(listener);
			else
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to get TaskNode:" +task_node_name , " RunTimeController::Register");
		}
	}

	void RunTimeController::Unregister(TaskNodeListenerPtr listener, const std::string task_node_name)
	{
		{
			//tbb::spin_mutex::scoped_lock lock(m_Mutex);
		}
	}


	void RunTimeController::Update(double delta_time)
	{
		m_SimulationTaskNode->Update(delta_time,NULL);

		if(m_StepSimulationRequest) 
		{
			m_StepSimulationRequest = false;
			//send message that we are done
			SimEngine::Get().GetSimSystemManager()->SendImmediate(MessagePtr(new TimeStepDoneMessage()));
		}
		SimEngine::Get().GetSimSystemManager()->SyncMessages(delta_time);
		
	}

	void RunTimeController::Log()
	{

	}

	void RunTimeController::LoadXML(TiXmlElement *xml_elem)
	{
		m_SimulationTaskNode->LoadXML(xml_elem);
	}

	void RunTimeController::OnSimulationStepRequest(RequestTimeStepMessagePtr message)
	{
		m_StepSimulationRequest = true;
		m_RequestDeltaTime = message->GetTimeStep();
	}
}
