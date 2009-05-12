/****************************************************************************
* This file is part of GASS.                                                *
* See http://sourceforge.net/projects/gass/                                 *
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

#include "Core/Reflection/BaseReflectionObject.h"
#include <string>

namespace GASS
{
	class ISystemManager;
	typedef std::string SystemType;

	class GASSCoreExport ISystem
	{
	public:
		virtual ~ISystem(){}
		virtual std::string GetName() const = 0;
		virtual void SetName(const std::string &name) = 0;
		virtual ISystemManager* GetOwner() const = 0;
		virtual void SetOwner(ISystemManager* owner)= 0;
		virtual void OnCreate() = 0;
		virtual SystemType GetSystemType() = 0;
	};

	typedef boost::shared_ptr<ISystem> SystemPtr;
}
