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

#ifndef GAME_MESSAGES_H
#define GAME_MESSAGES_H

namespace GASS
{
	enum GameSceneObjectMessages
	{
		OBJECT_RM_ENTER_VEHICLE = 1000,
		OBJECT_RM_EXIT_VEHICLE = 1001,
		OBJECT_RM_GOTO_POSITION = 1002,
		OBJECT_NM_PLAYER_INPUT = 1003,
		OBJECT_RM_RELOAD = 1004,
		OBJECT_RM_FIRE = 1005,
		OBJECT_NM_READY_TO_FIRE = 1006,
		OBJECT_NM_HIT = 1007,
		OBJECT_RM_OUT_OF_ARMOR = 1008,
	};
}
#endif
