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

#ifndef GASS_VECTOR_H
#define GASS_VECTOR_H

#include "Core/Common.h"
#include "Core/Math/GASSMatrix.h"
#include "Core/Utils/GASSException.h"
#include <iostream>
#include <assert.h>
#include <math.h>

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
	* Class that holds two Floats.
	*/
	class GASSCoreExport Vec2
	{
	public:
		Float x,y;
		inline Vec2(Float _x,Float _y)
		{
			x = _x;y = _y;
		}
		inline Vec2(){}
		inline void Set(float _x,float _y)
		{
			x = _x; y = _y;
		}

		inline bool operator== (const Vec2 &v) const
		{
			return (v.x == x &&  v.y == y);
		}

		friend std::ostream& operator << (std::ostream& os, const Vec2& vec)
		{
			os << vec.x << " " << vec.y;
			return os;
		}

		friend std::istream& operator >> (std::istream& is, Vec2& vec)
		{
			if(!(is >> vec.x >>  vec.y))
			{
				GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "Failed to parse vec2 value","Vec2::>>");
			}
			return is;
		}
	};

	/**
	* Class that holds two integer.
	*/
	class GASSCoreExport Vec2i
	{
	public:
		int x,y;

		void Set(int _x,int _y)
		{
			x = _x;y = _y;
		}
		Vec2i operator+ (const Vec2i &v)
		{
			Vec2i ret;
			ret.x = x + v.x;
			ret.y = y + v.y;
			return ret;
		}
		Vec2i operator- (const Vec2i &v)
		{
			Vec2i ret;
			ret.x = x - v.x;
			ret.y = y - v.y;
			return ret;
		}
		Vec2i operator- ()
		{
			Vec2i ret;
			ret.x = -x;
			ret.y = -y;
			return ret;
		}
		Vec2i operator* (Float scalar)
		{
			Vec2i ret;
			ret.x = static_cast<int>(x*scalar);
			ret.y = static_cast<int>(y*scalar);
			return ret;
		}

		friend std::ostream& operator << (std::ostream& os, const Vec2i& vec)
		{
			os << vec.x << " " << vec.y;
			return os;
		}

		friend std::istream& operator >> (std::istream& is, Vec2i& vec)
		{
			if(!(is >> vec.x  >> vec.y))
			{
				GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "Failed to parse Vec2i","Vec2i::>>");
			}
			return is;
		}


	};

	/**
	* Class representing a Float-vector with 3 elements.
	*/
	class GASSCoreExport Vec3
	{
	public:
		Float x;//,h;
		Float y;//,p;
		Float z;//,r;
		
		static Vec3 m_UnitX;
		static Vec3 m_UnitY;
		static Vec3 m_UnitZ;



		inline Vec3(Float _x,Float _y,Float _z)
		{
			x = _x;y = _y;z = _z;
		}
		inline Vec3(){}

		inline void Set(Float _x,Float _y,Float _z)
		{
			x = _x; y = _y; z = _z;
		}

		// arithmetic operations
		inline Vec3 operator+ (const Vec3 &v) const
		{
			Vec3 temp_v;
			temp_v.x= x + v.x;
			temp_v.y= y + v.y;
			temp_v.z= z + v.z;
			return temp_v;
		}

		inline Vec3& operator += ( const Vec3& v )
		{
			x += v.x;
			y += v.y;
			z += v.z;
			return *this;
		}

		inline Vec3 operator- (const Vec3 &v) const
		{
			Vec3 temp_v;

			temp_v.x= x - v.x;
			temp_v.y= y - v.y;
			temp_v.z= z - v.z;

			return temp_v;
		}

		

		inline Vec3 operator- () const
		{
			Vec3 temp_v;

			temp_v.x= - x;
			temp_v.y= - y;
			temp_v.z= - z;

			return temp_v;
		}

		inline Vec3& operator -= ( const Vec3& v )
		{
			x -= v.x;
			y -= v.y;
			z -= v.z;
			return *this;
		}

		inline Vec3 operator* (const Float scalar) const
		{
			Vec3 temp_v;

			temp_v.x= x * scalar;
			temp_v.y= y * scalar;
			temp_v.z= z * scalar;

			return temp_v;
		}

		inline Vec3& operator *= ( const Float scalar)
		{
			x *= scalar;
			y *= scalar;
			z *= scalar;
			return *this;
		}

		inline Vec3 operator* (const Vec3 &v) const
		{
			Vec3 temp_v;

			temp_v.x= x * v.x;
			temp_v.y= y * v.y;
			temp_v.z= z * v.z;
			return temp_v;
		}

		inline Vec3& operator *= ( const Vec3& v )
		{
			x *= v.x;
			y *= v.y;
			z *= v.z;
			return *this;
		}

		inline Vec3 operator/ (const Vec3 &v) const
		{
			Vec3 temp_v;
			temp_v.x= x / v.x;
			temp_v.y= y / v.y;
			temp_v.z= z / v.z;
			return temp_v;
		}

		inline Vec3 operator/ (const Float scalar) const
		{
			Vec3 temp_v;

			temp_v.x= x / scalar;
			temp_v.y= y / scalar;
			temp_v.z= z / scalar;
			return temp_v;
		}

		inline 	Float& operator [] ( unsigned element )
		{
			assert( element < 3 );
			return *(&x+element);
		}

		inline 	Float operator [] ( unsigned element ) const
		{
			assert( element < 3 );
			return *(&x+element);
		}

		inline friend Vec3 operator * ( Float scalar, const Vec3& v )
		{
			Vec3 ret;

			ret.x = scalar * v.x;
			ret.y = scalar * v.y;
			ret.z = scalar * v.z;

			return ret;
		}


		inline bool operator== (const Vec3 &v) const
		{
			return (v.x == x &&  v.y == y && v.z == z);
		}

		inline bool operator!= (const Vec3 &v) const
		{
			return !(v.x == x &&  v.y == y && v.z == z);
		}

		inline Float SquaredLength() const
		{
			return x*x+y*y+z*z;
		}

		inline Float Length() const
		{
			return sqrt(x*x+y*y+z*z);
		}

		inline void Normalize()
		{
			Float dist = Length();

			if(dist > 0)
			{
				x /= dist;
				y /= dist;
				z /= dist;
			}
			else
			{
				x = 1;
				y = 0;
				z = 0;
			}
		}
	
		friend std::ostream& operator << (std::ostream& os, const Vec3& vec)
		{
			os << vec.x << " " << vec.y << " " << vec.z;
			return os;
		}

		friend std::istream& operator >> (std::istream& is, Vec3& vec)
		{
			if(!(is >> vec.x >>  vec.y >> vec.z))
			{
				GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "Failed to parse Vec3","Vec3::>>");
			}

			return is;
		}

		void FastNormalize();
		Float FastLength() const;
		Float FastInvLength() const;
		bool InSphere(Vec3 &v, Float radius) const;
		std::string ToString(const std::string &separator) const;
	};

	
	/**
	* Class representing a Float-vector with 4 elements.
	*/

	class GASSCoreExport Vec4
	{
	public:
		Float x,y,z,w;

		inline Vec4(){}
		inline Vec4(Float _x,Float _y,Float _z,Float _w)
		{
			x = _x;y = _y;z = _z;w = _w;
		}

		inline Vec4 operator+ (const Vec4 &v) const
		{
			Vec4 temp_v;

			temp_v.x= x + v.x;
			temp_v.y= y + v.y;
			temp_v.z= z + v.z;
			temp_v.w= w + v.w;

			return temp_v;
		}

		inline Vec4 operator- (const Vec4 &v) const
		{
			Vec4 temp_v;

			temp_v.x= x - v.x;
			temp_v.y= y - v.y;
			temp_v.z= z - v.z;
			temp_v.w= w - v.w;

			return temp_v;
		}


		inline Vec4 operator- () const
		{
			Vec4 temp_v;

			temp_v.x= - x;
			temp_v.y= - y;
			temp_v.z= - z;
			temp_v.w= - w;

			return temp_v;
		}


		inline Vec4 operator* (const float scalar) const
		{
			Vec4 temp_v;

			temp_v.x= x * scalar;
			temp_v.y= y * scalar;
			temp_v.z= z * scalar;
			temp_v.w= w * scalar;

			return temp_v;
		}


		inline Vec4 operator* (const Vec4 &v) const
		{
			Vec4 temp_v;

			temp_v.x= x * v.x;
			temp_v.y= y * v.y;
			temp_v.z= z * v.z;
			temp_v.w= w * v.w;

			return temp_v;
		}

		inline Vec4 operator/ (const Vec4 &v) const
		{
			Vec4 temp_v;

			temp_v.x= x / v.x;
			temp_v.y= y / v.y;
			temp_v.z= z / v.z;
			temp_v.w= w / v.w;

			return temp_v;
		}

		inline Vec4& operator += ( const Vec4& v )
		{
			x += v.x;
			y += v.y;
			z += v.z;
			w += v.w;
			return *this;
		}

		Vec4 operator* (const Mat4& mat) const
		{
			Vec4 ret;
			ret.x = x*mat.m_Data[0][0] + y*mat.m_Data[1][0] + z*mat.m_Data[2][0] + w*mat.m_Data[3][0];
			ret.y = x*mat.m_Data[0][1] + y*mat.m_Data[1][1] + z*mat.m_Data[2][1] + w*mat.m_Data[3][1];
			ret.z = x*mat.m_Data[0][2] + y*mat.m_Data[1][2] + z*mat.m_Data[2][2] + w*mat.m_Data[3][2];
			ret.w = x*mat.m_Data[0][3] + y*mat.m_Data[1][3] + z*mat.m_Data[2][3] + w*mat.m_Data[3][3];
			return ret;
		}
		inline void Set(Float _x,Float _y,Float _z,Float _w)
		{
			x = _x;y=_y; z=_z; w=_w;
		}
		inline bool operator == (const Vec4 &v)
		{
			if(x == v.x && y == v.y && z == v.z && w == v.w) return true;
			else return false;
		}

		friend std::ostream& operator << (std::ostream& os, const Vec4& vec)
		{
			os << vec.x << " " << vec.y << " " << vec.z  << " " << vec.w;
			return os;
		}

		friend std::istream& operator >> (std::istream& is, Vec4& vec)
		{
			if(!(is >> vec.x >> vec.y >> vec.z >> vec.w))
			{
				GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "Failed to parse Vec4","Vec4::>>");
			}
			return is;
		}
	};
}

#endif // #ifndef VECTOR_HH
