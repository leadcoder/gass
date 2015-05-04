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

#pragma once

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
	class ITaskNode2Listener;
	class TaskNode2;
	typedef SPTR<ITaskNode2Listener> TaskNode2ListenerPtr;
	typedef SPTR<TaskNode2> TaskNode2Ptr;

	class GASSCoreExport RunTimeController2 : public SHARE_CLASS<RunTimeController2>
	{
	public:
		RunTimeController2();
		virtual ~RunTimeController2();
		/**
			Initialize the rtc controller, if number of threads is -1, TBB will
			match number of threads with number of machine kernels
		*/
		void Init(int num_threads = -1);
	private:
		tbb::task_scheduler_init* m_Scheduler;
	};
	typedef SPTR<RunTimeController2> RunTimeController2Ptr;
}