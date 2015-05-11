/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                        *
*                                                                           *
* Copyright (c) 2008-2015 GASS team. See Contributors.txt for details.      *
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

#include "Core/Math/GASSMatrix.h"
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSQuaternion.h"

namespace GASS
{
	class PxConvert
	{
	public:
		static inline physx::PxVec2				ToPx(const Vec2 &v) {return physx::PxVec2(v.x,v.y);}
		static inline physx::PxVec3				ToPx(const Vec3 &v) {return physx::PxVec3(v.x,v.y,v.z);}
		static inline physx::PxVec4				ToPx(const Vec4 &v) {return physx::PxVec4(v.x,v.y,v.z,v.w);}
		static inline physx::PxQuat				ToPx(const Quaternion &q) {return physx::PxQuat(q.x,q.y,q.z,q.w);}
		
		static inline Vec2						ToGASS(const physx::PxVec2 &v) {return Vec2(v.x,v.y);}
		static inline Vec3						ToGASS(const physx::PxVec3 &v) {return Vec3(v.x,v.y,v.z);}
		static inline Vec4						ToGASS(const physx::PxVec4 &v) {return Vec4(v.x,v.y,v.z,v.w);}
		static inline Quaternion				ToGASS(const physx::PxQuat &q) {return Quaternion(q.w,q.x,q.y,q.z);}
	};
}


