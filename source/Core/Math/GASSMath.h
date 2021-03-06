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
#include <cmath>

#define GASS_PI 3.1415926535898
#define GASS_HALF_PI 1.5707963267949

namespace GASS
{
	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Math
	*  @{
	*/

	/**\class Math
	@brief Class with a couple of math functions.
	*/
	class GASSCoreExport Math
	{
	public:
		/**
		@brief Convert radians to degrees.
		@param rad Number of radians, as a float.
		@return Number of degrees.
		*/
		constexpr inline static float Rad2Deg(float rad);
		constexpr inline static double Rad2Deg(double rad);

		/**
		@brief Convert degrees to radians.
		@param deg Number of degrees, as a float.
		@return Number of radians.
		*/
		constexpr inline static float Deg2Rad(float deg);
		constexpr inline static double Deg2Rad(double deg);
		
		template<class TYPE> inline static TYPE ASin(TYPE fValue);

		template<class TYPE>
		inline static bool Equal(TYPE v1, TYPE v2, TYPE tolerance = std::numeric_limits<TYPE>::epsilon());
	
		/**
		Get float random value
		@param start Start of random span
		@param end End of random span
		@return random value between start and end
		*/
		template<class TYPE> inline static TYPE RandomValue(TYPE start, TYPE end);
		
		/**
		Inverted root square function
		*/
		static inline float InvSqrt(float x)
		{
#ifdef GASS_USE_FAST_INV
			float xhalf = 0.5f*x;
			int i = *(int*)&x;
			i = 0x5f3759df - (i >> 1); // This line hides a LOT of math!
			x = *(float*)&i;
			x = x*(1.5f - xhalf*x*x); // repeat this statement for a better approximation
			return x;
#else
			return 1.0f / sqrt(x);
#endif
		}

		template<class TYPE>
		inline static TYPE Clamp(TYPE value, TYPE min_v, TYPE max_v);
	private:
	};

	//implementation
	constexpr float Math::Rad2Deg(float rad)
	{
		return 360.0f * rad / (static_cast<float>(GASS_PI*2.0));
	}

	constexpr double Math::Rad2Deg(double rad)
	{
		return 360.0 * rad / (GASS_PI*2.0);
	}

	constexpr float Math::Deg2Rad(float deg)
	{
		return static_cast<float>(2.0*GASS_PI) * deg / 360.0f;
	}

	constexpr double Math::Deg2Rad(double deg)
	{
		return 2.0*GASS_PI * deg / 360.0;
	}

	template<class TYPE>
	TYPE Math::ASin(TYPE value)
	{
		if (-1.0 < value)
		{
			if (value < 1.0)
				return asin(value);
			else
				return static_cast<TYPE>(GASS_HALF_PI);
		}
		else
		{
			return -static_cast<TYPE>(GASS_HALF_PI);
		}
	}

	template<class TYPE>
	bool Math::Equal(TYPE v1, TYPE v2, TYPE tolerance)
	{
		return (fabs(v1 - v2) < tolerance);
	}

	template<class TYPE>
	TYPE Math::RandomValue(TYPE start, TYPE end)
	{
		const double span = end - start;
		const double norm_rand = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
		const TYPE ret_value = start + static_cast<TYPE>(norm_rand * span);
		return ret_value;
	}

	template<class TYPE>
	TYPE Math::Clamp(TYPE value, TYPE min_v, TYPE max_v)
	{
		return std::min<TYPE>(std::max<TYPE>(value, min_v), max_v);
	}
}
