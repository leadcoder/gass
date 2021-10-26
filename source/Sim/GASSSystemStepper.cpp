/****************************************************************************
* This file is part of GASS.                                                *
* See https://github.com/leadcoder/gass                                     *
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
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

#include <memory>

#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSystemStepper.h"

#include "Sim/GASSSimEngine.h"
#include "Core/Utils/GASSException.h"
#include "Core/Utils/GASSLogger.h"

namespace GASS
{
	SystemGroupStepper::SystemGroupStepper(UpdateGroupID id, SimSystemManager* sim_sys_manager) : m_TimeToProcess(0),
		m_UpdateFrequency(0),
		m_Paused(false),
		m_ID(id),
		m_MaxSimulationSteps(-1),
		m_CurrentTime(0),
		m_SimSysManager(sim_sys_manager)
	{

	}

	void SystemGroupStepper::Update(double delta_time)
	{
		if (!m_Paused)
		{
			if (m_UpdateFrequency > 0)
			{
				const double update_interval = 1.0 / m_UpdateFrequency;
				//do some time slicing
				m_TimeToProcess += delta_time;
				const auto num_steps = static_cast<long long> (m_TimeToProcess / update_interval);
				long long clamp_num_steps = num_steps;
				
				if (m_MaxSimulationSteps > 0 && num_steps > m_MaxSimulationSteps)
					clamp_num_steps = m_MaxSimulationSteps;

				//std::cout << "steps:" << clamp_num_steps << "\n";
				for (int i = 0; i < clamp_num_steps; ++i)
				{
					m_SimSysManager->UpdateSystems(update_interval, m_ID);
					SimEngine::Get().SyncMessages(update_interval);
					m_CurrentTime += update_interval;
				}
				m_TimeToProcess -= update_interval * num_steps;
			}
			else
			{
				m_SimSysManager->UpdateSystems(delta_time, m_ID);
				SimEngine::Get().SyncMessages(delta_time);
			}
		}
	}
	

	SystemStepper::SystemStepper(SimSystemManager* sim_system_manager) : m_StepSimulationRequest(false),
		m_UpdateSimOnRequest(false),
		m_RequestDeltaTime(0),
		m_SimSysManager(sim_system_manager),
		m_CurrentState(SS_STOPPED),
		m_SimTimeScale(1.0),
		m_TimeToProcess(0),
		m_MaxSimulationSteps(20),
		m_CurrentTime(0),
		m_PreSimGroup(UGID_PRE_SIM, sim_system_manager),
		m_SimGroup(UGID_SIM, sim_system_manager),
		m_PostSimGroup(UGID_POST_SIM, sim_system_manager)
	{
		m_PreSimGroup.SetUpdateFrequency(0);
		m_PreSimGroup.SetMaxSimulationSteps(1);
		m_SimGroup.SetUpdateFrequency(60.0);
		m_SimGroup.SetMaxSimulationSteps(m_MaxSimulationSteps);
		m_PostSimGroup.SetUpdateFrequency(0);
		m_PostSimGroup.SetMaxSimulationSteps(1);
	}

	void SystemStepper::OnInit()
	{
		GASS_LOG(LINFO) << "SystemStepper::OnInit - Pre/Post update frequency: " << GetMaxUpdateFrequency();
		GASS_LOG(LINFO) << "SystemStepper::OnInit - Sim update frequency: " << GetSimulationUpdateFrequency();
	}

	void SystemStepper::OnUpdate(double delta_time)
	{
		if (m_UpdateSimOnRequest) //simulation is updated on request?
		{
			m_PreSimGroup.Update(delta_time);
			if (m_StepSimulationRequest)
			{
				m_SimGroup.Update(m_RequestDeltaTime);
				m_SimSysManager->SendImmediate(SystemMessagePtr(new TimeStepDoneEvent()));
				m_StepSimulationRequest = false;
			}
			m_PostSimGroup.Update(delta_time);
		}
		else
		{
			m_PreSimGroup.Update(delta_time);
			m_SimGroup.Update(delta_time*m_SimTimeScale);
			m_PostSimGroup.Update(delta_time);
		}
	}

	void SystemStepper::SetUpdateSimOnRequest(bool value) 
	{
		m_UpdateSimOnRequest = value;
		if(value)
		{
			//if manual stepping we can accept more simulation steps
			m_SimGroup.SetMaxSimulationSteps(800);
		}
		else //if real time, don't sub-step
		{
			m_SimGroup.SetMaxSimulationSteps(m_MaxSimulationSteps);
		}
	}

	void SystemStepper::SetSimulationPaused(bool value)
	{
		if(value && m_CurrentState == SS_RUNNING)
		{
			m_CurrentState = SS_PAUSED;
			m_SimSysManager->PostMessage(std::make_shared<GASS::SimEvent>(GASS::SET_PAUSE));
			m_SimGroup.SetPaused(true);
		}
		else if(!value && m_CurrentState == SS_PAUSED)
		{
			m_CurrentState = SS_RUNNING;
			m_SimSysManager->PostMessage(std::make_shared<GASS::SimEvent>(GASS::SET_RESUME));
			m_SimGroup.SetPaused(false);
		}
	}

	void SystemStepper::StopSimulation()
	{
		m_CurrentState = SS_STOPPED;
		m_SimSysManager->PostMessage(std::make_shared<GASS::SimEvent>(GASS::SET_STOP));
		m_SimGroup.SetPaused(true);
		m_SimGroup.ResetTime();
	}

	void SystemStepper::StartSimulation()
	{
		if(m_CurrentState == SS_STOPPED)
		{
			m_SimSysManager->PostMessage(std::make_shared<GASS::SimEvent>(GASS::SET_START));
			m_SimGroup.SetPaused(false);
			m_CurrentState = SS_RUNNING;
		}
	}

	double SystemStepper::GetTime() const
	{
		return m_PreSimGroup.GetTime();
	}

	double SystemStepper::GetSimulationTime() const
	{
		return m_SimGroup.GetTime();
	}
}