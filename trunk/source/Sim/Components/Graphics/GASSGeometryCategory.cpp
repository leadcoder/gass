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
#include "GASSGeometryCategory.h"
#include "Core/Math/GASSVector.h"

namespace GASS
{
	template<> std::map<std::string ,GeometryType> EnumBinder<GeometryType,GeometryCategory>::m_Names;
	template<> std::map<GeometryType,std::string> EnumBinder<GeometryType,GeometryCategory>::m_Types;

	GeometryCategory::GeometryCategory() : EnumBinder<GeometryType,GeometryCategory>()
	{
		
	}	
	GeometryCategory::GeometryCategory(GeometryType type) : EnumBinder<GeometryType,GeometryCategory>(type)
	{

	}

	GeometryCategory::~GeometryCategory()
	{

	}

	void GeometryCategory::Register()
	{
		Bind("GT_REGULAR", GT_REGULAR);
		Bind("GT_TERRAIN", GT_TERRAIN);
		Bind("GT_GIZMO", GT_GIZMO);
		Bind("GT_UNKNOWN", GT_UNKNOWN);
	}
}
