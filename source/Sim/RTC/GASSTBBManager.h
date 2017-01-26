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
#include "Sim/Messages/GASSCoreSystemMessages.h"

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
	typedef GASS_SHARED_PTR<ITaskNodeListener> TaskNodeListenerPtr;
	typedef GASS_SHARED_PTR<TaskNode> TaskNode2Ptr;

	class GASSExport TBBManager : public GASS_ENABLE_SHARED_FROM_THIS<TBBManager>
	{
	public:
		TBBManager();
		virtual ~TBBManager();
		/**
			Initialize TBB, if number of threads is -1, TBB will
			match number of threads with number of machine kernels
		*/
		void Init(int num_threads = -1);
	private:
		tbb::task_scheduler_init* m_Scheduler;
	};
	typedef GASS_SHARED_PTR<TBBManager> TBBManagerPtr;
}