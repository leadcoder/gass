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

#ifndef TBB_UPDATE_TASK_H
#define TBB_UPDATE_TASK_H

#include "Sim/GASSCommon.h"
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include "Sim/Scheduling/GASSTBBRuntimeController.h"

namespace tbb
{
  class task;
}

namespace GASS
{
	/**\
	* \brief
	*/
	class GASSExport TBBUpdateTask : public tbb::task
	{
	public:
		TBBUpdateTask(double delta_time, const TaskListenerVector &update_vec);
		tbb::task*  execute();
	private:
		TaskListenerVector m_UpdateVec;
		double m_DeltaTime;

	};
}

#endif

