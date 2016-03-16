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
	/**
		All platforms components should be derived from this class 
	*/

	enum PlatformType
	{
		PT_HUMAN,
		PT_CAR,
		PT_TANK,
		PT_TRUCK,
		PT_AIRCRAFT,
		PT_BUILDING,
		PT_WEAPON_SYSTEM,
	};

	class IPlatformComponent
	{
	public:
		virtual ~IPlatformComponent(){}
		virtual PlatformType GetType() const = 0;
		virtual Vec3 GetSize() const = 0;
		virtual Float GetMaxSpeed() const = 0;
	protected:
	};

	typedef GASS_SHARED_PTR<IPlatformComponent> PlatformComponentPtr;
	typedef GASS_WEAK_PTR<IPlatformComponent> PlatformComponentWeakPtr;

}
