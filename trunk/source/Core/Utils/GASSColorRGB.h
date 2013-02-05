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

#ifndef GASS_COLOR_RGB_H
#define GASS_COLOR_RGB_H

#include "Core/Common.h"
#include <assert.h>
#include <math.h>
namespace GASS
{

	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Utils
	*  @{
	*/

	/**
	* Class that holds color values.
	*/
	class GASSCoreExport ColorRGB
	{
	public:
		Float r,g,b;
		inline ColorRGB(Float _r,Float _g,Float _b)
		{
			r = _r;g = _g;b = _b;
		}
		inline ColorRGB(){}
		inline void Set(Float _r,Float _g,Float _b)
		{
			r = _r;g = _g;b = _b;
		}

		inline bool operator== (const ColorRGB &v) const
		{
			return (v.r == r &&  v.g == g &&  v.b == b);
		}

		friend std::ostream& operator << (std::ostream& os, const ColorRGB& v)
		{
			os << v.r << " " << v.g << " " << v.b;
			return os;
		}

		friend std::istream& operator >> (std::istream& is, ColorRGB& v)
		{
			is >> v.r >>  v.g >>  v.b;
			return is;
		}
	};
}
#endif 
