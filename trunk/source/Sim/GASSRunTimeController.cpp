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
	RunTimeController::RunTimeController(SimEngine* engine) : m_StepSimulationRequest(false),
		m_RequestDeltaTime(0),
		m_Engine(engine)
	{

	}

	RunTimeController::~RunTimeController()
	{	

	}

	void RunTimeController::Init(int num_threads)
	{
		m_Engine->GetSimSystemManager()->RegisterForMessage(REG_TMESS(RunTimeController::OnSimulationStepRequest,TimeStepRequest,0));

		//Create task groups

		m_RootNode = TaskNode2Ptr(new GASS::TaskNode2(0));
		m_RootNode->SetUpdateFrequency(0.0);
		m_RootNode->SetListenerUpdateMode(GASS::TaskNode2::SEQUENCE);

		m_RootNode->SetListenerUpdateMode(GASS::TaskNode2::SEQUENCE);
		m_RootNode->SetChildrenUpdateMode(GASS::TaskNode2::SEQUENCE);

		m_PreSimNode = GASS::TaskNode2Ptr(new GASS::TaskNode2(UGID_PRE_SIM));

		double update_freq = m_Engine->GetMaxUpdateFreq();

		m_PreSimNode->SetUpdateFrequency(update_freq);
		m_PreSimNode->SetMaxSimulationSteps(1);
		m_PreSimNode->SetListenerUpdateMode(GASS::TaskNode2::SEQUENCE);
		m_RootNode->AddChildNode(m_PreSimNode);
		//pre_sim_node->RegisterPostUpdate(this);

		m_SimNode = GASS::TaskNode2Ptr(new GASS::TaskNode2(UGID_SIM));
		m_SimNode->SetUpdateFrequency(update_freq);
		m_SimNode->SetMaxSimulationSteps(1);
		m_SimNode->SetListenerUpdateMode(GASS::TaskNode2::SEQUENCE);
		m_RootNode->AddChildNode(m_SimNode);

		m_PostSimNode = GASS::TaskNode2Ptr(new GASS::TaskNode2(UGID_POST_SIM));
		m_PostSimNode->SetUpdateFrequency(update_freq);
		m_PostSimNode->SetMaxSimulationSteps(1);
		m_PostSimNode->SetListenerUpdateMode(GASS::TaskNode2::SEQUENCE);
		m_RootNode->AddChildNode(m_PostSimNode);

		m_Engine->GetSimSystemManager()->RegisterForMessage(REG_TMESS(RunTimeController::OnSimulationStepRequest, TimeStepRequest,0));
	}
	
	void RunTimeController::Tick(double delta_time)
	{
		//Manual update nodes
		m_PreSimNode->Update(delta_time,NULL);
		m_Engine->SyncMessages(delta_time);

		if(m_UpdateSimOnRequest)
		{
			if(m_StepSimulationRequest)
			{
				m_SimNode->Update(m_RequestDeltaTime,NULL);
				m_Engine->GetSimSystemManager()->SendImmediate(SystemMessagePtr(new TimeStepDoneEvent()));
				m_Engine->SyncMessages(m_RequestDeltaTime);
			}
		}
		else
		{
			m_SimNode->Update(delta_time,NULL);
			m_Engine->SyncMessages(delta_time);
		}

		m_PostSimNode->Update(delta_time,NULL);
		m_Engine->SyncMessages(delta_time);
	}

	void RunTimeController::OnSimulationStepRequest(TimeStepRequestPtr message)
	{
		m_StepSimulationRequest = true;
		m_RequestDeltaTime = message->GetTimeStep();
	}


	void RunTimeController::SetUpdateSimOnRequest(bool value) 
	{
		m_UpdateSimOnRequest = value;
		if(value)
		{
			//if manual stepping we should look to 600 step
			m_SimNode->SetMaxSimulationSteps(800);
		}
		else //if realtime, don't substep
		{
			m_SimNode->SetMaxSimulationSteps(1);
		}
	}

	void RunTimeController::Update(double delta_time, TaskNode2* caller)
	{

	}
}
