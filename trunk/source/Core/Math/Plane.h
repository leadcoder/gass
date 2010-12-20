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

#ifndef PLANE_HH
#define PLANE_HH


#include "Core/Math/CoreMath.h"
namespace GASS
{

	class GASSCoreExport Plane4
	{
	public:
		Plane4();
		Plane4(const Vec3 &p1,const Vec3 &p2,const Vec3 &p3);
		Plane4(const Vec3& origin, const Vec3& normal)
		{
			m_Normal = normal;
			m_Origin = origin;
			a = normal.x;
			b = normal.y;
			c = normal.z;
			d = -(normal.x*origin.x+normal.y*origin.y +normal.z*origin.z);
		};
		virtual ~Plane4();

		bool IsFrontFacingTo(const Vec3& direction) const
		{
			const double dot = Math::Dot(m_Normal,direction);
			return (dot <= 0);
		};
		double SignedDistanceTo(const Vec3& point) const
		{
			return Math::Dot(point,m_Normal) + d;
		};
		void Normalize();
		float a, b, c, d;
		Vec3 m_Normal;
		Vec3 m_Origin;
	};
}

#endif // #ifndef PLANE_HH
