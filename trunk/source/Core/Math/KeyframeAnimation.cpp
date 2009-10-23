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

#include "Core/Math/CoreMath.h"
#include "Core/Math/KeyframeAnimation.h"
#include "Core/Math/Quaternion.h"
//#include "Core/Helpers/Misc.h"

namespace GASS
{
KeyframeAnimation::KeyframeAnimation(void)
{
	m_SplineBuildNeeded = true;
	m_InterpolateMode = IM_SPLINE;
	//m_InterpolateMode = IM_LINEAR;
	m_Length = 0;
	m_Loop = 0;
	m_RotToPath = false;
}

KeyframeAnimation::~KeyframeAnimation(void)
{
}

void KeyframeAnimation::AutoCalulateRotation()
{
	Vec3 dir,up,left;
	up.Set(0,1,0);
	Vec3 vPoint;

	bool closed = false;

	if(m_KeyVector[m_KeyVector.size()-1].m_Pos == m_KeyVector[0].m_Pos) closed = true;

	for(int  i = 0; i < m_KeyVector.size(); i++)
	{
		
		Quaternion q;
		if(!closed && i == m_KeyVector.size()-1 && i > 0)
		{
			vPoint =  m_PositionSpline.Interpolate(i-1, 0.99);
			dir = vPoint - m_KeyVector[i].m_Pos;
			dir = -dir;
		}
		else
		{
			vPoint =  m_PositionSpline.Interpolate(i, 0.01);
			dir = vPoint - m_KeyVector[i].m_Pos;
		}
		dir.Normalize();
		left = Math::Cross(up,dir);
		left.Normalize();

		q.FromAxes(left,up,dir);
		m_KeyVector[i].m_Rot = q;
	}
}

/*
void KeyframeAnimation::Render()
{
	RenderKeys();
	Float steps = 25;

	glBegin(GL_LINE_STRIP);							

		for(int  i = 0; i < m_KeyVector.size(); i++)
		{
			//Key *k1 = &m_KeyVector[i];
			//Key *k2 = &m_KeyVector[i%m_KeyVector.size()];
			//Float t = GetKeys(timeIndex, k1, k2, NULL);
			
			for(Float t = 0; t <= (1 + (1.0f / steps)); t += 1.0f / steps)
			{
				Vec3 vPoint =  m_PositionSpline.Interpolate(i, t);
				//Vec3 vPoint = PointOnCurve(g_vStartPoint, g_vControlPoint1, g_vControlPoint2, g_vEndPoint, t);
				glVertex3f(vPoint.x, vPoint.y, vPoint.z);
			}
		}

	glEnd();
	
}


void KeyframeAnimation::RenderKeys()
{
	for(int i = 0; i < m_KeyVector.size(); i++)
	{
		glPushMatrix();
		glTranslatef(m_KeyVector[i].m_Pos.x,
					m_KeyVector[i].m_Pos.y,
					m_KeyVector[i].m_Pos.z);
		//just draw sphere
		glutSolidSphere(0.001,8,8);
		glPopMatrix();		
	}
}

*/

Float KeyframeAnimation::GetKeys(Float timePos, const Key* &key1, const Key* &key2, int *firstKeyIndex)  const
{
	int i;
	int firstIndex = -1;

	key1 = NULL;
	key2 = NULL;
	if(m_KeyVector.size() == 0)
	{
		return 0;
	}
	else if(m_KeyVector.size() == 1)
	{
		key1 = &m_KeyVector[0];
		return 0;
	}

	// Wrap time 
	//if(m_Loop)
	{
		while (timePos > m_Length)
		{
			timePos -= m_Length;
		}

		while(timePos < 0)
		{
			timePos += m_Length;
		}
	}

	// Find last keyframe before or on current time
	i = 0;
	while (i < m_KeyVector.size() && m_KeyVector[i].m_Time <= timePos)
	{
		key1 = &m_KeyVector[i];
		++i;
		++firstIndex;
	}

	// Trap case where there is no key before this time (problem with animation config)
	// In this case use the first key anyway and pretend it's time index 0
	if (firstIndex == -1)
	{
		key1 = &m_KeyVector[0];
		++firstIndex;
	}

	// Fill index of the first key
	if (firstKeyIndex != NULL)
	{
		*firstKeyIndex = firstIndex;
	}

	// Parametric time
	// t1 = time of previous keyframe
	// t2 = time of next keyframe 
	Float t1, t2;

	// Find first keyframe after the time
	// If no next keyframe, wrap back to first

	//if(m_Loop)
	{
		if (i == m_KeyVector.size())
		{
			key2 = &m_KeyVector[0];
			t2 = m_Length;
		}
		else
		{
			key2 = &m_KeyVector[i];
			t2 = key2->m_Time;
		}
	}

	t1 = key1->m_Time;

	if (t1 == t2)
	{
		// Same KeyFrame (only one)
		return 0.0;
	}
	else
	{
		return (timePos - t1) / (t2 - t1);
	}
}

Key KeyframeAnimation::GetBoneKeyFrame(Float weight,int index0, int index1) const
{
	assert(!(index0 < 0 || index0 >=  m_KeyVector.size()));
	assert(!(index1 < 0 || index1 >=  m_KeyVector.size()));
	Key kret;// = Key();
	const Key *k1 = &m_KeyVector[index0];
	const Key *k2 = &m_KeyVector[index1];
	if (weight == 0.0)
	{
		return (*k1);
	}
	else if (weight == 1.0)
	{
		return (*k2);
	}
	else
	{
		//Linear interpolate
		kret.m_Rot = Quaternion::Slerp2(weight,k1->m_Rot, k2->m_Rot);
		//kret.m_Rot = k1->m_Rot;
		Vec3 base = k1->m_Pos;
		kret.m_Pos = ( base + ((k2->m_Pos - base) * weight) );

		// Scale
		base = k1->m_Scale;
		kret.m_Scale = ( base + ((k2->m_Scale - base) * weight));
	}
	return kret;
}


Key KeyframeAnimation::GetInterpolatedKeyFrame(Float timeIndex)
{
	// Return value
	Key kret = Key();

	// Keyframe pointers
	const Key *k1 = NULL;
	const Key *k2 = NULL;
	int firstKeyIndex;

	Float t = GetKeys(timeIndex, k1, k2, &firstKeyIndex);

	if (t == 0.0)
	{
		// Just use k1
		kret = *k1;
	}
	else
	{
		// Interpolate by t
		
		Vec3 base;
		switch(m_InterpolateMode)
		{
		case IM_LINEAR:
			// Interpolate linearly
			// Rotation
			//kret.setRotation( Quaternion::Slerp(t, k1->getRotation(), k2->getRotation()) );
			kret.m_Rot = Quaternion::Slerp(t, k1->m_Rot, k2->m_Rot);

			// Translation
			base = k1->m_Pos;
			kret.m_Pos = ( base + ((k2->m_Pos - base) * t) );

			// Scale
			base = k1->m_Scale;
			kret.m_Scale = ( base + ((k2->m_Scale - base) * t) );
			break;

		case IM_SPLINE:
			// Spline interpolation

			// Build splines if required
			if (m_SplineBuildNeeded)
			{
				BuildInterpolationSplines();
			}

			// Translation
			kret.m_Pos =  m_PositionSpline.Interpolate(firstKeyIndex, t);

			// Scale
			kret.m_Scale = m_ScaleSpline.Interpolate(firstKeyIndex, t);

			// Rotation
			if(m_RotToPath)
			{
				Vec3 left,dir,up;
				Vec3 vPoint =  m_PositionSpline.Interpolate(firstKeyIndex, t+0.01);
				dir = vPoint - kret.m_Pos;
				dir.Normalize();
				left.x = dir.z;
				left.y = 0;
				left.z = -dir.x;
				up = Math::Cross(dir,left);
				up.Normalize();
				kret.m_Rot.FromAxes(left,up,dir);
			}
			else 
			{
				//kret.m_Rot = k2->m_Rot;
				//kret.m_Rot = Quaternion::Slerp(t, k1->m_Rot, k2->m_Rot);
				kret.m_Rot = Quaternion::Slerp2(t,k1->m_Rot, k2->m_Rot);
				//kret.m_Rot = m_RotationSpline.Interpolate(firstKeyIndex, t);
			}
		

			

			break;
		}

	}
	return kret;

}


 void KeyframeAnimation::BuildInterpolationSplines(void) 
    {
        // Don't calc automatically, do it on request at the end
        //mPositionSpline.setAutoCalculate(false);
        //mRotationSpline.setAutoCalculate(false);
        //mScaleSpline.setAutoCalculate(false);

        m_PositionSpline.Clear();
        m_RotationSpline.Clear();
        m_ScaleSpline.Clear();

		int i = 0;
        for (; i < m_KeyVector.size(); i++)
        {
			Key* key = &m_KeyVector[i];
            m_PositionSpline.AddPoint(m_KeyVector[i].m_Pos);
            m_ScaleSpline.AddPoint(m_KeyVector[i].m_Scale);
        }

		m_PositionSpline.RecalcTangents();
        m_ScaleSpline.RecalcTangents();

		//AutoCalulateRotation();
		for (i = 0; i < m_KeyVector.size(); i++)
        {
			m_RotationSpline.AddPoint(m_KeyVector[i].m_Rot);
		}

		m_RotationSpline.RecalcTangents();

        m_SplineBuildNeeded = false;
    }
}
