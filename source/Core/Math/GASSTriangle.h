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

#pragma once
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSLineSegment.h"

namespace GASS
{
	/**
		Class holding triangle
	*/
	template<class TYPE>
	class TTriangle
	{
	public:
		TTriangle(const TVec3<TYPE>& p1, const TVec3<TYPE>& p2, const TVec3<TYPE>& p3) : P1(p1), P2(p2), P3(p3) {}
		TTriangle() {}


		//public for fast access
		TVec3<TYPE> P1, P2, P3;

		bool LineIsect(const TLineSegment<TYPE> &line_segment, const TVec3<TYPE> &isect_point)
		{
			/*Vec3 normal;
			const Vec3 edge1 = tri.P2 - tri.P1;
			const Vec3 edge2 = tri.P3 - tri.P1;

			normal = Vec3::Cross(edge1, edge2);
			normal.Normalize();*/
			TPlane<TYPE> tri_plane(*this);

			const PlaneSide side1 = tri_plane.ClassifyPoint(line_segment.m_Start);
			const PlaneSide side2 = tri_plane.ClassifyPoint(line_segment.m_End);

			if ((side1 == PS_BACK && side2 == PS_BACK) || 
				(side1 == PS_FRONT && side2 == PS_FRONT)) 
				return false;

			TVec3<TYPE> ray_dir = line_segment.m_End - line_segment.m_Start;
			ray_dir.Normalize();
			const TYPE ray_scale = tri_plane.RayIsect(Ray(line_segment.m_Start, ray_dir));

			if (ray_scale == -1) 
				return false;

			isect_point = line_segment.m_Start + ray_dir * ray_scale;

			if (_CheckPoint(isect_point))
				return true;
			return false;
		}

		bool IsectTriangle(const TTriangle &t, TVec3<TYPE> &isect_point)
		{
			//check first against second
			if (LineIsect(LineSegment(t.P1, t.P2), isect_point)) return true;
			if (LineIsect(LineSegment(t.P1, t.P3), isect_point)) return true;
			if (LineIsect(LineSegment(t.P2, t.P3), isect_point)) return true;

			//check second against first
			if (t.LineIsect(LineSegment(P1, P2), isect_point)) return true;
			if (t.LineIsect(LineSegment(P1, P3), isect_point)) return true;
			if (t.LineIsect(LineSegment(P2, P3), isect_point)) return true;
			return false;
		}

		int _CheckPoint(const TVec3<TYPE>& point)
		{
			const TVec3<TYPE> e10 = P2 - P1;
			const TVec3<TYPE> e20 = P3 - P1;
			const TYPE a = TVec3<TYPE>::Dot(e10, e10);
			const TYPE b = TVec3<TYPE>::Dot(e10, e20);
			const TYPE c = TVec3<TYPE>::Dot(e20, e20);
			const TYPE ac_bb = (a*c) - (b*b);
			const TVec3<TYPE> vp(point.x - P1.x, point.y - P1.y, point.z - P1.z);
			const TYPE d = TVec3<TYPE>::Dot(vp, e10);
			const TYPE e = TVec3<TYPE>::Dot(vp, e20);
			const TYPE x = (d*c) - (e*b);
			const TYPE y = (e*a) - (d*b);
			const TYPE z = x + y - ac_bb;
			return ((int(z) & ~(int(x) | int(y))) & 0x80000000);
		}
	};

	typedef TTriangle<float> Trianglef;
	typedef TTriangle<double> Triangled;
	typedef TTriangle<Float> Triangle;
}
