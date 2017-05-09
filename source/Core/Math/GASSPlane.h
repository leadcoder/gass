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
	enum PlaneSide
	{
		PS_FRONT,
		PS_BACK,
		PS_ON_PLANE
	};

	/**
	* Class representing a plane in 3D.
	*/
	template<class TYPE>
	class TPlane
	{
	public:
		TPlane()
		{

		}

		virtual ~TPlane()
		{

		}

		/**
		Construct plane from triangle
		*/
		TPlane(const TTriangle<TYPE> &tri)
		{
			m_Normal = TVec3<TYPE>::Cross((tri.P2 - tri.P1), (tri.P3 - tri.P1));
			m_Normal.Normalize();
			m_Origin = tri.P1;
		}

		/**
			Construct plane from plane point (origin) and plane normal
		*/

		TPlane(const TVec3<TYPE>& origin, const TVec3<TYPE>& normal) :
			m_Normal(normal),
			m_Origin(origin)
		{
			/*a = normal.x;
			b = normal.y;
			c = normal.z;
			d = -(normal.x*origin.x+normal.y*origin.y +normal.z*origin.z);*/
		};

		bool IsFrontFacingTo(const TVec3<TYPE>& direction) const
		{
			const TYPE dot = TVec3<TYPE>::Dot(m_Normal,direction);
			return (dot <= 0);
		};

		TYPE GetD() const
		{
			return -(m_Normal.x*m_Origin.x+m_Normal.y*m_Origin.y +m_Normal.z*m_Origin.z);
		}

		TYPE SignedDistanceTo(const TVec3<TYPE>& point) const
		{
			return TVec3<TYPE>::Dot(point,m_Normal) + GetD();
		};

		/**
		@brief Calculate the distance (along the ray) where a infinite
		ray intersect a infinite plane.
		@param ray Ray to check.
		@param plane Plane to check against.
		@return Intersection distance.
		*/
		static TYPE RayIsect(const Ray& ray, const Plane &plane)
		{
			return plane.RayIsect(ray);
		}

		TYPE RayIsect(const Ray &ray) const
		{
			const TYPE d = -(TVec3<TYPE>::Dot(m_Normal, m_Origin));

			const TYPE numer = TVec3<TYPE>::Dot(m_Normal, ray.m_Origin) + d;
			const TYPE denom = TVec3<TYPE>::Dot(m_Normal, ray.m_Dir);

			if (denom == 0)  // normal is orthogonal to vector, cant intersect
				return (-1.0f);

			return -(numer / denom);
		}

		PlaneSide ClassifyPoint(const TVec3<TYPE> &point)
		{
			const TVec3<TYPE> dir = origin - point;
			const double d = TVec3<TYPE>::Dot(dir, normal);

			if (d < -0.001f)
				return PS_FRONT;
			else if (d > 0.001f)
				return PS_BACK;
			return PS_ON_PLANE;
		}
	
		TVec3<TYPE> m_Normal;
		TVec3<TYPE> m_Origin;
	};

	typedef TPlane<float> Planef;
	typedef TPlane<double> Planed;
	typedef TPlane<Float> Plane;
}