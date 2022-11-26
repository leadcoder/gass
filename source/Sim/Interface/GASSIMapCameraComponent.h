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

namespace GASS
{
	class IMapCameraComponent
	{
		GASS_DECLARE_CLASS_AS_INTERFACE(IMapCameraComponent)
	public:
		virtual double GetPitch() const = 0;
		virtual void SetPitch(double value) = 0;
		virtual double GetHeading() const = 0;
		virtual void SetHeading(double value) = 0;
		virtual double GetRange() const = 0;
		virtual void SetRange(double value) = 0;
		virtual void FlyToObject(SceneObjectPtr obj) = 0;
		virtual void SetPosition(const GASS::Vec3& pos) = 0;
		virtual void SetRotation(const GASS::Quaternion& rot) = 0;
	};
	typedef GASS_SHARED_PTR<IMapCameraComponent> MapCameraComponentPtr;
	typedef GASS_WEAK_PTR<IMapCameraComponent> MapCameraComponentWeakPtr;
}
