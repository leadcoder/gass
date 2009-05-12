/****************************************************************************
* This file is part of GASS.                                                *
* See http://sourceforge.net/projects/gass/                                 *
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

#include "Core/Common.h"
#include "Core/Math/Vector.h"


namespace GASS
{

	class AABox;
	class GASSCoreExport Sphere  
	{
	public:
		Sphere();
		Sphere(const Vec3& center,float radius);
		virtual ~Sphere();
		void Draw();
		AABox GetAABox();
		bool AABoxInside(const AABox &box);
		bool SphereInside(const Sphere &sphere);
		void Union(const Sphere &sphere);
	
		float m_Radius;
		Vec3 m_Pos;
	};
}


