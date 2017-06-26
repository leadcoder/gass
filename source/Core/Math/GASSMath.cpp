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
#include <limits>
#include "Core/Math/GASSMath.h"

#undef min
#undef max

namespace GASS
{
	bool Math::GetLineIntersection(const Vec2 &p1, const Vec2 &p2, const Vec2 &p3, const Vec2 &p4, Vec2 &isect)
	{
		// Store the values for fast access and easy
		// equations-to-code conversion

		const Float x1 = p1.x, x2 = p2.x, x3 = p3.x, x4 = p4.x;
		const Float y1 = p1.y, y2 = p2.y, y3 = p3.y, y4 = p4.y;

		const Float d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
		// If d is zero, there is no intersection
		if (d == 0)
			return false;

		// Get the x and y
		const Float pre = (x1*y2 - y1*x2), post = (x3*y4 - y3*x4);
		const Float x = (pre * (x3 - x4) - (x1 - x2) * post) / d;
		const Float y = (pre * (y3 - y4) - (y1 - y2) * post) / d;

		// Check if the x and y coordinates are within both lines
		if (x < std::min(x1, x2) || x > std::max(x1, x2) ||
			x < std::min(x3, x4) || x > std::max(x3, x4)) return false;
		if (y < std::min(y1, y2) || y > std::max(y1, y2) ||
			y < std::min(y3, y4) || y > std::max(y3, y4)) return false;

		// Return the point of intersection
		isect.x = x;
		isect.y = y;
		return true;
	}
}
