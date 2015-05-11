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

namespace GASS
{
	enum OSGNodeMasks
	{
		NM_VISIBLE = 1 << 0,
		NM_RECEIVE_SHADOWS = 1 << 1,
		NM_CAST_SHADOWS = 1 << 2,
		NM_USER_OFFSET  = 3,
		//Collision masks
		//NM_REGULAR_GEOMETRY = 1 << 3,
		//NM_TERRAIN_GEOMETRY  = 1 << 4,
		//NM_GIZMO_GEOMETRY = 1 << 5,
		//user defined start here
		//NM_USER  = 1 << 6
	};
}