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
#include "Core/Math/GASSMath.h"
#include "Core/Math/GASSTriangle.h"
#include "Core/Math/GASSRay.h"

namespace GASS
{
	/**
	* Class representing a plane in 3D.
	*/

	class GASSCoreExport Plane
	{
	public:
		Plane()
		{

		}

		virtual ~Plane()
		{

		}

		/**
		Construct plane from triangle
		*/
		Plane(const Triangle &tri)
		{
			m_Normal = Vec3::Cross((tri.P2 - tri.P1), (tri.P3 - tri.P1));
			m_Normal.Normalize();
			m_Origin = tri.P1;
		}

		/**
			Construct plane from plane point (origin) and plane normal
		*/

		Plane(const Vec3& origin, const Vec3& normal)
		{
			m_Normal = normal;
			m_Origin = origin;
			/*a = normal.x;
			b = normal.y;
			c = normal.z;
			d = -(normal.x*origin.x+normal.y*origin.y +normal.z*origin.z);*/
		};

		bool IsFrontFacingTo(const Vec3& direction) const
		{
			const double dot = Vec3::Dot(m_Normal,direction);
			return (dot <= 0);
		};

		Float GetD() const
		{
			return -(m_Normal.x*m_Origin.x+m_Normal.y*m_Origin.y +m_Normal.z*m_Origin.z);
		}

		Float SignedDistanceTo(const Vec3& point) const
		{
			return Vec3::Dot(point,m_Normal) + GetD();
		};

		Float RayIsect(const Ray &ray) const
		{
			const Float d = -(Vec3::Dot(m_Normal, m_Origin));

			const Float numer = Vec3::Dot(m_Normal, ray.m_Origin) + d;
			const Float denom = Vec3::Dot(m_Normal, ray.m_Dir);

			if (denom == 0)  // normal is orthogonal to vector, cant intersect
				return (-1.0f);

			return -(numer / denom);
		}
		Vec3 m_Normal;
		Vec3 m_Origin;
	};
}