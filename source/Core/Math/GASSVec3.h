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
#include "Core/Math/GASSMath.h"
#include <iostream>
#include <sstream>
#include <assert.h>
#include <math.h>
#include <stdio.h>

//NOTE THAT THIS FILE IS BASED ON CODE FROM: Ogre3d

namespace GASS
{
	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Math
	*  @{
	*/

	/**
	* Class representing a vector with 3 elements.
	*/
	template<class TYPE>
	class TVec3
	{
	public:
		TYPE x;//,h;
		TYPE y;//,p;
		TYPE z;//,r;

		static TVec3 m_UnitX;
		static TVec3 m_UnitY;
		static TVec3 m_UnitZ;

		inline TVec3(TYPE _x, TYPE _y, TYPE _z)
		{
			x = _x; y = _y; z = _z;
		}
		inline TVec3() {}

		inline void Set(TYPE _x, TYPE _y, TYPE _z)
		{
			x = _x; y = _y; z = _z;
		}

		// arithmetic operations
		inline TVec3 operator+ (const TVec3 &v) const
		{
			TVec3 temp_v;
			temp_v.x = x + v.x;
			temp_v.y = y + v.y;
			temp_v.z = z + v.z;
			return temp_v;
		}

		inline TVec3& operator += (const TVec3& v)
		{
			x += v.x;
			y += v.y;
			z += v.z;
			return *this;
		}

		inline TVec3 operator- (const TVec3 &v) const
		{
			TVec3 temp_v;

			temp_v.x = x - v.x;
			temp_v.y = y - v.y;
			temp_v.z = z - v.z;

			return temp_v;
		}



		inline TVec3 operator- () const
		{
			TVec3 temp_v;

			temp_v.x = -x;
			temp_v.y = -y;
			temp_v.z = -z;

			return temp_v;
		}

		inline TVec3& operator -= (const TVec3& v)
		{
			x -= v.x;
			y -= v.y;
			z -= v.z;
			return *this;
		}

		inline TVec3 operator* (const TYPE scalar) const
		{
			TVec3 temp_v;

			temp_v.x = x * scalar;
			temp_v.y = y * scalar;
			temp_v.z = z * scalar;

			return temp_v;
		}

		inline TVec3& operator *= (const TYPE scalar)
		{
			x *= scalar;
			y *= scalar;
			z *= scalar;
			return *this;
		}

		inline TVec3 operator* (const TVec3 &v) const
		{
			TVec3 temp_v;

			temp_v.x = x * v.x;
			temp_v.y = y * v.y;
			temp_v.z = z * v.z;
			return temp_v;
		}

		inline TVec3& operator *= (const TVec3& v)
		{
			x *= v.x;
			y *= v.y;
			z *= v.z;
			return *this;
		}

		inline TVec3 operator/ (const TVec3 &v) const
		{
			TVec3 temp_v;
			temp_v.x = x / v.x;
			temp_v.y = y / v.y;
			temp_v.z = z / v.z;
			return temp_v;
		}

		inline TVec3 operator/ (const TYPE scalar) const
		{
			TVec3 temp_v;

			temp_v.x = x / scalar;
			temp_v.y = y / scalar;
			temp_v.z = z / scalar;
			return temp_v;
		}

		inline 	TYPE& operator [] (unsigned element)
		{
			assert(element < 3);
			return *(&x + element);
		}

		inline 	TYPE operator [] (unsigned element) const
		{
			assert(element < 3);
			return *(&x + element);
		}

		inline friend TVec3 operator * (TYPE scalar, const TVec3& v)
		{
			TVec3 ret;

			ret.x = scalar * v.x;
			ret.y = scalar * v.y;
			ret.z = scalar * v.z;

			return ret;
		}


		inline bool operator== (const TVec3 &v) const
		{
			return (v.x == x &&  v.y == y && v.z == z);
		}

		inline bool Equal(const TVec3 &v, TYPE tolerance = std::numeric_limits<TYPE>::epsilon()) const
		{
			return Math::Equal(x, v.x, tolerance) &&
				   Math::Equal(y, v.y, tolerance) &&
				   Math::Equal(z, v.z, tolerance);
		}

		static TVec3 Deg2Rad(const TVec3 &vec)
		{
			TVec3 ret;
			ret.x = Math::Deg2Rad(vec.x);
			ret.y = Math::Deg2Rad(vec.y);
			ret.z = Math::Deg2Rad(vec.z);
			return ret;
		}

		static TVec3 Rad2Deg(const TVec3 &vec)
		{
			TVec3 ret;
			ret.x = Math::Rad2Deg(vec.x);
			ret.y = Math::Rad2Deg(vec.y);
			ret.z = Math::Rad2Deg(vec.z);
			return ret;
		}

		inline bool operator!= (const TVec3 &v) const
		{
			return !(v.x == x &&  v.y == y && v.z == z);
		}

		inline TYPE SquaredLength() const
		{
			return x*x + y*y + z*z;
		}

		inline TYPE Length() const
		{
			return sqrt(x*x + y*y + z*z);
		}

		inline void Normalize()
		{
			TYPE dist = Length();

			if (dist > 0)
			{
				x /= dist;
				y /= dist;
				z /= dist;
			} 
			else 
			{
				//consider 0, 0, 0 instead or assert
				x = 1;
				y = 0;
				z = 0;
			}
		}

		TYPE Dot(const TVec3 &v) const
		{
			return Dot(*this,v);
		}

		TVec3 Cross(const TVec3 &v) const
		{
			return Cross(*this, v);
		}

		/**
		@brief Calculate the dot product of two vectors.
		@param v1 First vector, as a Vec3.
		@param v2 Second vector, as a Vec3.
		@return The dot product.
		*/
		static TYPE Dot(const TVec3 &v1, const TVec3 &v2)
		{
			return (v1.x*v2.x + v1.y*v2.y + v1.z*v2.z);
		}

		/**
		@brief Calculate the cross product of two vectors.
		@param v1 First vector, as a Vec3.
		@param v2 Second vector, as a Vec3.
		@return The cross product.
		*/
		static TVec3 Cross(const TVec3 &v1, const TVec3 &v2)
		{
			TVec3 ret;
			ret.x = (v1.y * v2.z) - (v1.z * v2.y);
			ret.y = (v1.z * v2.x) - (v1.x * v2.z);
			ret.z = (v1.x * v2.y) - (v1.y * v2.x);
			return ret;
		}

		//move this to sphere?
		/*bool InSphere(TVec3 &v, Type radius) const
		{
			Float dist = (*this - v).SquaredLength();
			if (dist < radius*radius) return true;
			else return false;
		}*/

		friend std::ostream& operator << (std::ostream& os, const TVec3& vec)
		{
			os << vec.x << " " << vec.y << " " << vec.z;
			return os;
		}

		friend std::istream& operator >> (std::istream& is, TVec3& vec)
		{
			if (!(is >> vec.x >> vec.y >> vec.z))
			{
				GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "Failed to parse Vec3", "Vec3::>>");
			}

			return is;
		}

		

	/*	void FastNormalize()
		{
			Type invsqrt = Math::InvSqrt(static_cast<float>(x*x + y*y + z*z));
			x *= invsqrt;
			y *= invsqrt;
			z *= invsqrt;
		}

		Type FastLength()  const
		{
			return 1.0f / Math::InvSqrt(static_cast<float>(x*x + y*y + z*z));
		}

		Type FastInvLength() const
		{
			return Math::InvSqrt(static_cast<float>(x*x + y*y + z*z));
		}*/

		std::string ToString() const
		{
			std::stringstream ss;
			ss << this;
			return ss.str();
		/*	char ret[128];
#ifdef _MSC_VER
			sprintf_s(ret, "%.3f%s%.3f%s%.3f", x, separator.c_str(), y, separator.c_str(), z);
#else
			sprintf(ret, "%.3f%s%.3f%s%.3f", x, separator.c_str(), y, separator.c_str(), z);
#endif
			return ret;
			*/

		}
	};
	template <class TYPE> TVec3<TYPE> TVec3<TYPE>::m_UnitX = TVec3<TYPE>(1, 0, 0);
	template <class TYPE> TVec3<TYPE> TVec3<TYPE>::m_UnitY = TVec3<TYPE>(0, 1, 0);
	template <class TYPE> TVec3<TYPE> TVec3<TYPE>::m_UnitZ = TVec3<TYPE>(0, 0, 1);
	
	typedef TVec3<double> Vec3;
	typedef TVec3<float> Vec3f;
}

