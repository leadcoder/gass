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

#pragma once

#include "Core/Common.h"
#include "Core/Utils/GASSException.h"
#include <assert.h>
#include <math.h>
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

		inline bool operator== (const ColorRGBA &v) const
		{
			return (v.r == r &&  v.g == g &&  v.b == b && v.a == a);
		}

		friend std::ostream& operator << (std::ostream& os, const ColorRGBA& v)
		{
			os << v.r << " " << v.g << " " << v.b << " " << v.a;
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
