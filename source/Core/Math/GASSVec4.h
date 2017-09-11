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

#include "Core/Common.h"
#include "Core/Utils/GASSException.h"
#include <iostream>
#include <assert.h>
#include <math.h>

//NOTE THAT THIS FILE IS BASED ON CODE FROM: Ogre3d

namespace GASS
{
	template<class TYPE> class TMat4;
	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Math
	*  @{
	*/
	
	/**
	* Class representing a Float-vector with 4 elements.
	*/
	template<class TYPE>
	class TVec4
	{
	public:
		TYPE x,y,z,w;

		inline TVec4(){}
		inline TVec4(TYPE _x, TYPE _y, TYPE _z, TYPE _w)
		{
			x = _x;y = _y;z = _z;w = _w;
		}

		inline TVec4 operator+ (const TVec4 &v) const
		{
			TVec4 temp_v;

			temp_v.x= x + v.x;
			temp_v.y= y + v.y;
			temp_v.z= z + v.z;
			temp_v.w= w + v.w;

			return temp_v;
		}

		inline TVec4 operator- (const TVec4 &v) const
		{
			TVec4 temp_v;

			temp_v.x= x - v.x;
			temp_v.y= y - v.y;
			temp_v.z= z - v.z;
			temp_v.w= w - v.w;

			return temp_v;
		}


		inline TVec4 operator- () const
		{
			TVec4 temp_v;

			temp_v.x= - x;
			temp_v.y= - y;
			temp_v.z= - z;
			temp_v.w= - w;

			return temp_v;
		}


		inline TVec4 operator* (const TYPE scalar) const
		{
			TVec4 temp_v;

			temp_v.x= x * scalar;
			temp_v.y= y * scalar;
			temp_v.z= z * scalar;
			temp_v.w= w * scalar;

			return temp_v;
		}


		inline TVec4 operator* (const TVec4 &v) const
		{
			TVec4 temp_v;

			temp_v.x= x * v.x;
			temp_v.y= y * v.y;
			temp_v.z= z * v.z;
			temp_v.w= w * v.w;

			return temp_v;
		}

		inline TVec4 operator/ (const TVec4 &v) const
		{
			TVec4 temp_v;

			temp_v.x= x / v.x;
			temp_v.y= y / v.y;
			temp_v.z= z / v.z;
			temp_v.w= w / v.w;

			return temp_v;
		}

		inline TVec4& operator += ( const TVec4& v )
		{
			x += v.x;
			y += v.y;
			z += v.z;
			w += v.w;
			return *this;
		}

		/*TVec4 operator* (const TMat4<TYPE> &mat) const
		{
			TVec4<TYPE> ret;
			ret.x = x*mat.m_Data[0][0] + y*mat.m_Data[1][0] + z*mat.m_Data[2][0] + w*mat.m_Data[3][0];
			ret.y = x*mat.m_Data[0][1] + y*mat.m_Data[1][1] + z*mat.m_Data[2][1] + w*mat.m_Data[3][1];
			ret.z = x*mat.m_Data[0][2] + y*mat.m_Data[1][2] + z*mat.m_Data[2][2] + w*mat.m_Data[3][2];
			ret.w = x*mat.m_Data[0][3] + y*mat.m_Data[1][3] + z*mat.m_Data[2][3] + w*mat.m_Data[3][3];
			return ret;
		}*/

		inline void Set(TYPE _x, TYPE _y, TYPE _z, TYPE _w)
		{
			x = _x;y=_y; z=_z; w=_w;
		}
		inline bool operator == (const TVec4 &v) const
		{
			if(x == v.x && y == v.y && z == v.z && w == v.w) return true;
			else return false;
		}

		friend std::ostream& operator << (std::ostream& os, const TVec4& vec)
		{
			os << std::setprecision(std::numeric_limits<TYPE>::digits10 + 1) << vec.x << " " << vec.y << " " << vec.z << " " << vec.w;
			return os;
		}

		friend std::istream& operator >> (std::istream& is, TVec4& vec)
		{
			if(!(is >> vec.x >> vec.y >> vec.z >> vec.w))
			{
				GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "Failed to parse TVec4","TVec4::>>");
			}
			return is;
		}
	};

	typedef TVec4<Float> Vec4;
	typedef TVec4<double> Vec4d;
	typedef TVec4<float> Vec4f;
}


