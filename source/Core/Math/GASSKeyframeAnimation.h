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

#ifndef GASS_KEYFRAMEANIMATION_H
#define GASS_KEYFRAMEANIMATION_H

#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSSpline.h"
#include "Core/Math/GASSRotationSpline.h"
#include "Core/Math/GASSQuaternion.h"
#include <vector>

namespace GASS
{
	enum
	{
		IM_LINEAR,
		IM_SPLINE
	};

	class GASSCoreExport Key
	{
	public:
		Key()
		{
			m_Pos.Set(0,0,0);
			m_Rot.x = m_Rot.y = m_Rot.z = m_Rot.w = 0;
			m_Scale.Set(1,1,1);
			m_UsePos = false;
			m_UseRot = false;
			m_Time = 0;
			m_UseTime = false;
		}

		friend std::ostream& operator << (std::ostream& os, const Key& key)
		{
			os << std::setprecision(std::numeric_limits<Float>::digits10 + 1)  << key.m_Time << " " << key.m_Pos << " " << key.m_Rot << " " << key.m_Scale;
			return os;
		}

		friend std::istream& operator >> (std::istream& os, Key& key)
		{
			os >> key.m_Time >> key.m_Pos >> key.m_Rot >> key.m_Scale;
			return os;
		}
		Vec3 m_Pos;
		Quaternion m_Rot;
		Vec3 m_Scale;
		Float m_Time;
		bool m_UsePos;
		bool m_UseRot;
		bool m_UseTime;
	};

	
	class GASSCoreExport KeyframeAnimation
	{
	public:
		KeyframeAnimation(void);
		~KeyframeAnimation(void);
		Float GetKeys(Float timePos, const Key* &key1, const Key* &key2, int *firstKeyIndex) const;
		Key GetInterpolatedKeyFrame(Float timeIndex);
		void BuildInterpolationSplines(void);
		void AutoCalulateRotation() ;
		Key GetBoneKeyFrame(Float weight,int index0, int index1) const;
	//public for fast access
		std::vector<Key> m_KeyVector;
		int m_InterpolateMode;
		int m_Loop;
		Spline m_PositionSpline;
		RotationSpline m_RotationSpline;
		Spline m_ScaleSpline;
		Float m_Length;
		bool m_SplineBuildNeeded;
		bool m_RotToPath;
	};
}
#endif // #ifndef KEYFRAMEANIMATION_HH
