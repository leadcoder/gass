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

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include "Core/Common.h"

namespace GASS
{
	class IComponent;
	class IComponentContainer;

	typedef boost::shared_ptr<IComponentContainer> ComponentContainerPtr;
	typedef boost::weak_ptr<IComponentContainer> ComponentContainerWeakPtr;
	typedef boost::shared_ptr<IComponent> ComponentPtr;
	typedef boost::weak_ptr<IComponent> ComponentWeakPtr;
	
	
	class GASSCoreExport IComponentContainer
	{
	public:
		typedef std::vector<ComponentPtr> ComponentVector;
		typedef std::vector<ComponentContainerPtr> ComponentContainerVector;
	public:
		virtual ~IComponentContainer(){}
		virtual std::string GetName() const = 0;
		virtual void SetName(const std::string &name) = 0;
		virtual void OnCreate() = 0;
		virtual void AddChild(ComponentContainerPtr child) = 0;
		virtual void AddComponent(ComponentPtr comp) = 0;
		virtual ComponentContainerVector GetChildren() = 0;
		virtual ComponentPtr GetComponent(const std::string &name) = 0;
		virtual ComponentVector GetComponents() = 0;
		virtual ComponentContainerPtr GetParent()const  = 0;
		virtual void SetParent(ComponentContainerWeakPtr parent) = 0;
	};


}
