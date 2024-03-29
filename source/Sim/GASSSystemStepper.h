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

#pragma once

#include "Sim/GASSCommon.h"
#include "Core/Utils/GASSEnumBinder.h"

namespace GASS
{
	enum UpdateGroupID
	{
		UGID_NO_UPDATE,
		UGID_PRE_SIM,
		UGID_SIM,
		UGID_POST_SIM,
		UGID_LAST
	};

	START_ENUM_BINDER(UpdateGroupID, UpdateGroupIDBinder)
		BIND(UGID_NO_UPDATE)
		BIND(UGID_PRE_SIM)
		BIND(UGID_SIM)
		BIND(UGID_POST_SIM)
		END_ENUM_BINDER(UpdateGroupID, UpdateGroupIDBinder)

	class SimSystemManager;

	class GASSExport SystemGroupStepper
	{
	public:
		SystemGroupStepper(UpdateGroupID id, SimSystemManager* manager);
		void Update(double delta_time);
		void SetPaused(bool value) { m_Paused = value; }
		bool GetPaused() const { return m_Paused; }
		void ResetTime() { m_CurrentTime = 0; }
		double GetTime() const { return m_CurrentTime; }
		void SetUpdateFrequency(double value) { m_UpdateFrequency = value; }
		double GetUpdateFrequency() const { return m_UpdateFrequency; }
		void SetMaxSimulationSteps(int value) { m_MaxSimulationSteps = value; }
		int GetMaxSimulationSteps() const { return m_MaxSimulationSteps; }
	private:
		bool m_Paused;
		double m_UpdateFrequency;
		int m_MaxSimulationSteps;
		double m_CurrentTime;
		UpdateGroupID m_ID;
		SimSystemManager* m_SimSysManager;
		double m_TimeToProcess;//Keep track of time if not updating
	};

	enum SimulationState
	{
		SS_STOPPED,
		SS_PAUSED,
		SS_RUNNING,
		SS_EXTERNAL,
	};

	class GASSExport SystemStepper
	{
		friend class SimSystemManager;
	public:
		SystemStepper(SimSystemManager* sim_sys_manager);
	protected:
		/**
			Initialize
		*/
		void OnInit();

		/**
		Step systems
		*/
		void OnUpdate(double delta_time);

		/**
			Set simulation node to paused or not
		*/
		void SetSimulationPaused(bool value);

		/**
			Stop simulation node
		*/
		void StopSimulation();

		/**
			Start simulation node
		*/
		void StartSimulation();

		/**
			Set this to true if we want to step simulation from external source
		*/
		void SetUpdateSimOnRequest(bool value);
		bool GetUpdateSimOnRequest() const { return m_UpdateSimOnRequest; }

		SimulationState GetSimulationState() const { return m_CurrentState; }

		/**
			Get time since first update
		*/
		double GetTime() const;

		/**
			Get simulation time
		*/
		double GetSimulationTime() const;

		/**
			Get simulation time multiplier, default x1
		*/
		double GetSimulationTimeScale() const { return m_SimTimeScale; }

		/**
			Set simulation time multiplier, default x1
		*/
		void SetSimulationTimeScale(double value) { m_SimTimeScale = value; }


		/**
			Get max simulation sub steps each frame, when target frame rate can't be reached (to large delta time)
			this value clamp number of simulation iteration that has to be executed to reached target delta_time
		*/
		int GetMaxSimulationSteps() const { return m_SimGroup.GetMaxSimulationSteps(); }

		/**
			Set max simulation sub steps each frame
		*/
		void SetMaxSimulationSteps(int value) { m_MaxSimulationSteps = value; m_SimGroup.SetMaxSimulationSteps(value); }

		void SetMaxUpdateFrequency(double fps)
		{
			m_PreSimGroup.SetUpdateFrequency(fps);
			m_PostSimGroup.SetUpdateFrequency(fps);
		}

		double GetMaxUpdateFrequency() const
		{
			return m_PostSimGroup.GetUpdateFrequency();
		}

		void SetSimulationUpdateFrequency(double fps)
		{
			m_SimGroup.SetUpdateFrequency(fps);
		}

		double GetSimulationUpdateFrequency() const
		{
			return m_SimGroup.GetUpdateFrequency();
		}
	
		void OnTimeStepRequest(double time)
		{
			m_StepSimulationRequest = true;
			m_RequestDeltaTime = time;
		}

	private:
		SimSystemManager* m_SimSysManager;
		bool m_UpdateSimOnRequest;
		bool m_StepSimulationRequest; //indicate that we want to step simulation next frame
		double m_RequestDeltaTime;
		double m_SimTimeScale;
		SimulationState m_CurrentState;
		double m_TimeToProcess;
		double m_CurrentTime;
		int m_MaxSimulationSteps;

		SystemGroupStepper m_PreSimGroup;
		SystemGroupStepper m_SimGroup;
		SystemGroupStepper m_PostSimGroup;
	};
	using SystemStepperPtr = std::shared_ptr<SystemStepper>;
}