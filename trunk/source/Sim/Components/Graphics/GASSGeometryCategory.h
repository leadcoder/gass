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
#include "Sim/Utils/GASSEnumBinder.h"
#include "Core/Math/GASSVector.h"

namespace GASS
{

	/**
		Enumeration of geometry types. 
		This can be used by a collision system to find certain
		geomtries or a physics engine to separate dynamic and static objects
	*/
	enum GeometryType
	{
		GT_REGULAR,
		GT_GIZMO,
		GT_TERRAIN,
		GT_UNKNOWN,
	};

	
	/**
		Class used to make GeometryType reflection possible
	*/
	class GASSExport GeometryCategory : public EnumBinder<GeometryType,GeometryCategory>
	{
	public:
		GeometryCategory();
		GeometryCategory(GeometryType type);
		virtual ~GeometryCategory();
		static void Register();
	};
	typedef boost::shared_ptr<GeometryCategory> GeometryCategoryPtr;

}
