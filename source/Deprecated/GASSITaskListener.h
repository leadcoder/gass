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

#ifndef I_TASK_LISTENER_H
#define I_TASK_LISTENER_H

#include "Sim/GASSCommon.h"
#include "Sim/Scheduling/GASSTaskGroups.h"



namespace GASS
{

	class GASSExport ITaskListener
	{
	public:
		virtual ~ITaskListener(){}
		virtual void Update(double delta_time) = 0;
		virtual TaskGroup GetTaskGroup() const = 0;
	private:
	};
}
#endif
