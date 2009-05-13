/****************************************************************************
* This file is part of GASS.                                                *
* See http://sourceforge.net/projects/gass/                                 *
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

#include <math.h>
#include <stdio.h>
#include "Core/Math/Vector.h"
#include "Core/Math/Matrix.h"
#include "Core/Math/CoreMath.h"

namespace GASS
{

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
		Float invsqrt = Math::InvSqrt(x*x+y*y+z*z);
		x *= invsqrt;
		y *= invsqrt;
		z *= invsqrt;
	}

	Float Vec3::FastLength()  const
	{
		return 1.0f/Math::InvSqrt(x*x+y*y+z*z);
	}

	Float Vec3::FastInvLength() const 
	{
		return Math::InvSqrt(x*x+y*y+z*z);
	}

	std::string Vec3::ToString()
	{
		char ret[128];
		sprintf(ret,"%.3f;%.3f;%.3f",x,y,z);
		return ret;
	}


	



	/*void Vec3::Print()  const
	{
	printf("%f,%f,%f",x,y,z);
	}

	std::string Vec3::ToString()
	{
	char ret[128];
	sprintf(ret,"%.3f;%.3f;%.3f",x,y,z);
	return ret;
	}

	/*void Vec3::Transform3(const Mat4 &m)
	{
	Vec3 vec;
	vec.x = x*m.m_Data2[0]+y*m.m_Data2[4]+z*m.m_Data2[8]+m.m_Data2[12];
	vec.y = x*m.m_Data2[1]+y*m.m_Data2[5]+z*m.m_Data2[9]+m.m_Data2[13];
	vec.z = x*m.m_Data2[2]+y*m.m_Data2[6]+z*m.m_Data2[10]+m.m_Data2[14];
	x = vec.x;
	y = vec.y;
	z = vec.z;
	}*/
	
}
