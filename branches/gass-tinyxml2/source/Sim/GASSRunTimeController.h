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

#ifndef RUN_TIME_CONTROLLER_HH
#define RUN_TIME_CONTROLLER_HH

#include "Sim/GASSCommon.h"
#include "Sim/Messages/GASSCoreSystemMessages.h"
#include <vector>

namespace tinyxml2
{
	class XMLElement;
}


namespace tbb
{
	class task_scheduler_init;
}

namespace GASS
{
	class ITaskNodeListener;
	class TaskNode;
	typedef SPTR<ITaskNodeListener> TaskNodeListenerPtr;
	typedef SPTR<TaskNode> TaskNodePtr;

	class GASSExport RunTimeController : public SHARE_CLASS<RunTimeController>,  public IMessageListener
	{
	public:
		RunTimeController();
		virtual ~RunTimeController();
		/**
			Initialize the rtc controller, if number of threads is -1, TBB will
			match number of threads with number of machine kernerls
		*/
		void Init(int num_threads = -1);
		void Update(double delta_time);
		void Register(TaskNodeListenerPtr listener, const std::string task_node_name);
		void Unregister(TaskNodeListenerPtr listener, const std::string task_node_name);
		void Log();
		void LoadXML(tinyxml2::XMLElement *xml_elem);
		TaskNodePtr  GetTaskNode() const {return m_SimulationTaskNode;}
		bool HasUpdateRequest() const{return m_StepSimulationRequest;}
		double GetUpdateRequestTimeStep() const {return m_RequestDeltaTime;}
		bool GetSimulationPaused() const {return m_SimulationPaused;}
		void SetSimulationPaused(bool value) {m_SimulationPaused = value;}
	private:
		void OnSimulationStepRequest(TimeStepRequestPtr message);
		//tbb::spin_mutex m_Mutex;
		tbb::task_scheduler_init* m_Scheduler;
		TaskNodePtr m_SimulationTaskNode;

		bool m_SimulationPaused;
		bool m_SimulateRealTime;
		bool m_StepSimulationRequest;
		double m_RequestDeltaTime;

	};
	typedef SPTR<RunTimeController> RunTimeControllerPtr;
}

#endif