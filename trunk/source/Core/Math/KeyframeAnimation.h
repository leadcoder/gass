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

#ifndef KEYFRAMEANIMATION_HH
#define KEYFRAMEANIMATION_HH

#include "Core/Math/Vector.h"
#include "Core/Math/Spline.h"
#include "Core/Math/RotationSpline.h"
#include "Core/Math/Quaternion.h"
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
		};
		Vec3 m_Pos;
		Quaternion m_Rot;
		Vec3 m_Scale;
		float m_Time;
		bool m_UsePos;
		bool m_UseRot;
		bool m_UseTime;
	};

	class GASSCoreExport KeyframeAnimation
	{
	public:
		KeyframeAnimation(void);
		~KeyframeAnimation(void);
		float GetKeys(float timePos, const Key* &key1, const Key* &key2, int *firstKeyIndex) const;
		Key GetInterpolatedKeyFrame(float timeIndex);
		void BuildInterpolationSplines(void);
		void AutoCalulateRotation() ;
		Key GetBoneKeyFrame(float weight,int index0, int index1) const;

	private:
		std::vector<Key> m_KeyVector;
		int m_InterpolateMode;
		int m_Loop;
		Spline m_PositionSpline;
		RotationSpline m_RotationSpline;
		Spline m_ScaleSpline;
		float m_Length;
		bool m_SplineBuildNeeded;
		bool m_RotToPath;
	};
}
#endif // #ifndef KEYFRAMEANIMATION_HH
