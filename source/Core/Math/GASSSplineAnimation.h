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

#ifndef GASS_SPLINE_ANIMATION_H
#define GASS_SPLINE_ANIMATION_H


#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSSpline.h"
#include <vector>

namespace GASS
{
	class GASSCoreExport SplineNode
	{
	public:
		SplineNode()
		{
			m_Pos.Set(0,0,0);
			m_SegmentDistance = 0;
		};
		Vec3 m_Pos;
		Float m_SegmentDistance;
	};

	class GASSCoreExport SplineAnimation
	{
	public:
		SplineAnimation(void);
		~SplineAnimation(void);
		void BuildInterpolationSplines(void);
		void AddNode(const Vec3 &pos);
		Mat4 GetTransformation(Float desired_distance, const Vec3 &up_dir);
		Vec3 GetVectorMask(const Vec3 &up_dir);
		Mat4 GetTransformation(Float desired_distance, const Vec3 &up_dir, Float pitch_weight);
		Mat4 GetSkewTransformation(Float desired_distance, const Vec3 &up_dir, Float skew_weight);
	
	
		Vec3 GetPosition(Float desired_distance);
		void GetIndexAndTime(Float desired_distance,Float &time, int &index);
	
		std::vector<SplineNode> m_NodeVector;
		Spline m_Spline;
		Float m_Length;
		Float m_NumericSteps;
	};
}

#endif