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
		TYPE x,y;
		inline TVec2(TYPE _x, TYPE _y)
		{
			x = _x;y = _y;
		}
		inline TVec2(){}
		inline void Set(TYPE _x, TYPE _y)
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
		TVec2 operator* (TYPE scalar) const
		{
			TVec2 ret;
			ret.x = x*scalar;
			ret.y = y*scalar;
			return ret;
		}

		inline bool operator== (const TVec2 &v) const
		{
			return (v.x == x &&  v.y == y);
		}

		friend std::ostream& operator << (std::ostream& os, const TVec2& vec)
		{
			os << std::setprecision(std::numeric_limits<TYPE>::digits10 + 1) << vec.x << " " << vec.y;
			return os;
		}

		friend std::istream& operator >> (std::istream& is, TVec2& vec)
		{
			if(!(is >> vec.x >>  vec.y))
			{
				GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "Failed to parse vec2 value","Vec2::>>");
			}
			return is;
		}
	};

	typedef TVec2<Float> Vec2;
	typedef TVec2<double> Vec2d;
	typedef TVec2<float> Vec2f;
	typedef TVec2<int> Vec2i;
}
