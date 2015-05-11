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

#include <math.h>

#include "Core/Math/GASSPlane.h"

namespace GASS
{

	Plane4::Plane4()
	{

	}

	Plane4::~Plane4()
	{

	}

	Plane4::Plane4(const Vec3 &p1,const Vec3 &p2,const Vec3 &p3)
	{
		m_Normal = Math::Cross((p2-p1),(p3-p1));
		m_Normal.Normalize();
		m_Origin = p1;
		a = m_Normal.x;
		b = m_Normal.y;
		c = m_Normal.z;
		d = -(m_Normal.x*m_Origin.x+m_Normal.y*m_Origin.y
			+m_Normal.z*m_Origin.z);
	}

	void Plane4::Normalize()
	{
		const float t = (float) sqrt(a * a + b * b + c * c);
		a /= t;
		b /= t;
		c /= t;
		d /= t;
	}
}
