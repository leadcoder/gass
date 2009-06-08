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

#ifndef VECTOR_HH
#define VECTOR_HH

#include "Core/Common.h"
#include "Core/Math/Matrix.h"
#include "Core/Math/CoreMath.h"
#include <iostream>
#include <assert.h>
#include <math.h>

#ifdef WIN32
# pragma warning (disable : 4244)
#endif /* WIN32 */


/*NOTE THAT THIS FILE IS BASED ON MATERIAL FROM: Ogre3d

LGPL tex goes here?
-----------------------------------------------------------------------------
*/

namespace GASS
{

	/**\class Vec2
	* \brief Class that holds two Floats.
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

		friend std::ostream& operator << (std::ostream& os, const Vec2& vec)
		{
			os << vec.x << " " << vec.y;
			return os;
		}

		friend std::istream& operator >> (std::istream& os, Vec2& vec)
		{
			os >> vec.x >>  vec.y;
			return os;
		}
	};

	/**\class Vec2
	* \brief Class that holds two integer.
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

		friend std::istream& operator >> (std::istream& os, Vec2i& vec)
		{
			os >> vec.x  >> vec.y;
			return os;
		}


	};

	/**\class Vec3
	* \brief Class representing a Float-vector with 3 elements.
	*/
	class GASSCoreExport Vec3
	{
	public:
		/**\var Float x,y,z
		* \brief Contains the data of the vector.
		*/
		union
		{
			Float x,h;
		};
		union
		{
			Float y,p;
		};
		union
		{
			Float z,r;
		};

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

		inline Vec3 operator* (const Float scalar) const
		{
			Vec3 temp_v;

			temp_v.x= x * scalar;
			temp_v.y= y * scalar;
			temp_v.z= z * scalar;

			return temp_v;
		}

		inline Vec3 operator* (const Vec3 &v) const
		{
			Vec3 temp_v;

			temp_v.x= x * v.x;
			temp_v.y= y * v.y;
			temp_v.z= z * v.z;
			return temp_v;
		}

		inline Vec3 operator/ (const Vec3 &v) const
		{
			Vec3 temp_v;
			temp_v.x= x / v.x;
			temp_v.y= y / v.y;
			temp_v.z= z / v.z;
			return temp_v;
		}

		inline 	Float* operator [] ( unsigned element )
		{
			assert( element < 3 );
			if(element == 0) return &x;
			else if(element == 1) return &y;
			else return &z;
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
			return (Float) sqrt(x*x+y*y+z*z);
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

		void FastNormalize();
		Float FastLength() const;
		Float FastInvLength() const;


		/**\fn InRange(Vec3 &v, Float radius);
		* \brief Check if vector is inside a sphere.
		* \param v Center of sphere.
		* \param radius Radius of sphere.
		* \return True if inside sphere.
		*/
		bool InSphere(Vec3 &v, Float radius) const;
		std::string ToString();
		//char CheckRange2(Vec3 &v, Float radius) const;
		/**\fn Print();
		* \brief Print the vector to the text console (command window).
		*/
		//void Print() const;
		//std::string ToString();
		//Float Length() const;
		//void Normalize();
		//void Transform3(const Mat4 &m);

		friend std::ostream& operator << (std::ostream& os, const Vec3& vec)
		{
			os << vec.x << " " << vec.y << " " << vec.z;
			return os;
		}

		friend std::istream& operator >> (std::istream& os, Vec3& vec)
		{
			os >> vec.x >>  vec.y >> vec.z;
			return os;
		}

	};


	class GASSCoreExport Vec4
	{
	public:
		/**\var Float x,y,z
		* \brief Contains the data of the vector.
		*/
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

		friend std::istream& operator >> (std::istream& os, Vec4& vec)
		{
			os >> vec.x >> vec.y >> vec.z >> vec.w;
			return os;
		}
	};
}

#endif // #ifndef VECTOR_HH
