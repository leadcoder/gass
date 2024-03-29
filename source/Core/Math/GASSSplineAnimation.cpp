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

#include "Core/Math/GASSMath.h"
#include "Core/Math/GASSSplineAnimation.h"
#include "Core/Math/GASSQuaternion.h"

namespace GASS
{
	SplineAnimation::SplineAnimation()  
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

	void SplineAnimation::BuildInterpolationSplines() 
	{
		m_Spline.Clear();
		m_Spline.SetAutoCalc(true);
		for (auto & node : m_NodeVector)
		{
			m_Spline.AddPoint(node.m_Pos);
		}
		m_Spline.RecalcTangents();
	

		Vec3 last_spline_point(0,0,0);
		bool first = true;
		Float seg_dist = 0;
		Float tot_dist = 0;
		const Float step_size = 1.0f / m_NumericSteps;
		for(int  i = 0; i < static_cast<int>(m_NodeVector.size()); i++)
		{
			for(Float t = 0; t <= (1 + step_size); t += step_size)
			{
				const Vec3 spline_point =  m_Spline.Interpolate(i, t);
				if(!first)
				{
					const Vec3  dir = spline_point - last_spline_point;
					seg_dist += dir.Length();
					
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
		transformation.MakeIdentity();
		double time = 0;
		int index = 0;
		GetIndexAndTime(desired_distance,time,index);
		const Vec3 pos = m_Spline.Interpolate(index, time);
		const Vec3 rot_pos = m_Spline.Interpolate(index, time + 0.01);

		Vec3 left,dir,up;
		dir = rot_pos - pos;
		dir.Normalize();

		left = Vec3::Cross(up_dir,dir);
		left.Normalize();
		
		up = Vec3::Cross(dir,left);
		up.Normalize();

		transformation.MakeIdentity();
		transformation.SetRotationByAxis(left,up,dir);
		transformation.SetTranslation(pos);
		return transformation;
	}


	Vec3 SplineAnimation::GetVectorMask(const Vec3 &up_dir) const
	{
		const Vec3 abs_up_dir(fabs(up_dir.x),fabs(up_dir.y),fabs(up_dir.z));
		Vec3 ret(1,1,1);
		ret = (ret - abs_up_dir);
		return ret;
	}


	Mat4 SplineAnimation::GetTransformation(Float desired_distance, const Vec3 &up_dir, Float pitch_weight)
	{
		Mat4 transformation;
		transformation.MakeIdentity();
		double time = 0;
		int index = 0;
		GetIndexAndTime(desired_distance,time,index);
		const Vec3 pos = m_Spline.Interpolate(index, time);
		const Vec3 rot_pos = m_Spline.Interpolate(index, time + 0.01);

		Vec3 left,dir,up;
		dir = rot_pos - pos;
		dir.Normalize();

		const Vec3 mask = GetVectorMask(up_dir);

		Vec3 mask_dir = mask*dir;
		mask_dir.Normalize();

		dir = dir*pitch_weight +  mask_dir*(1-pitch_weight);
		dir.Normalize();

		left = Vec3::Cross(up_dir,dir);
		left.Normalize();
		
		up = Vec3::Cross(dir,left);
		up.Normalize();

		transformation.MakeIdentity();
		transformation.SetRotationByAxis(left, up, dir);
		transformation.SetTranslation(pos);
		return transformation;
	}

	Mat4 SplineAnimation::GetSkewTransformation(Float desired_distance, const Vec3 &up_dir, Float skew_weight)
	{
		Mat4 transformation;
		transformation.MakeIdentity();
		double time = 0;
		int index = 0;
		GetIndexAndTime(desired_distance,time,index);
		const Vec3 pos = m_Spline.Interpolate(index, time);
		const Vec3 rot_pos = m_Spline.Interpolate(index, time + 0.01);

		Vec3 left,dir,up;
		dir = rot_pos - pos;
		dir.Normalize();

		const Vec3 mask = GetVectorMask(up_dir);

		Vec3 mask_dir = mask*dir;
		mask_dir.Normalize();

		left = Vec3::Cross(up_dir,dir);
		left.Normalize();
		
		up = Vec3::Cross(dir,left);
		up.Normalize();

		up = up_dir*skew_weight +  up*(1-skew_weight);
		dir = mask_dir*skew_weight +  dir*(1-skew_weight);
		
		
		transformation.MakeIdentity();
		transformation.SetRotationByAxis(left,up,dir);
		transformation.SetTranslation(pos);
		return transformation;
	}

	Vec3 SplineAnimation::GetPosition(Float desired_distance)
	{
		Float time = 0;
		int index = 0;
		GetIndexAndTime(desired_distance,time,index);
		Vec3 pos = m_Spline.Interpolate(index, time);
		return pos;
	}


	void SplineAnimation::GetIndexAndTime(Float desired_distance,Float &time, int &index)
	{
		Vec3 last_spline_point(0,0,0);
		bool first = true;
		Float dist = 0;
		Float step_size = 1.0f / m_NumericSteps;
		
		for(int  i = 0; i < static_cast<int>(m_NodeVector.size()); i++)
		{
			if(desired_distance <= m_NodeVector[i].m_SegmentDistance + dist)
			{
				for(Float t = 0; t <= (1 + step_size); t += step_size)
				{
					const Vec3 spline_point =  m_Spline.Interpolate(i, t);
					if(!first)
					{
						const Vec3  dir = spline_point - last_spline_point;
						const Float seg_len = dir.Length();
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
		index =static_cast<int>(m_NodeVector.size()-1);
	}
}
