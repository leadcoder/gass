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
#include "Sim/GASSEulerRotation.h"


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
		virtual ~ILocationComponent() {}
		virtual Vec3 GetPosition() const = 0;
		virtual void SetPosition(const Vec3 &value) = 0;

		virtual Vec3 GetWorldPosition() const = 0;
		virtual void SetWorldPosition(const Vec3 &value) = 0;

		virtual void SetEulerRotation(const EulerRotation &value) = 0;
		virtual EulerRotation GetEulerRotation() const = 0;

		virtual Quaternion GetRotation() const = 0;
		virtual void SetRotation(const Quaternion& value) = 0;

		virtual Quaternion GetWorldRotation() const = 0;
		virtual void SetWorldRotation(const Quaternion& value) = 0;

		virtual Vec3 GetScale() const = 0;
		virtual void SetScale(const Vec3 &value) = 0;
		
		
		/** Get value indicating if location is relative to first LocationComponent that is found above in scene tree hierarchy */
		virtual bool GetAttachToParent() const = 0;

		/** Set if location should be relative to first LocationComponent that is
		* found above in scene tree hierarchy (true) or world location (false) */
		virtual void SetAttachToParent(bool value) = 0;

		virtual void SetVisible(bool value) = 0;
		virtual bool GetVisible() const = 0;

	protected:
	};

	typedef GASS_WEAK_PTR<ILocationComponent> LocationComponentWeakPtr;
	typedef GASS_SHARED_PTR<ILocationComponent> LocationComponentPtr;
}
