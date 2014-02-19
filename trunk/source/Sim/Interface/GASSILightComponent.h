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

#pragma once

#include "Sim/GASSCommon.h"
#include "Core/Utils/GASSEnumBinder.h"
namespace GASS
{
	enum LightType
	{
		LT_DIRECTIONAL,
		LT_SPOT,
		LT_POINT
	};

	START_ENUM_BINDER(LightType,LightTypeBinder)
		BIND(LT_DIRECTIONAL)
		BIND(LT_SPOT)
		BIND(LT_POINT)
	END_ENUM_BINDER(LightType,LightTypeBinder)
	
	

	/**
		Light source interface that all light components should be derived from.
		
		Note that interaction with this interface during RTC update is undefined 
		if running GASS in multi-threaded mode. Interaction with components should 
		instead be done through messages.

		At present this interface is only used for searching for all objects that 
		has light components in a scene. 
	*/
	class ILightComponent
	{
	public:
		virtual ~ILightComponent(){}
	protected:
	};
}
