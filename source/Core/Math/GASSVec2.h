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
#include <cassert>
#include <cmath>
#include <iomanip>

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
	* Class representing a vector with 2 elements.
	*/
	template<class TYPE>
	class TVec2
	{
	public:
		TYPE x, y;
		TVec2(TYPE _x, TYPE _y) : x(_x), y(_y)
		{

		}
		TVec2() = default;

		void Set(TYPE _x, TYPE _y)
		{
			x = _x; y = _y;
		}

		TVec2 operator+ (const TVec2 &v) const
		{
			TVec2 ret;
			ret.x = x + v.x;
			ret.y = y + v.y;
			return ret;
		}
		
		TVec2& operator += (const TVec2& v)
		{
			x += v.x;
			y += v.y;
			return *this;
		}

		TVec2 operator- (const TVec2 &v) const
		{
			TVec2 ret;
			ret.x = x - v.x;
			ret.y = y - v.y;
			return ret;
		}

		TVec2 operator- () const
		{
			TVec2 ret;
			ret.x = -x;
			ret.y = -y;
			return ret;
		}

		TVec2& operator -= (const TVec2& v)
		{
			x -= v.x;
			y -= v.y;
			return *this;
		}

		TVec2 operator* (TYPE scalar) const
		{
			TVec2 ret;
			ret.x = x * scalar;
			ret.y = y * scalar;
			return ret;
		}

		TVec2& operator *= (const TYPE scalar)
		{
			x *= scalar;
			y *= scalar;
			return *this;
		}

		friend TVec2 operator * (TYPE scalar, const TVec2& v)
		{
			TVec2 ret;
			ret.x = scalar * v.x;
			ret.y = scalar * v.y;
			return ret;
		}

		TVec2& operator *= (const TVec2& v)
		{
			x *= v.x;
			y *= v.y;
			return *this;
		}

		TVec2 operator* (const TVec2 &v) const
		{
			TVec2 temp_v;
			temp_v.x = x * v.x;
			temp_v.y = y * v.y;
			return temp_v;
		}

		TVec2 operator/ (const TYPE scalar) const
		{
			TVec2 temp_v;
			temp_v.x = x / scalar;
			temp_v.y = y / scalar;
			return temp_v;
		}

		TVec2 operator/ (const TVec2 &v) const
		{
			TVec2 temp_v;
			temp_v.x = x / v.x;
			temp_v.y = y / v.y;
			return temp_v;
		}

		bool operator== (const TVec2 &v) const
		{
			return (v.x == x && v.y == y);
		}

		bool operator!= (const TVec2 &v) const
		{
			return !(v.x == x && v.y == y);
		}

		TYPE& operator [] (unsigned element)
		{
			assert(element < 2);
			return *(&x + element);
		}

		TYPE operator [] (unsigned element) const
		{
			assert(element < 2);
			return *(&x + element);
		}

		bool Equal(const TVec2 &v, TYPE tolerance = std::numeric_limits<TYPE>::epsilon()) const
		{
			return Math::Equal(x, v.x, tolerance) && Math::Equal(y, v.y, tolerance);
		}

		TYPE SquaredLength() const
		{
			return x * x + y * y;
		}

		TYPE Length() const
		{
			return sqrt(x * x + y * y);
		}

		inline TYPE Normalize()
		{
			TYPE dist = Length();

			if (dist > 0)
			{
				x /= dist;
				y /= dist;
			}
			else
			{
				//consider 0, 0 instead or assert
				x = 1;
				y = 0;
			}
			return dist;
		}

		inline TVec2 NormalizedCopy() const
		{
			TVec2 norm_copy(x, y);
			norm_copy.Normalize();
			return norm_copy;
		}

		friend std::ostream& operator << (std::ostream& os, const TVec2& vec)
		{
			os << std::setprecision(std::numeric_limits<TYPE>::digits10 + 1) << vec.x << " " << vec.y;
			return os;
		}

		friend std::istream& operator >> (std::istream& is, TVec2& vec)
		{
			if (!(is >> vec.x >> vec.y))
			{
				GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "Failed to parse vec2 value", "Vec2::>>");
			}
			return is;
		}

		static TVec2<double> Convert(const TVec2<float> &vf)
		{
			return TVec2<double>(vf.x, vf.y);
		}

		static TVec2<float> Convert(const TVec2<double> &vd)
		{
			return TVec2<float>(static_cast<float>(vd.x),
				static_cast<float>(vd.y));
		}

		static void Convert(const TVec2<double> &vd, TVec2<float> &vf)
		{
			vf.x = static_cast<float>(vd.x);
			vf.y = static_cast<float>(vd.y);
		}

		static void Convert(const TVec2<float> &vf, TVec2<double> &vd)
		{
			vd.x = vf.x;
			vd.y = vf.y;
		}
	};

	typedef TVec2<Float> Vec2;
	typedef TVec2<double> Vec2d;
	typedef TVec2<float> Vec2f;
	typedef TVec2<int> Vec2i;
}
