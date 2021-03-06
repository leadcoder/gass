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
#include <iomanip>

namespace GASS
{
	/**
	* Class that holds RGBA color values.
	*/

	class ColorRGBA
	{
	public:
		Float r,g,b,a;
		inline ColorRGBA(Float _r,Float _g,Float _b, Float _a)
		{
			r = _r;g = _g;b = _b;a=_a;
		}
		inline ColorRGBA(){}
		inline void Set(Float _r,Float _g,Float _b, Float _a)
		{
			r = _r;g = _g;b = _b;a=_a;
		}

		inline bool operator== (const ColorRGBA &c) const
		{
			return (c.r == r && c.g == g && c.b == b && c.a == a);
		}

		inline bool operator!= (const ColorRGBA &c) const
		{
			return !(*this == c);
		}

		friend std::ostream& operator << (std::ostream& os, const ColorRGBA& v)
		{
			os << std::setprecision(std::numeric_limits<Float>::digits10 + 1)  << v.r << " " << v.g << " " << v.b << " " << v.a;
			return os;
		}

		friend std::istream& operator >> (std::istream& is, ColorRGBA& v)
		{
			if(!(is >> v.r >>  v.g >>  v.b >>  v.a))
			{
				GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "Failed to parse ColorRGB","ColorRGB::>>");
			}
			return is;
		}
	};
}
