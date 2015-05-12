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

#pragma once

#include "Sim/GASSCommon.h"
#include "Core/RTC/GASSTaskNode2.h"
#include "Sim/Messages/GASSCoreSystemMessages.h"

#include <vector>

namespace tbb
{
	class task_scheduler_init;
}

namespace GASS
{
	class TaskNode2;
	class ITaskNode2Listener;
	typedef SPTR<ITaskNode2Listener> TaskNode2ListenerPtr;
	typedef SPTR<TaskNode2> TaskNode2Ptr;


	enum SimulationState
	{
		SS_STOPPED,
		SS_PAUSED,
		SS_RUNNING,
		SS_EXTERNAL,
	};

	class GASSExport RunTimeController : public SHARE_CLASS<RunTimeController>,  public IMessageListener, public ITaskNode2Listener
	{
	public:
		RunTimeController(SimEngine* engine);
		virtual ~RunTimeController();
		/**
			Initialize the RTC, if number of threads is -1, TBB will
			match number of threads with number of machine kernels
		*/
		void Init(int num_threads = -1);

		/**
		Step simulation
		*/

		void Tick(double delta_time);

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
			Get root node for all runtimes update nodes, TODO: remove this...
		*/
		TaskNode2Ptr  GetRootNode() const {return m_RootNode;}

		/**
			Set this to true if we want to step simulation from external source
		*/
		void SetUpdateSimOnRequest(bool value);
		bool GetUpdateSimOnRequest() const{return m_UpdateSimOnRequest;}

		SimulationState GetSimulationState() const {return m_CurrentState;}

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
		double GetSimulationTimeScale() const {return m_SimTimeScale;}

		/**
			Set simulation time multiplier, default x1
		*/
		void SetSimulationTimeScale(double value) {m_SimTimeScale = value;}
	private:
		void OnSimulationStepRequest(TimeStepRequestPtr message);
		void Update(double delta_time, TaskNode2* caller);
		tbb::task_scheduler_init* m_Scheduler;
		SimEngine* m_Engine;

		TaskNode2Ptr m_RootNode;
		TaskNode2Ptr m_PreSimNode;
		TaskNode2Ptr m_SimNode;
		TaskNode2Ptr m_PostSimNode;

		bool m_UpdateSimOnRequest;
		
		//indicate that we want to step simulation next frame
		bool m_StepSimulationRequest;
		double m_RequestDeltaTime;
		double m_SimTimeScale;

		SimulationState m_CurrentState;
	};
	typedef SPTR<RunTimeController> RunTimeControllerPtr;
}