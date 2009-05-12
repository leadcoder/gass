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

#include "Core/Math/AABox.h"
#include "Core/Math/Sphere.h"

namespace GASS
{
	Sphere::Sphere(const Vec3& center,float radius)
	{
		m_Radius = radius;
		m_Pos = center;
	}
		
	Sphere::Sphere()
	{
		m_Radius = 0;
		m_Pos.Set(0,0,0);

	}

	Sphere::~Sphere()
	{

	}

	AABox Sphere::GetAABox()
	{
		AABox aabox;
		aabox.m_Max.x = m_Pos.x + m_Radius;
		aabox.m_Min.x = m_Pos.x - m_Radius;

		aabox.m_Max.y = m_Pos.y + m_Radius;
		aabox.m_Min.y = m_Pos.y - m_Radius;

		aabox.m_Max.z = m_Pos.z + m_Radius;
		aabox.m_Min.z = m_Pos.z - m_Radius;
		return aabox;
	}

	bool Sphere::AABoxInside(const AABox &box)
	{
		AABox temp;
		temp.m_Max = box.m_Max;
		temp.m_Min = box.m_Min;
		temp.m_Max.x += m_Radius;
		temp.m_Max.y += m_Radius;
		temp.m_Max.z += m_Radius;

		temp.m_Min.x -= m_Radius;
		temp.m_Min.y -= m_Radius;
		temp.m_Min.z -= m_Radius;
		return temp.PointInside(m_Pos);
	}

	bool Sphere::SphereInside(const Sphere &sphere)
	{
		float dist = (sphere.m_Pos -  m_Pos).SquaredLength();
		float tot_rad = sphere.m_Radius + m_Radius;

		if(dist < tot_rad*tot_rad) return true;
		else return false;
	}

	void Sphere::Union(const Sphere &sphere)
	{

		Vec3 p1,p2,p3,v1;
		float r1,r2,r3,l;

		if(sphere.m_Radius > m_Radius)
		{
			r1 = sphere.m_Radius;
			p1 = sphere.m_Pos;
			r2 = m_Radius;
			p2 = m_Pos;
		}
		else
		{
			r2 = sphere.m_Radius;
			p2 = sphere.m_Pos;
			r1 = m_Radius;
			p1 = m_Pos;
		}

		v1 = p2-p1;
		l = v1.Length();
		if(r1 < (l+r2))
		{
			Vec3 vn = v1*(1.f/l);
			r3 = (r1+r2+l)/2.f;
			p3 = p1 - vn*r1+ vn*r3;
		}
		else
		{
			r3 = r1;
			p3 = p1;
		}
		m_Pos = p3;
		m_Radius = r3;

	}

}
