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

#include <math.h>

#include "Core/Math/GASSPlane.h"
#include "Core/Math/GASSTriangle.h"

namespace GASS
{

	Plane::Plane()
	{

	}

	Plane::~Plane()
	{

	}

	Plane::Plane(const Triangle &tri)
	{
		m_Normal = Math::Cross((tri.P2 - tri.P1),(tri.P3 - tri.P1));
		m_Normal.Normalize();
		m_Origin = tri.P1;
		/*a = m_Normal.x;
		b = m_Normal.y;
		c = m_Normal.z;
		d = -(m_Normal.x*m_Origin.x+m_Normal.y*m_Origin.y
			+m_Normal.z*m_Origin.z);*/
	}

	/*void Plane::Normalize()
	{
		const float t = (float) sqrt(a * a + b * b + c * c);
		a /= t;
		b /= t;
		c /= t;
		d /= t;
	}*/
}
