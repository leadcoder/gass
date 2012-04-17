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

#include <limits>
#include "Core/Math/GASSAABox.h"
#include "Core/Math/GASSSphere.h"
#include "Core/Math/GASSPolygon.h"

#undef min
#undef max
namespace GASS
{

	AABox::AABox()
	{
		m_Max.x = m_Max.y = m_Max.z = -std::numeric_limits<Float>::max();
		m_Min.x = m_Min.y = m_Min.z = std::numeric_limits<Float>::max();
	}

	AABox::AABox(const Vec3 &min_pos, const Vec3 &max_pos)
	{
		m_Max = (max_pos);
		m_Min = (min_pos);

	}

	AABox::~AABox()
	{

	}

	void AABox::Union(Float x,Float y,Float z)
	{
		Vec3 point;
		point.Set(x,y,z);
		Union(point);

	}

	void AABox::Union(const Vec3 &point)
	{
		if(point.x > m_Max.x) m_Max.x = point.x;
		if(point.x < m_Min.x) m_Min.x = point.x;

		if(point.y > m_Max.y) m_Max.y = point.y;
		if(point.y < m_Min.y) m_Min.y = point.y;

		if(point.z > m_Max.z) m_Max.z = point.z;
		if(point.z < m_Min.z) m_Min.z = point.z;
	}


	void AABox::Union(const AABox &aabox)
	{
		if(aabox.m_Max.x > m_Max.x) m_Max.x = aabox.m_Max.x;
		if(aabox.m_Min.x < m_Min.x) m_Min.x = aabox.m_Min.x;

		if(aabox.m_Max.y > m_Max.y) m_Max.y = aabox.m_Max.y;
		if(aabox.m_Min.y < m_Min.y) m_Min.y = aabox.m_Min.y;

		if(aabox.m_Max.z > m_Max.z) m_Max.z = aabox.m_Max.z;
		if(aabox.m_Min.z < m_Min.z) m_Min.z = aabox.m_Min.z;
	}


	void AABox::Transform(const Mat4 &mat)
	{
		Vec3 p1,p2,p3,p4,p5,p6,p7,p8;

		p1.Set(m_Min.x,m_Max.y,m_Max.z);
		p2 = m_Max;
		p3.Set(m_Min.x,m_Min.y,m_Max.z);
		p4.Set(m_Max.x,m_Min.y,m_Max.z);
		p5.Set(m_Min.x,m_Max.y,m_Min.z);
		p6.Set(m_Max.x,m_Max.y,m_Min.z);
		p7.Set(m_Max.x,m_Min.y,m_Min.z);
		p8 = m_Min;

		p1 = mat * p1;
		p2 = mat * p2;
		p3 = mat * p3;
		p4 = mat * p4;
		p5 = mat * p5;
		p6 = mat * p6;
		p7 = mat * p7;
		p8 = mat * p8;

		m_Max.x = m_Max.y = m_Max.z = -std::numeric_limits<Float>::max();
		m_Min.x = m_Min.y = m_Min.z =std::numeric_limits<Float>::max();

		Union(p1);
		Union(p2);
		Union(p3);
		Union(p4);
		Union(p5);
		Union(p6);
		Union(p7);
		Union(p8);
	}

	Sphere AABox::GetBoundingSphere() const
	{
		Sphere sphere;
		sphere.m_Pos = (m_Max + m_Min)*0.5;
		sphere.m_Radius = (sphere.m_Pos - m_Max).Length();
		return sphere;
	}


	AABox AABox::GetAABox(std::vector<Polygon> &poly_vec)
	{
		AABox aabox;
		for(size_t i = 0; i < poly_vec.size(); i++)
		{
			//Polygon poly = &(poly_vec[i]);
			//Vec3 center = poly->GetCenter();
			AABox poly_aabox = GetAABox(poly_vec[i]);

			if(i==0) aabox = poly_aabox;
			else aabox.Union(poly_aabox);

		}
		return aabox;
	}

	AABox AABox::GetAABox(const Polygon &poly)
	{
		AABox ret;
		for(size_t i = 0; i < poly.m_VertexVector.size(); i++)
		{
			const Vec3* pos = &poly.m_VertexVector[i];
			if(i == 0)
			{
				ret.m_Max.x = ret.m_Min.x = pos->x;
				ret.m_Max.y = ret.m_Min.y = pos->y;
				ret.m_Max.z = ret.m_Min.z = pos->z;
			}
			else
			{
				if(pos->x >= ret.m_Max.x) ret.m_Max.x = pos->x;
				else if(pos->x < ret.m_Min.x) ret.m_Min.x = pos->x;

				if(pos->y >= ret.m_Max.y) ret.m_Max.y = pos->y;
				else if(pos->y < ret.m_Min.y) ret.m_Min.y = pos->y;

				if(pos->z >= ret.m_Max.z) ret.m_Max.z = pos->z;
				else if(pos->z < ret.m_Min.z) ret.m_Min.z = pos->z;
			}
		}
		return ret;
	}



	bool AABox::PolyInside(const Polygon &poly) const
	{
		size_t i = 0;
		for(; i < poly.m_VertexVector.size(); i++)
		{
			const Vec3* pos = &poly.m_VertexVector[i];
			if(PointInside(*pos)) return true;
		}


		// Get Poly Center
		const Vec3 center = poly.Center();

		if(PointInside(center)) return true;

		const Vec3* p1,*p2;
		for(i = 0; i < poly.m_VertexVector.size(); i++)
		{
			int i2;
			if(i == poly.m_VertexVector.size()-1) i2 = 0; else i2 = i+1;
			p1 = &poly.m_VertexVector[i];
			p2 = &poly.m_VertexVector[i2];

			if(LineInside(*p1,*p2)) return true;
		}
		return false;
	}
	bool AABox::PointInside(const Vec3 &point) const
	{
		if(point.x < m_Max.x && point.x > m_Min.x &&
			point.y < m_Max.y && point.y > m_Min.y &&
			point.z < m_Max.z && point.z > m_Min.z)return true;
		else return false;

	}


	bool AABox::LineInside(const Vec3 &p1,const Vec3 &p2) const
	{
		float x,y,z,scale;
		Vec3 dir,pos,isect;
		pos = p1;
		dir = p2-p1;

		if(m_Max.x < p1.x && m_Max.x < p2.x) return false;
		if(m_Min.x > p1.x && m_Min.x > p2.x) return false;

		if(m_Max.y < p1.y && m_Max.y < p2.y) return false;
		if(m_Min.y > p1.y && m_Min.y > p2.y) return false;

		if(m_Max.z < p1.z && m_Max.z < p2.z) return false;
		if(m_Min.z > p1.z && m_Min.z > p2.z) return false;



		//Check max  X plane
		if((m_Max.x < p1.x && m_Max.x > p2.x) || (m_Max.x > p1.x && m_Max.x <= p2.x))
		{
			x = m_Max.x;
			scale = (x - pos.x) / dir.x;
			isect = pos + dir * scale;
			if(isect.y > m_Min.y && isect.y < m_Max.y && isect.z > m_Min.z && isect.z < m_Max.z) return true;
		}
		//Check min  X plane
		if((m_Min.x < p1.x && m_Min.x > p2.x) || (m_Min.x > p1.x && m_Min.x < p2.x))
		{
			x = m_Min.x;
			scale = (x - pos.x) / dir.x;
			isect = pos + dir * scale;
			if(isect.y > m_Min.y && isect.y < m_Max.y &&
				isect.z > m_Min.z && isect.z < m_Max.z) return true;
		}

		//Check max  Y plane
		if((m_Max.y < p1.y && m_Max.y > p2.y) || (m_Max.y > p1.y && m_Max.y < p2.y))
		{
			y = m_Max.y;
			scale = (y - pos.y) / dir.y;
			isect = pos + dir * scale;
			if(isect.x > m_Min.x && isect.x < m_Max.x &&
				isect.z > m_Min.z && isect.z < m_Max.z) return true;
		}

		//Check min  Y plane
		if((m_Min.y < p1.y && m_Min.y > p2.y) || (m_Min.y > p1.y && m_Min.y < p2.y))
		{
			y = m_Min.y;
			scale = (y - pos.y) / dir.y;
			isect = pos + dir * scale;
			if(isect.x > m_Min.x && isect.x < m_Max.x &&
				isect.z > m_Min.z && isect.z < m_Max.z) return true;
		}
		//Check max  Z plane
		if((m_Max.z < p1.z && m_Max.z > p2.z) || (m_Max.z > p1.z && m_Max.z < p2.z))
		{
			z = m_Max.z;
			scale = (z - pos.z) / dir.z;
			isect = pos + dir * scale;
			if(isect.y > m_Min.y && isect.y < m_Max.y &&
				isect.x > m_Min.x && isect.x < m_Max.x) return true;
		}

		//Check min  Z plane
		if((m_Min.z < p1.z && m_Min.z > p2.z) || (m_Min.z > p1.y && m_Min.z < p2.z))
		{
			z = m_Min.z;
			scale = (z - pos.z) / dir.z;
			isect = pos + dir * scale;
			if(isect.y > m_Min.y && isect.y < m_Max.y &&
				isect.x > m_Min.x && isect.x < m_Max.x) return true;
		}
		return false;
	}

	Vec3 AABox::GetSize() const
	{
		return (m_Max - m_Min);
	}
}

