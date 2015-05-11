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

#include "Sim/GASSCommon.h"
#include "Core/Utils/GASSEnumBinder.h"
#include "Core/Math/GASSVector.h"

namespace GASS
{

	/**
		Enumeration of geometry types. 
		This can be used by a collision system to find certain
		geometries or a physics engine to separate dynamic and static objects
	*/
	/*enum GeometryType
	{
		GT_REGULAR,
		GT_GIZMO,
		GT_TERRAIN,
		GT_UNKNOWN,
	};*/

	//Some predefined geometry types, can be used during collision picking
	enum GeometryFlags
	{
		GEOMETRY_FLAG_UNKNOWN				=   1 << 0,
		GEOMETRY_FLAG_GROUND				=   1 << 1,
		GEOMETRY_FLAG_STATIC_OBJECT			=	1 << 2,
		GEOMETRY_FLAG_DYNAMIC_OBJECT		=	1 << 3,
		GEOMETRY_FLAG_TRANSPARENT_OBJECT	=	1 << 4,
		GEOMETRY_FLAG_EDITOR				=	1 << 5,
		GEOMETRY_FLAG_GIZMO					=	1 << 6,
		GEOMETRY_FLAG_RAY_CAST_WHEEL		=	1 << 7,
		GEOMETRY_FLAG_VEHICLE_CHASSIS		=	1 << 8,
		GEOMETRY_FLAG_PAGED_LOD				=	1 << 9,
		
		GEOMETRY_FLAG_ALL					=	GEOMETRY_FLAG_UNKNOWN | GEOMETRY_FLAG_GROUND | GEOMETRY_FLAG_STATIC_OBJECT | GEOMETRY_FLAG_STATIC_OBJECT | GEOMETRY_FLAG_DYNAMIC_OBJECT | GEOMETRY_FLAG_TRANSPARENT_OBJECT | GEOMETRY_FLAG_EDITOR | GEOMETRY_FLAG_GIZMO,
		GEOMETRY_FLAG_SCENE_OBJECTS		    =   GEOMETRY_FLAG_UNKNOWN | GEOMETRY_FLAG_GROUND | GEOMETRY_FLAG_STATIC_OBJECT | GEOMETRY_FLAG_DYNAMIC_OBJECT,
	};



	//Some predefined materials
	enum MaterialFlag
	{
		//Ground materials (10 reserved)
		MATERIAL_FLAG_MUD				=	1 << 0,
		MATERIAL_FLAG_GRAVEL			=	1 << 1,
		MATERIAL_FLAG_TARMAC			=	1 << 2,
		MATERIAL_FLAG_GRASS			    =	1 << 3,
		MATERIAL_FLAG_SAND				=	1 << 4,
		MATERIAL_FLAG_STONE				=	1 << 5,
		MATERIAL_FLAG_WATER				=	1 << 6,
		MATERIAL_FLAG_UNKONWN_GROUND	=	1 << 7,
		MATERIAL_FLAG_GROUND_ALL		=	MATERIAL_FLAG_MUD | MATERIAL_FLAG_GRAVEL | MATERIAL_FLAG_TARMAC | MATERIAL_FLAG_GRASS | MATERIAL_FLAG_SAND | MATERIAL_FLAG_STONE | MATERIAL_FLAG_WATER | MATERIAL_FLAG_UNKONWN_GROUND,

		//Object materials (10 reserved)
		MATERIAL_FLAG_METAL				=	1 << 10,
		MATERIAL_FLAG_PLASTIC			=	1 << 11,
		MATERIAL_FLAG_RUBBER 		    =	1 << 12,
		MATERIAL_FLAG_WOOD				=	1 << 13,
		MATERIAL_FLAG_FABRIC			=	1 << 14,
		MATERIAL_FLAG_UNKONWN_OBJECT	=	1 << 15,
		MATERIAL_FLAG_OBJECT_ALL		=	MATERIAL_FLAG_METAL| MATERIAL_FLAG_PLASTIC | MATERIAL_FLAG_RUBBER | MATERIAL_FLAG_UNKONWN_OBJECT,
	};


	/**
		Class used to manage GeometryFlags reflection possible
	*/
	class GASSExport GeometryFlagManager
	{
	public:
		GeometryFlagManager() {}
		virtual ~GeometryFlagManager() {}
		/**
			Load geometry flags form xml-files
		*/
		static void LoadGeometryFlagsFile(const std::string &file);
		static GeometryFlags GetMask(GeometryFlags geom_flag);
	private:
		static std::map<GeometryFlags, GeometryFlags> m_CollisionMaskMap;
	};

	/**
		Class used to make GeometryFlags reflection possible
	*/
	START_FLAG_ENUM_BINDER(GeometryFlags,GeometryFlagsBinder)
		BIND_FLAG(GEOMETRY_FLAG_UNKNOWN)
		BIND_FLAG(GEOMETRY_FLAG_GROUND)
		BIND_FLAG(GEOMETRY_FLAG_STATIC_OBJECT)
		BIND_FLAG(GEOMETRY_FLAG_DYNAMIC_OBJECT)
		BIND_FLAG(GEOMETRY_FLAG_TRANSPARENT_OBJECT)
		BIND_FLAG(GEOMETRY_FLAG_EDITOR)
		BIND_FLAG(GEOMETRY_FLAG_GIZMO)
		BIND_FLAG(GEOMETRY_FLAG_RAY_CAST_WHEEL)
		BIND_FLAG(GEOMETRY_FLAG_VEHICLE_CHASSIS)
		//BIND_FLAG(GEOMETRY_FLAG_SCENE_OBJECTS)
		//BIND_FLAG(GEOMETRY_FLAG_ALL)
	END_FLAG_ENUM_BINDER(GeometryFlags,GeometryFlagsBinder)
}
