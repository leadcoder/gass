/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                        *
*                                                                           *
* Copyright (c) 2008-2015 GASS team. See Contributors.txt for details.      *
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
		m_UpdateSimOnRequest(false),
		m_RequestDeltaTime(0),
		m_Engine(engine),
		m_CurrentState(SS_STOPPED),
		m_SimTimeScale(1.0)
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
		//Create task groups
		m_RootNode = TaskNode2Ptr(new GASS::TaskNode2(0));
		m_RootNode->SetUpdateFrequency(0.0);
		m_RootNode->SetListenerUpdateMode(GASS::TaskNode2::SEQUENCE);
		m_RootNode->SetListenerUpdateMode(GASS::TaskNode2::SEQUENCE);
		m_RootNode->SetChildrenUpdateMode(GASS::TaskNode2::SEQUENCE);

		m_PreSimNode = GASS::TaskNode2Ptr(new GASS::TaskNode2(UGID_PRE_SIM));

		double update_freq = m_Engine->GetMaxUpdateFreq();

		LogManager::getSingleton().stream() << "MaxUpdateFreq: " << update_freq;

		m_PreSimNode->SetUpdateFrequency(update_freq);
		m_PreSimNode->SetMaxSimulationSteps(1);
		m_PreSimNode->SetListenerUpdateMode(GASS::TaskNode2::SEQUENCE);
		//we need to sync messages after each update
		m_PreSimNode->RegisterPostUpdate(shared_from_this());
		m_RootNode->AddChildNode(m_PreSimNode);
		

		m_SimNode = GASS::TaskNode2Ptr(new GASS::TaskNode2(UGID_SIM));
		m_SimNode->SetUpdateFrequency(update_freq);
		m_SimNode->SetMaxSimulationSteps(1);
		m_SimNode->SetListenerUpdateMode(GASS::TaskNode2::SEQUENCE);
		//we need to sync messages after each update
		m_SimNode->RegisterPostUpdate(shared_from_this());
		m_RootNode->AddChildNode(m_SimNode);

		m_PostSimNode = GASS::TaskNode2Ptr(new GASS::TaskNode2(UGID_POST_SIM));
		m_PostSimNode->SetUpdateFrequency(update_freq);
		m_PostSimNode->SetMaxSimulationSteps(1);
		m_PostSimNode->SetListenerUpdateMode(GASS::TaskNode2::SEQUENCE);
		//we need to sync messages after each update
		m_PostSimNode->RegisterPostUpdate(shared_from_this());
		m_RootNode->AddChildNode(m_PostSimNode);

		m_Engine->GetSimSystemManager()->RegisterForMessage(REG_TMESS(RunTimeController::OnSimulationStepRequest, TimeStepRequest,0));
	}
	
	void RunTimeController::Tick(double delta_time)
	{
		if(m_UpdateSimOnRequest) //simulation is updated on request?
		{
			m_PreSimNode->Update(delta_time,NULL);
			if(m_StepSimulationRequest)
			{
				m_SimNode->Update(m_RequestDeltaTime,NULL);
				m_Engine->GetSimSystemManager()->SendImmediate(SystemMessagePtr(new TimeStepDoneEvent()));
				m_StepSimulationRequest = false;
			}
			m_PostSimNode->Update(delta_time,NULL);
		}
		else
		{
			m_PreSimNode->Update(delta_time,NULL);
			m_SimNode->Update(delta_time * m_SimTimeScale,NULL);
			m_PostSimNode->Update(delta_time,NULL);
		}
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
			//if manual stepping we can accept more simulation steps
			m_SimNode->SetMaxSimulationSteps(800);
		}
		else //if real time, don't substep
		{
			m_SimNode->SetMaxSimulationSteps(1);
		}
	}


	void RunTimeController::SetSimulationPaused(bool value)
	{
		if(value && m_CurrentState == SS_RUNNING)
		{
			m_CurrentState = SS_PAUSED;
			m_Engine->GetSimSystemManager()->PostMessage(GASS::SimEventPtr(new GASS::SimEvent(GASS::SET_PAUSE)));
			m_SimNode->SetPaused(true);
		}
		else if(!value && m_CurrentState == SS_PAUSED)
		{
			m_CurrentState = SS_RUNNING;
			m_Engine->GetSimSystemManager()->PostMessage(GASS::SimEventPtr(new GASS::SimEvent(GASS::SET_RESUME)));
			m_SimNode->SetPaused(false);
		}
	}

	void RunTimeController::StopSimulation()
	{
		m_CurrentState = SS_STOPPED;
		m_Engine->GetSimSystemManager()->PostMessage(GASS::SimEventPtr(new GASS::SimEvent(GASS::SET_STOP)));
		m_SimNode->SetPaused(true);
		m_SimNode->ResetTime();
	}

	void RunTimeController::StartSimulation()
	{
		if(m_CurrentState == SS_STOPPED)
		{
			m_Engine->GetSimSystemManager()->PostMessage(GASS::SimEventPtr(new GASS::SimEvent(GASS::SET_START)));
			m_SimNode->SetPaused(false);
			m_CurrentState = SS_RUNNING;
		}
	}

	double RunTimeController::GetTime() const
	{
		return m_PreSimNode->GetTime();
	}

	double RunTimeController::GetSimulationTime() const
	{
		return m_SimNode->GetTime();
	}

	void RunTimeController::Update(double delta_time, TaskNode2* caller)
	{
		if(m_PreSimNode.get() == caller)
		{

		}
		else if(m_SimNode.get() == caller)
		{

		}
		else if(m_PostSimNode.get() == caller)
		{

		}
		m_Engine->SyncMessages(delta_time);
	}
}
