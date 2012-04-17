/****************************************************************************
*                                                                           *
* HiFiEngine                                                                *
* Copyright (C)2003 - 2005 Johan Hedstrom                                   *
* Email: hifiengine@gmail.com                                               *
* Web page: http://n00b.dyndns.org/HiFiEngine                               *
*                                                                           *
* HiFiEngine is only used with knowledge from the author. This software     *
* is not allowed to redistribute without permission from the author.        *
* For further license information, please turn to the product home page or  *
* contact author. Abuse against the HiFiEngine license is prohibited by law.*
*                                                                           *
*****************************************************************************/ 

#ifndef GASS_SPLINE_ANIMATION_H
#define GASS_SPLINE_ANIMATION_H


#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSSpline.h"
#include "Core/Math/GASSRotationSpline.h"
#include "Core/Math/GASSQuaternion.h"
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
		void AutoCalulateRotation();
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