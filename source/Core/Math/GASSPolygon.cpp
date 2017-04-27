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

#include "Core/Math/GASSPolygon.h"

namespace GASS
{
	template class TPolygon<double>;
#if 0
	Polygon::Polygon()
	{

	}

	Polygon::~Polygon()
	{

	}

	Vec3 Polygon::Center() const
	{
		Vec3 ret(0,0,0);
		for(size_t i = 0; i < m_VertexVector.size(); i++)
		{
			const Vec3* pos = &m_VertexVector[i];
			ret.x += pos->x;
			ret.y += pos->y;
			ret.z += pos->z;
		}
		ret = ret * (1.0/m_VertexVector.size());
		return ret;
	}
#endif
	
}
