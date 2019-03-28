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
#include "Core/Math/GASSVec2.h"

namespace GASS
{
	/**
		Class holding 2D line segment
	*/
	template<class TYPE>
	class TLineSegment2D
	{
	public:
		TLineSegment2D() {};
		TLineSegment2D(const TVec2<TYPE>& start, const TVec2<TYPE>& end) : Start(start),
			End(end) {}

		/**
		Get point on line segment
		@param dist normalized distance [0,1] where 0 == Start, 1==End
		@return Point on line at given distance
		*/
		TVec2<TYPE> GetPoint(TYPE dist) const { return Start + (End - Start)*dist; }
		TYPE GetLength() const { return (End - Start).Length(); }

		/**
		Get direction vector of line segment
		@return Normalized direction
		*/
		TVec2<TYPE> GetDirection() const
		{
			TVec2<TYPE> dir = End - Start;
			dir.Normalize();
			return dir;
		}

		/**
		Get closest point on line segment
		@param point Input point
		@return Closest point on line segment to input point
		*/
		TVec2<TYPE> ClosestPointOnLine(const TVec2<TYPE> &point) const
		{
			const TVec2<TYPE> line_vec = End - Start;
			const TVec2<TYPE> point_vec = point - Start;

			const TYPE p_dot_l = TVec2<TYPE>::Dot(point_vec, line_vec);
			if (p_dot_l <= 0)
				return Start;

			TYPE l_dot_l = TVec2<TYPE>::Dot(line_vec, line_vec);
			if (l_dot_l <= p_dot_l)
				return End;

			const TYPE line_dist = p_dot_l / l_dot_l;
			const TVec2<TYPE> point_on_line = Start + line_vec * line_dist;
			return point_on_line;
		}

		/**
		Get line intersection
		@param  first line
		@param  second  line
		@param  isect Potential intersection point
		@return true if intersection exist
		*/
		static bool GetIntersection(const TLineSegment2D<TYPE> &l1, const TLineSegment2D<TYPE> &l2, TVec2<TYPE> &isect)
		{
			// Store the values for fast access and easy
			// equations-to-code conversion

			const TYPE x1 = l1.Start.x;
			const TYPE x2 = l1.End.x;
			const TYPE x3 = l2.Start.x;
			const TYPE x4 = l2.End.x;

			const TYPE y1 = l1.Start.y;
			const TYPE y2 = l1.End.y;
			const TYPE y3 = l2.Start.y;
			const TYPE y4 = l2.End.y;

			const TYPE d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
			// If d is zero, there is no intersection
			if (d == 0)
				return false;

			// Get the x and y
			const TYPE pre = (x1*y2 - y1 * x2), post = (x3*y4 - y3 * x4);
			const TYPE x = (pre * (x3 - x4) - (x1 - x2) * post) / d;
			const TYPE y = (pre * (y3 - y4) - (y1 - y2) * post) / d;

			// Check if the x and y coordinates are within both lines
			if (x < std::min<TYPE>(x1, x2) || x > std::max<TYPE>(x1, x2) ||
				x < std::min<TYPE>(x3, x4) || x > std::max<TYPE>(x3, x4)) return false;
			if (y < std::min<TYPE>(y1, y2) || y > std::max<TYPE>(y1, y2) ||
				y < std::min<TYPE>(y3, y4) || y > std::max<TYPE>(y3, y4)) return false;

			// Return the point of intersection
			isect.x = x;
			isect.y = y;
			return true;
		}

		//public for fast access
		TVec2<TYPE> Start;
		TVec2<TYPE> End;
	};
	typedef TLineSegment2D<double> LineSegment2Dd;
	typedef TLineSegment2D<float> LineSegment2Df;
}
