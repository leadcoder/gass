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
	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Utils
	*  @{
	*/

	/**
	* Class that holds RGB color values.
	*/
	class ColorRGB
	{
	public:
		double r,g,b;
		inline ColorRGB(Float _r,Float _g,Float _b)
		{
			r = _r;g = _g;b = _b;
		}
		inline ColorRGB(){}
		inline void Set(Float _r,Float _g,Float _b)
		{
			r = _r;g = _g;b = _b;
		}

		inline bool operator== (const ColorRGB &c) const
		{
			return (c.r == r && c.g == g && c.b == b);
		}

		inline bool operator!= (const ColorRGB &c) const
		{
			return !(*this == c);
		}

		friend std::ostream& operator << (std::ostream& os, const ColorRGB& v)
		{
			os << std::setprecision(std::numeric_limits<double>::digits10 + 1) << v.r << " " << v.g << " " << v.b;
			return os;
		}

		friend std::istream& operator >> (std::istream& is, ColorRGB& v)
		{
			if(!(is >> v.r >>  v.g >>  v.b))
			{
				GASS_EXCEPT(Exception::ERR_INVALIDPARAMS, "Failed to parse ColorRGB","ColorRGB::>>");
			}
			return is;
		}
	};
}
 
