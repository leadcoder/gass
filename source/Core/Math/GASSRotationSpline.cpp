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

#include <cassert>
#include "Core/Math/GASSRotationSpline.h"

namespace GASS
{
	RotationSpline::RotationSpline()
	{
		m_AutoCalc = false;
	}
	
	void RotationSpline::AddPoint(const Quaternion& p)
	{
		m_Points.push_back(p);
		if (m_AutoCalc)
		{
			RecalcTangents();
		}
	}
	//---------------------------------------------------------------------


	Quaternion RotationSpline::Interpolate(unsigned int fromIndex, float t) const
	{
		// Bounds check
		assert (fromIndex < m_Points.size() &&
			"fromIndex out of bounds");

		if ((fromIndex + 1) == m_Points.size())
		{
			// Duff request, cannot blend to nothing
			// Just return source
			return m_Points[fromIndex];

		}
		// Fast special cases
		if (t == 0.0f)
		{
			return m_Points[fromIndex];
		}
		else if(t == 1.0f)
		{
			return m_Points[fromIndex + 1];
		}

		// float interpolation
		// Use squad using tangents we've already set up
		const Quaternion &p = m_Points[fromIndex];
		const Quaternion &q = m_Points[fromIndex+1];
		const Quaternion &a = m_Tangents[fromIndex];
		const Quaternion &b = m_Tangents[fromIndex+1];

		return Quaternion::Squad(t, p, a, b, q);

	}
	//---------------------------------------------------------------------
	void RotationSpline::RecalcTangents()
	{
		// ShoeMake (1987) approach
		// Just like Catmull-Rom floatly, just more gnarly
		// And no, I don't understand how to derive this!
		//
		// let p = point[i], pInv = p.Inverse
		// tangent[i] = p * exp( -0.25 * ( log(pInv * point[i+1]) + log(pInv * point[i-1]) ) )
		//
		// Assume endpoint tangents are parallel with line with neighbour

		const auto num_points = static_cast<unsigned int>(m_Points.size());

		if (num_points < 2)
		{
			// Can't do anything yet
			return;
		}

		m_Tangents.resize(num_points);

		const bool is_closed = m_Points[0] == m_Points[num_points - 1];
	
		Quaternion invp, part1, part2, pre_exp;
		for(unsigned int i = 0; i < num_points; ++i)
		{
			const Quaternion &p = m_Points[i];
			invp = p.Inverse();

			if (i ==0)
			{
				// special case start
				part1 = (invp * m_Points[i+1]).Log();
				if (is_closed)
				{
					// Use num_Points-2 since num_Points-1 == end == start == this one
					part2 = (invp * m_Points[num_points-2]).Log();
				}
				else
				{
					part2 = (invp * p).Log();
				}
			}
			else if (i == num_points-1)
			{
				// special case end
				if (is_closed)
				{
					// Wrap to [1] (not [0], this is the same as end == this one)
					part1 = (invp * m_Points[1]).Log();
				}
				else
				{
					part1 = (invp * p).Log();
				}
				part2 = (invp * m_Points[i-1]).Log();
			}
			else
			{
				part1 = (invp * m_Points[i+1]).Log();
				part2 = (invp * m_Points[i-1]).Log();
			}

			pre_exp = -0.25 * (part1 + part2);
			m_Tangents[i] = p * pre_exp.Exp();
		}
	}

	//---------------------------------------------------------------------
	void RotationSpline::Clear()
	{
		m_Points.clear();
		m_Tangents.clear();
	}
}
