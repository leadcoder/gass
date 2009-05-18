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

#include "Core/Common.h"

namespace GASS
{
	class IComponentContainer;
	
	typedef boost::shared_ptr<IComponentContainer> ComponentContainerPtr;
	typedef boost::weak_ptr<IComponentContainer> ComponentContainerWeakPtr;
	

	class GASSCoreExport IComponent
	{
	public:
		virtual ~IComponent(){}
		virtual std::string GetName() const = 0;
		virtual void SetName(const std::string &name) = 0;
		virtual void SetOwner(ComponentContainerPtr owner) = 0;
		virtual ComponentContainerPtr GetOwner() const = 0;
		virtual void OnCreate() = 0;
	};

	typedef boost::shared_ptr<IComponent> ComponentPtr;
	typedef boost::weak_ptr<IComponent> ComponentWeakPtr;

}
