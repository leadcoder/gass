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



#include "Core/RTC/GASSRunTimeController2.h"
#include "Core/RTC/GASSTaskNode2.h"
#include "Core/Utils/GASSException.h"
#include "Core/Utils/GASSLogManager.h"
#include <tbb/task_scheduler_init.h>
#include <tbb/spin_mutex.h>

namespace GASS
{
	RunTimeController2::RunTimeController2() : m_SimulationPaused(false),
		m_SimulateRealTime(false),
		m_StepSimulationRequest(false),
		m_RequestDeltaTime(0)
	{

	}

	RunTimeController2::~RunTimeController2()
	{	

	}

	void RunTimeController2::Init(int num_threads)
	{
		//int nthread = tbb::task_scheduler_init::automatic;
		int  default_num_t = tbb::task_scheduler_init::default_num_threads();
		if(num_threads == -1)
			num_threads = default_num_t;
		
		m_Scheduler = new tbb::task_scheduler_init(num_threads);
		//m_TasksRoot = new( tbb::task::allocate_root() ) tbb::empty_task;
		//LogManager::getSingleton().stream() << "RunTimeController2 initialized with "  << num_threads  << " threads";

		m_RootNode = TaskNode2Ptr(new TaskNode2(this,0));

		//Hack to support asynchronous time stepping
		//SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(RunTimeController2::OnSimulationStepRequest,TimeStepRequest,0));
		
	}

	/*void RunTimeController2::Register(TaskNodeListener2Ptr listener, const std::string task_node_name)
	{
		{
			//tbb::spin_mutex::scoped_lock lock(m_Mutex);
			TaskNode2* node = m_RootNode->GetNodeByName(task_node_name);
			if(node)
				node->Register(listener);
			else
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to get TaskNode:" +task_node_name , " RunTimeController2::Register");
		}
	}

	void RunTimeController2::Unregister(TaskNodeListener2Ptr listener, const std::string task_node_name)
	{
		{
			//tbb::spin_mutex::scoped_lock lock(m_Mutex);
			TaskNode2* node = m_RootNode->GetNodeByName(task_node_name);
			if(node)
				node->Unregister(listener);
			else
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to get TaskNode:" +task_node_name , " RunTimeController2::Register");
		}
	}*/

	void RunTimeController2::Update(double delta_time)
	{
		m_RootNode->Update(delta_time,NULL);

		if(m_StepSimulationRequest) 
		{
			m_StepSimulationRequest = false;
			//send message that we are done
			//SimEngine::Get().GetSimSystemManager()->SendImmediate(SystemMessagePtr(new TimeStepDoneEvent()));
		}
		//SimEngine::Get().SyncMessages(delta_time);
	}

	void RunTimeController2::Log()
	{

	}

/*	void RunTimeController2::LoadXML(tinyxml2::XMLElement *xml_elem)
	{
		m_RootNode->LoadXML(xml_elem);
	}
*/
	/*void RunTimeController2::OnSimulationStepRequest(TimeStepRequestPtr message)
	{
		m_StepSimulationRequest = true;
		m_RequestDeltaTime = message->GetTimeStep();
	}*/
}
