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
#include <vector>
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSSphere.h"
#include "Core/Math/GASSMatrix.h"
#include "Core/Math/GASSPolygon.h"
#include "Core/Math/GASSLineSegment.h"
#include "Core/Math/GASSRay.h"

#undef min
#undef max

namespace GASS
{

	/**
		2D Axis Aligned Box usually holding bounding information. 
		The box is represented by a max 2d-coordinate and min 2d-coordinate
	*/

	template<class TYPE>
	class TAARect
	{
	public:
		TAARect();
		TAARect(const TVec2<TYPE> &min_pos, const TVec2<TYPE> &max_pos);
		
		/**
			Merge this bounding box with other
		*/
		void Union(const TAARect &TAARect);
		
		/**
			Extend bounding box to include 2D point
		*/
		void Union(const TVec2<TYPE> &point);

		/**
		Extend bounding box from polygon
		*/
		/**
			Get bounding box size
		*/
		TVec2<TYPE> GetSize() const;


		/**
			Check that Max is greater then Min
		*/
		bool IsValid() const;
		
		/**
			Get all corner points of this box
		*/
		std::vector<TVec2<TYPE> > GetCorners() const;

		/**
			Check intersection with other bounding box
		*/
		bool BoxIntersect(const TAARect &box) const;

		/**
			Check if point is inside bounds
		*/
		bool PointInside(const TVec2<TYPE> &point) const;

		bool operator== (const TAARect &rect) const;
		

		//public for fast access
		TVec2<TYPE> Max;
		TVec2<TYPE> Min;
	};

	typedef TAARect<double> AARectd;
	typedef TAARect<float> AARectf;
	typedef TAARect<int> AARecti;
	
	template<class TYPE>
	TAARect<TYPE>::TAARect()
	{
		Max.x = Max.y =  -std::numeric_limits<TYPE>::max();
		Min.x = Min.y =  std::numeric_limits<TYPE>::max();
	}

	template<class TYPE>
	TAARect<TYPE>::TAARect(const TVec2<TYPE> &min_pos, const TVec2<TYPE> &max_pos) : Max(max_pos), Min(min_pos)
	{
	}

	template<class TYPE>
	bool TAARect<TYPE>::IsValid() const
	{
		return (Max.x >  Min.x &&
			Max.y >  Min.y);
	}

	template<class TYPE>
	void TAARect<TYPE>::Union(const TVec2<TYPE> &point)
	{
		if (point.x > Max.x) Max.x = point.x;
		if (point.x < Min.x) Min.x = point.x;

		if (point.y > Max.y) Max.y = point.y;
		if (point.y < Min.y) Min.y = point.y;
	}

	template<class TYPE>
	void TAARect<TYPE>::Union(const TAARect<TYPE> &TAABox)
	{
		if (TAABox.Max.x > Max.x) Max.x = TAABox.Max.x;
		if (TAABox.Min.x < Min.x) Min.x = TAABox.Min.x;

		if (TAABox.Max.y > Max.y) Max.y = TAABox.Max.y;
		if (TAABox.Min.y < Min.y) Min.y = TAABox.Min.y;

	}

	template<class TYPE>
	std::vector<TVec2<TYPE> > TAARect<TYPE>::GetCorners() const
	{
		std::vector< TVec2<TYPE> > ret;
		ret.push_back(Min);
		ret.push_back(TVec2<TYPE>(Max.x, Min.y));
		ret.push_back(Max);
		ret.push_back(TVec2<TYPE>(Min.x, Max.y));
		return ret;
	}
	
	template<class TYPE>
	bool TAARect<TYPE>::PointInside(const TVec2<TYPE> &point) const
	{
		if (point.x < Max.x && point.x > Min.x &&
			point.y < Max.y && point.y > Min.y)
			return true;
		else 
			return false;
	}

	template<class TYPE>
	bool TAARect<TYPE>::BoxIntersect(const TAARect<TYPE> &box) const
	{
		return (Min.x <= box.Max.x &&
			Max.x >= box.Min.x &&
			Min.y <= box.Max.y &&
			Max.y >= box.Min.y);
	}

	template<class TYPE>
	TVec2<TYPE> TAARect<TYPE>::GetSize() const
	{
		return (Max - Min);
	}

	template<class TYPE>
	bool TAARect<TYPE>::operator== (const TAARect<TYPE> &rect) const
	{
		return (rect.Min == Min && rect.Max == Max);
	}
}