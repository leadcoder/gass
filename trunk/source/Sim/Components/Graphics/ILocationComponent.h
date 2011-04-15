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

#include "Sim/Common.h"
#include "Core/Math/Quaternion.h"

namespace GASS
{
	class GASSExport ILocationComponent
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

	typedef boost::weak_ptr<ILocationComponent> LocationComponentWeakPtr;
	typedef boost::shared_ptr<ILocationComponent> LocationComponentPtr;
}
