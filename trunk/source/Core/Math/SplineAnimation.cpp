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

#include "Core/Math/CoreMath.h"
#include "Core/Math/SplineAnimation.h"
#include "Core/Math/Quaternion.h"

namespace GASS
{
	SplineAnimation::SplineAnimation(void)
	{
		m_Length = 0;
		m_NumericSteps = 25;
	}

	SplineAnimation::~SplineAnimation(void)
	{

	}

	void SplineAnimation::AddNode(const Vec3 &pos)
	{
		SplineNode node;
		node.m_Pos = pos;
		node.m_SegmentDistance = 0;
		m_NodeVector.push_back(node);
		BuildInterpolationSplines();
	}

	void SplineAnimation::BuildInterpolationSplines(void) 
	{
		m_Spline.Clear();
		m_Spline.SetAutoCalc(true);
		int i = 0;
		for (; i < m_NodeVector.size(); i++)
		{
			SplineNode* splineNode = &m_NodeVector[i];
			m_Spline.AddPoint(m_NodeVector[i].m_Pos);
		}
		m_Spline.RecalcTangents();
	

		Vec3 last_spline_point;
		bool first = true;
		Float seg_dist = 0;
		Float tot_dist = 0;
		Float step_size = 1.0f / m_NumericSteps;
		for(int  i = 0; i < m_NodeVector.size(); i++)
		{
			for(Float t = 0; t <= (1 + step_size); t += step_size)
			{
				Vec3 spline_point =  m_Spline.Interpolate(i, t);
				if(!first)
				{
					Vec3  dir = spline_point - last_spline_point;
					seg_dist += dir.FastLength();
					
				}
				last_spline_point = spline_point;
				first = false;
			}
			m_NodeVector[i].m_SegmentDistance = seg_dist;
			tot_dist += seg_dist;
			seg_dist = 0;
		}
		m_Length = tot_dist;
	}

	Mat4 SplineAnimation::GetTransformation(Float desired_distance, const Vec3 &up_dir)
	{
		Mat4 transformation;
		transformation.Identity();
		Float time;
		int index;
		GetIndexAndTime(desired_distance,time,index);
		Vec3 pos = m_Spline.Interpolate(index, time);
		Vec3 rot_pos = m_Spline.Interpolate(index, time + 0.01);

		Vec3 left,dir,up;
		dir = rot_pos - pos;
		dir.Normalize();

		left = Math::Cross(up_dir,dir);
		left.Normalize();
		
		//left.x = dir.z;
		//left.y = 0;
		//left.z = -dir.x;
		up = Math::Cross(dir,left);
		up.Normalize();

		transformation.Identity();
		transformation.SetViewDirVector(dir);
		transformation.SetUpVector(up);
		transformation.SetRightVector(left);
		transformation.SetTranslation(pos.x,pos.y,pos.z);
		return transformation;
	}


	Vec3 SplineAnimation::GetVectorMask(const Vec3 &up_dir)
	{
		Vec3 abs_up_dir(fabs(up_dir.x),fabs(up_dir.y),fabs(up_dir.z));
		Vec3 ret(1,1,1);
		ret = (ret - abs_up_dir);
		return ret;
	}


	Mat4 SplineAnimation::GetTransformation(Float desired_distance, const Vec3 &up_dir, Float pitch_weight)
	{
		Mat4 transformation;
		transformation.Identity();
		Float time;
		int index;
		GetIndexAndTime(desired_distance,time,index);
		Vec3 pos = m_Spline.Interpolate(index, time);
		Vec3 rot_pos = m_Spline.Interpolate(index, time + 0.01);

		Vec3 left,dir,up;
		dir = rot_pos - pos;
		dir.Normalize();

		Vec3 mask = GetVectorMask(up_dir);

		Vec3 mask_dir = mask*dir;
		mask_dir.Normalize();

		dir = dir*pitch_weight +  mask_dir*(1-pitch_weight);
		dir.Normalize();

		left = Math::Cross(up_dir,dir);
		left.Normalize();
		
		//left.x = dir.z;
		//left.y = 0;
		//left.z = -dir.x;
		up = Math::Cross(dir,left);
		up.Normalize();

		transformation.Identity();
		transformation.SetViewDirVector(dir);
		transformation.SetUpVector(up);
		transformation.SetRightVector(left);
		transformation.SetTranslation(pos.x,pos.y,pos.z);
		return transformation;
	}

	Mat4 SplineAnimation::GetSkewTransformation(Float desired_distance, const Vec3 &up_dir, Float skew_weight)
	{
		Mat4 transformation;
		transformation.Identity();
		Float time;
		int index;
		GetIndexAndTime(desired_distance,time,index);
		Vec3 pos = m_Spline.Interpolate(index, time);
		Vec3 rot_pos = m_Spline.Interpolate(index, time + 0.01);

		Vec3 left,dir,up;
		dir = rot_pos - pos;
		dir.Normalize();

		Vec3 mask = GetVectorMask(up_dir);

		Vec3 mask_dir = mask*dir;
		mask_dir.Normalize();

		left = Math::Cross(up_dir,dir);
		left.Normalize();
		
		up = Math::Cross(dir,left);
		up.Normalize();

		up = up_dir*skew_weight +  up*(1-skew_weight);
		dir = mask_dir*skew_weight +  dir*(1-skew_weight);
		
		
		transformation.Identity();
		transformation.SetViewDirVector(dir);
		transformation.SetUpVector(up);
		transformation.SetRightVector(left);
		transformation.SetTranslation(pos.x,pos.y,pos.z);
		return transformation;
	}

	Vec3 SplineAnimation::GetPosition(Float desired_distance)
	{
		Float time;
		int index;
		GetIndexAndTime(desired_distance,time,index);
		Vec3 pos = m_Spline.Interpolate(index, time);
		return pos;
	}


	void SplineAnimation::GetIndexAndTime(Float desired_distance,Float &time, int &index)
	{
		Vec3 last_spline_point;
		bool first = true;
		Float dist = 0;
		Float step_size = 1.0f / m_NumericSteps;
		
		for(int  i = 0; i < m_NodeVector.size(); i++)
		{
			if(desired_distance <= m_NodeVector[i].m_SegmentDistance + dist)
			{
				//step_size = m_NodeVector[i].m_SegmentDistance/seg_size;
				for(Float t = 0; t <= (1 + step_size); t += step_size)
				{
					Vec3 spline_point =  m_Spline.Interpolate(i, t);
					if(!first)
					{
						Vec3  dir = spline_point - last_spline_point;
						Float seg_len = dir.FastLength();
						dist += seg_len;
						if(dist > desired_distance)
						{
							Float percent = (desired_distance - (dist -  seg_len))/seg_len;
							time = (t  - step_size)  + percent*step_size;
							index = i;
							return;
						}
					}
					first = false;
					last_spline_point = spline_point;
				}
			}
			else
			{
				dist += m_NodeVector[i].m_SegmentDistance;
			}
		}

		//end of spline
		time = 0;
		index = m_NodeVector.size()-1;
	}
}
