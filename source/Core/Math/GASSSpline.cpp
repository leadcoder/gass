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
#include "Core/Math/GASSSpline.h"

namespace GASS
{

Spline::Spline(void)
{
	m_Coeffs.m_Data[0][0] = 2;
	m_Coeffs.m_Data[0][1] = -2;
	m_Coeffs.m_Data[0][2] = 1;
	m_Coeffs.m_Data[0][3] = 1;
	m_Coeffs.m_Data[1][0] = -3;
	m_Coeffs.m_Data[1][1] = 3;
	m_Coeffs.m_Data[1][2] = -2;
	m_Coeffs.m_Data[1][3] = -1;
	m_Coeffs.m_Data[2][0] = 0;
	m_Coeffs.m_Data[2][1] = 0;
	m_Coeffs.m_Data[2][2] = 1;
	m_Coeffs.m_Data[2][3] = 0;
	m_Coeffs.m_Data[3][0] = 1;
	m_Coeffs.m_Data[3][1] = 0;
	m_Coeffs.m_Data[3][2] = 0;
	m_Coeffs.m_Data[3][3] = 0;
    m_AutoCalc = false;
	m_TanStrength = 1;
}

Spline::~Spline(void)
{

}

Vec3 Spline::Interpolate(unsigned int fromIndex, Float t) const
    {
        // Bounds check
        assert(fromIndex < m_Points.size() &&
            "fromIndex out of bounds");

        if((fromIndex + 1) == m_Points.size())
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

        // Float interpolation
        // Form a vector of powers of t
        Float t2, t3;
        t2 = t * t;
        t3 = t2 * t;

		Vec4 powers;
        powers.x = t3;
		powers.y = t2;
		powers.z = t;
		powers.w = 1;

        // Algorithm is ret = powers * m_Coeffs.m_Data * Matrix4(point1, point2, tangent1, tangent2)
        const Vec3 &point1 = m_Points[fromIndex];
        const Vec3 &point2 = m_Points[fromIndex+1];
        const Vec3 &tan1 = m_Tangents[fromIndex];
        const Vec3 &tan2 = m_Tangents[fromIndex+1];
        Mat4 pt;

        pt[0][0] = point1.x;
        pt[0][1] = point1.y;
        pt[0][2] = point1.z;
        pt[0][3] = 1.0f;
        pt[1][0] = point2.x;
        pt[1][1] = point2.y;
        pt[1][2] = point2.z;
        pt[1][3] = 1.0f;
        pt[2][0] = tan1.x*m_TanStrength;
        pt[2][1] = tan1.y*m_TanStrength;
        pt[2][2] = tan1.z*m_TanStrength;
        pt[2][3] = 1.0f;
        pt[3][0] = tan2.x*m_TanStrength;
        pt[3][1] = tan2.y*m_TanStrength;
        pt[3][2] = tan2.z*m_TanStrength;
        pt[3][3] = 1.0f;

		Vec4 ret =  powers * m_Coeffs*pt;
		//Vec4 ret =  powers * pt;


		Vec3 res;
		res.Set(ret.x, ret.y, ret.z);
		return res;

    }

void Spline::Clear()
{
	m_Points.clear();
    m_Tangents.clear();
}

void Spline::AddPoint(Vec3 &p)
{
	m_Points.push_back(p);
	if (m_AutoCalc)
	{
		RecalcTangents();
    }
}


void Spline::RecalcTangents(void)
{
	// Catmull-Rom approach
	// 
	// tangent[i] = 0.5 * (point[i+1] - point[i-1])
	//
	// Assume endpoint tangents are parallel with line with neighbour

	unsigned int i, numPoints;
	bool isClosed;

	numPoints = static_cast<unsigned int>(m_Points.size());
	if (numPoints < 2)
	{
		// Can't do anything yet
		return;
	}

	// Closed or open?
	if (m_Points[0] == m_Points[numPoints-1])
	{
		isClosed = true;
	}
	else
	{
		isClosed = false;
	}

	m_Tangents.resize(numPoints);



	for(i = 0; i < numPoints; ++i)
	{
		if (i == 0)
		{
			// Special case start
			if (isClosed)
			{
				// Use numPoints-2 since numPoints-1 is the last point and == [0]
				m_Tangents[i] = (m_Points[1] - m_Points[numPoints-2])*0.5;
			}
			else
			{
				m_Tangents[i] = (m_Points[1] - m_Points[0])* 0.5;
			}
		}
		else if (i == numPoints-1)
		{
			// Special case end
			if (isClosed)
			{
				// Use same tangent as already calculated for [0]
				m_Tangents[i] = m_Tangents[0];
			}
			else
			{
				m_Tangents[i] = (m_Points[i] - m_Points[i-1])* 0.5;
			}
		}
		else
		{
			m_Tangents[i] = (m_Points[i+1] - m_Points[i-1])*0.5;
		}
	}
}
}
