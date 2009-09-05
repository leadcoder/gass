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
#ifndef TASK_GROUPS_H
#define TASK_GROUPS_H

namespace GASS
{
	typedef std::string TaskGroup;
	static const TaskGroup MAIN_TASK_GROUP = "MAIN_TASK_GROUP";
	static const TaskGroup GRAPHICS_TASK_GROUP = "GRAPHICS_TASK_GROUP";
	static const TaskGroup PHYSICS_TASK_GROUP = "PHYSICS_TASK_GROUP";
	static const TaskGroup INPUT_TASK_GROUP = "INPUT_TASK_GROUP";
	static const TaskGroup SOUND_TASK_GROUP = "SOUND_TASK_GROUP";
	static const TaskGroup SIM_TASK_GROUP = "SOUND_TASK_GROUP";
}


#endif
