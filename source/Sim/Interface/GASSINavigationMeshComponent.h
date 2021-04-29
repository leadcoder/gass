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

#include "Sim/GASSCommon.h"
#include "Sim/Interface/GASSINavigationComponent.h"
namespace GASS
{
	class INavigationMeshComponent : public INavigationComponent
	{
		GASS_DECLARE_CLASS_AS_INTERFACE(INavigationMeshComponent)
	public:
		virtual bool GetClosestPointOnMeshForPlatform(const PlatformType platform_type, const GASS::Vec2 &in_pos, const float search_radius, GASS::Vec3 &out_pos) const = 0;
		virtual bool Raycast(const PlatformType platform_type, const GASS::Vec3 &from_pos, const GASS::Vec3 &to_pos, GASS::Vec3 &hit_pos) const = 0;
		//virtual Vec3 GetRandomPoint() const = 0;
		//virtual bool GetRandomPointInCircle(const Vec3 &circle_center, const float radius, Vec3 &point)  const = 0;
		//virtual bool IsPointInside(const Vec3 &point) const = 0;
	};
	typedef GASS_SHARED_PTR<INavigationMeshComponent> NavigationMeshComponentPtr;
	typedef GASS_WEAK_PTR<INavigationMeshComponent> NavigationMeshComponentWeakPtr;
}
