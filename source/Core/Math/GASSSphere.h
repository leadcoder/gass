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

#ifndef GASS_SPHERE_H
#define GASS_SPHERE_H

#include "Core/Common.h"
#include "Core/Math/GASSVector.h"


namespace GASS
{

	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Math
	*  @{
	*/

	/**
	 Class holding Sphere information, position and radius
	*/

	template<class TYPE> class TAABox;
	typedef TAABox<Float> AABox;
	
	template<class TYPE>
	class TSphere
	{
	public:
		TSphere();
		TSphere(const TVec3<TYPE>& center,TYPE radius);
		virtual ~TSphere();
		TAABox<TYPE> GetAABox() const;
		bool AABoxInside(const TAABox<TYPE> &box) const;
		bool SphereInside(const TSphere &sphere) const;
		void Union(const TSphere &sphere);

		//public for fast access
		TYPE m_Radius;
		TVec3<TYPE> m_Pos;
	};
	typedef TSphere<double> Sphered;
	typedef TSphere<float> Spheref;
	typedef TSphere<Float> Sphere;

	template<class TYPE>
	TSphere<TYPE>::TSphere(const TVec3<TYPE>& center, TYPE radius)
	{
		m_Radius = radius;
		m_Pos = center;
	}

	template<class TYPE>
	TSphere<TYPE>::TSphere()
	{
		m_Radius = 0;
		m_Pos.Set(0, 0, 0);

	}

	template<class TYPE>
	TSphere<TYPE>::~TSphere()
	{

	}

	template<class TYPE>
	TAABox<TYPE> TSphere<TYPE>::GetAABox() const
	{
		TAABox<TYPE> aabox;
		aabox.m_Max.x = m_Pos.x + m_Radius;
		aabox.m_Min.x = m_Pos.x - m_Radius;

		aabox.m_Max.y = m_Pos.y + m_Radius;
		aabox.m_Min.y = m_Pos.y - m_Radius;

		aabox.m_Max.z = m_Pos.z + m_Radius;
		aabox.m_Min.z = m_Pos.z - m_Radius;
		return aabox;
	}

	template<class TYPE>
	bool TSphere<TYPE>::AABoxInside(const TAABox<TYPE> &box) const
	{
		TAABox<TYPE> temp;
		temp.m_Max = box.m_Max;
		temp.m_Min = box.m_Min;
		temp.m_Max.x += m_Radius;
		temp.m_Max.y += m_Radius;
		temp.m_Max.z += m_Radius;

		temp.m_Min.x -= m_Radius;
		temp.m_Min.y -= m_Radius;
		temp.m_Min.z -= m_Radius;
		return temp.PointInside(m_Pos);
	}

	template<class TYPE>
	bool TSphere<TYPE>::SphereInside(const TSphere &sphere) const
	{
		TYPE dist = (sphere.m_Pos - m_Pos).SquaredLength();
		TYPE tot_rad = sphere.m_Radius + m_Radius;

		if (dist < tot_rad*tot_rad) return true;
		else return false;
	}

	template<class TYPE>
	void TSphere<TYPE>::Union(const TSphere &sphere)
	{

		TVec3<TYPE> p1, p2, p3, v1;
		TYPE r1, r2, r3, l;

		if (sphere.m_Radius > m_Radius)
		{
			r1 = sphere.m_Radius;
			p1 = sphere.m_Pos;
			r2 = m_Radius;
			p2 = m_Pos;
		}
		else
		{
			r2 = sphere.m_Radius;
			p2 = sphere.m_Pos;
			r1 = m_Radius;
			p1 = m_Pos;
		}

		v1 = p2 - p1;
		l = v1.Length();
		if (r1 < (l + r2))
		{
			TVec3<TYPE> vn = v1*(1.f / l);
			r3 = (r1 + r2 + l) / 2.f;
			p3 = p1 - vn*r1 + vn*r3;
		}
		else
		{
			r3 = r1;
			p3 = p1;
		}
		m_Pos = p3;
		m_Radius = r3;
	}
}
#endif // #ifndef SPHERE_HH
