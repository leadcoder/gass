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

#ifndef TBB_RUNTIME_CONTROLLER_HH
#define TBB_RUNTIME_CONTROLLER_HH

#include "Sim/Common.h"
#include <vector>
#include "Sim/Scheduling/IRuntimeController.h"

#include "tbb/tick_count.h"
#include "tbb/task.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/spin_mutex.h"

namespace GASS
{
	typedef std::map<TaskGroup,TaskListenerVector> TaskGroupMap;
	class GASSExport TBBRuntimeController : public  IRuntimeController
	{
	public:
		TBBRuntimeController ();
		virtual ~TBBRuntimeController();
		void Init();
		void Update(double delta_time);
		void Register(ITaskListener *);
		void Unregister(ITaskListener *update_task);
		void Log();
	private:
		tbb::task* m_TasksRoot;
		tbb::spin_mutex m_Mutex;
		tbb::task_scheduler_init* m_Scheduler;
		TaskGroupMap m_TaskGroups;
		TaskListenerVector m_PrimaryUpdateVector;
	};
}

#endif // TBBRUNTIMECONTROLLER_HH
