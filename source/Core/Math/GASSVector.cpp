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

#include <stdio.h>
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSMath.h"

namespace GASS
{
/*
	Vec3 Vec3::m_UnitX = Vec3(1,0,0);
	Vec3 Vec3::m_UnitY = Vec3(0,1,0);
	Vec3 Vec3::m_UnitZ = Vec3(0,0,1);


	bool Vec3::InSphere(Vec3 &v, Float radius) const
	{
		Float dist = (*this - v).SquaredLength();
		if(dist < radius*radius) return true;
		else return false;
	}

	void Vec3::FastNormalize()
	{
		Float invsqrt = Math::InvSqrt(static_cast<float>(x*x+y*y+z*z));
		x *= invsqrt;
		y *= invsqrt;
		z *= invsqrt;
	}

	Float Vec3::FastLength()  const
	{
		return 1.0f/Math::InvSqrt(static_cast<float>(x*x+y*y+z*z));
	}

	Float Vec3::FastInvLength() const
	{
		return Math::InvSqrt(static_cast<float>(x*x+y*y+z*z));
	}

	std::string Vec3::ToString(const std::string &separator) const
	{
		char ret[128];
#ifdef _MSC_VER
		sprintf_s(ret,"%.3f%s%.3f%s%.3f",x,separator.c_str(),y,separator.c_str(),z);
#else
		sprintf(ret,"%.3f%s%.3f%s%.3f",x,separator.c_str(),y,separator.c_str(),z);
#endif
		return ret;
	}
	*/
}
