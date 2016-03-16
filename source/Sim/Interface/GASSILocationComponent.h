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
#include "Core/Math/GASSQuaternion.h"

namespace GASS
{

	/**
		Interface that all location components should be derived from.
		
		Note that interaction with this interface during RTC update is undefined 
		if running GASS in multi-threaded mode. Interaction with components should 
		instead be done through messages.
	*/

	class ILocationComponent
	{
	public:
		virtual ~ILocationComponent(){}
		virtual void SetPosition(const Vec3 &value) = 0;
		virtual Vec3 GetPosition() const = 0;
		virtual Vec3 GetWorldPosition() const = 0;
		virtual void SetEulerRotation(const Vec3 &value) = 0;
		virtual Vec3 GetEulerRotation() const = 0;
		virtual Quaternion GetRotation() const = 0;
		virtual Quaternion GetWorldRotation() const = 0;
		virtual Vec3 GetScale() const = 0;
	protected:
	};

	typedef GASS_WEAK_PTR<ILocationComponent> LocationComponentWeakPtr;
	typedef GASS_SHARED_PTR<ILocationComponent> LocationComponentPtr;
}
