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

#ifndef GASS_POLYGON_H
#define GASS_POLYGON_H

#include "Core/Common.h"
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSPlane.h"
#include "Core/Math/GASSTriangle.h"
#include "Core/Math/GASSLineSegment.h"
#include <vector>

namespace GASS
{
	template<class TYPE>
	class TPolygon
	{
	public:
		/**\var vector<Vec3> m_VertexVector;
		* \brief Contains the vertices of the polygon.
		*/
		std::vector<TVec3<TYPE> > m_VertexVector;
		/**\var Vec3 m_Normal;
		* \brief The normal of the polygon plane.
		*/
		TVec3<TYPE> m_Normal;

		TVec3<TYPE> GetCenter() const
		{
			TVec3<TYPE> ret(0, 0, 0);
			for (size_t i = 0; i < m_VertexVector.size(); i++)
			{
				const TVec3<TYPE>* pos = &m_VertexVector[i];
				ret.x += pos->x;
				ret.y += pos->y;
				ret.z += pos->z;
			}
			ret = ret * (static_cast<TYPE>(1.0) / static_cast<TYPE>(m_VertexVector.size()));
			return ret;
		}

		/**
		Get plane from triangle
		*/
		TPlane<TYPE> GetPlane() const
		{
			return TPlane<TYPE>(m_VertexVector[0], m_Normal);
		}

		/**
		@brief Check if a line intersect a Polygon. NOTE: this assume that the polygon is convex!
		@param line_segment Line segment to check.
		@param isect_point Potential intersection point.
		@return True if intersection, otherwise false.
		*/
		bool LineIsectConvexPolygon(const TLineSegment<TYPE> &line_segment, TVec3<TYPE> &isect_point)
		{
			TPlane<TYPE> plane = GetPlane();
			const int side1 = plane.ClassifyPoint(line_segment.m_Start);
			const int side2 = plane.ClassifyPoint(line_segment.m_End);
			if ((side1 == PS_BACK && side2 == PS_BACK) || (side1 == PS_FRONT && side2 == PS_FRONT)) 
				return false;

			const TVec3<TYPE> ray_dir = line_segment.GetDirection();
			const TYPE ray_scale = plane.RayIsect(TRay<TYPE>(line_segment.m_Start, ray_dir));

			if (ray_scale == -1) 
				return false;

			isect_point = line_segment.m_Start + ray_dir * ray_scale;

			size_t size = m_VertexVector.size();
			size %= 3;
			size_t index = 1;
			for (size_t i = 0; i < size + 1; i++)
			{
				const TTriangle<TYPE> tri(
					m_VertexVector[0],
					m_VertexVector[index],
					m_VertexVector[index + 1]);

				if (tri.CheckPoint(isect_point))
				{
					return true;
				}
				++index;
			}
			return false;
		}
	};

	using Polygond = TPolygon<double>;
	using Polygonf = TPolygon<float>;
	using Polygon = TPolygon<GASS::Float>;
}

#endif // #ifndef POLYGON_HH
