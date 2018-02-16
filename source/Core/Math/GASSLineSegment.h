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

namespace GASS
{
	/**
		Class holding line segment
	*/
	template<class TYPE>
	class TLineSegment
	{
	public:
		TLineSegment(){};
		TLineSegment(const TVec3<TYPE>& start,const TVec3<TYPE>& end): m_Start(start),
			m_End(end){}
		//public for fast access
		TVec3<TYPE> m_Start;
		TVec3<TYPE> m_End;

		TVec3<TYPE> GetPoint(TYPE dist) const {return m_Start + (m_End - m_Start)*dist;}
		TYPE GetLength() const {return (m_End - m_Start).Length();}

		TVec3<TYPE> GetDirection() const 
		{ 
			TVec3<TYPE> dir =  m_End - m_Start;
			dir.Normalize();
			return dir;
		}

		/**
		Get closest point on line segment
		@param point Input point
		@return Closest point on line segment to input point
		*/
		TVec3<TYPE> ClosestPointOnLine(const TVec3<TYPE> &point) const
		{
			const TVec3<TYPE> line_vec = m_End - m_Start;
			const TVec3<TYPE> point_vec = point - m_Start;

			const TYPE p_dot_l = TVec3<TYPE>::Dot(point_vec, line_vec);
			if (p_dot_l <= 0) 
				return m_Start;

			TYPE l_dot_l = TVec3<TYPE>::Dot(line_vec, line_vec);
			if (l_dot_l <= p_dot_l)
				return m_End;

			const  TYPE line_dist = p_dot_l / l_dot_l;
			const TVec3<TYPE> point_on_line = m_Start + line_vec*line_dist;
			return point_on_line;
		}
	};
	typedef TLineSegment<double> LineSegmentd;
	typedef TLineSegment<float> LineSegmentf;
	typedef TLineSegment<GASS::Float> LineSegment;
}
