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
#include <vector>

namespace GASS
{
	template<class TYPE>
	class TPolygon
	{
	public:
		TPolygon(){}
		~TPolygon() {}
		TVec3<TYPE> Center() const
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
		
		/**\var vector<Vec3> m_VertexVector;
		* \brief Contains the vertices of the polygon.
		*/
		std::vector<TVec3<TYPE> > m_VertexVector;
		/**\var Vec3 m_Normal;
		* \brief The normal of tha polygon plane.
		*/
		TVec3<TYPE> m_Normal;
	};

	typedef TPolygon<double> Polygond;
	typedef TPolygon<float> Polygonf;
	typedef TPolygon<GASS::Float> Polygon;
}

#endif // #ifndef POLYGON_HH
