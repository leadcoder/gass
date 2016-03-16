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
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSRay.h"

namespace GASS
{

	/**
		Camera interface that all camera components should be derived from
		
		Note that interaction with this interface during RTC update is undefined 
		if running GASS in multi-threaded mode. Interaction with components should 
		instead be done through messages.
	*/
	
	class ICameraComponent
	{
	public:
		virtual ~ICameraComponent(){}
		virtual bool GetCameraToViewportRay(float screenx, float screeny, Ray &ray) const = 0;
	protected:
	};

	typedef GASS_WEAK_PTR<ICameraComponent> CameraComponentWeakPtr;
	typedef GASS_SHARED_PTR<ICameraComponent> CameraComponentPtr;
}
