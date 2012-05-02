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

#ifndef ICOMPONENT_HH
#define ICOMPONENT_HH

#include "Core/Common.h"

namespace GASS
{
	class IComponentContainer;

	typedef boost::shared_ptr<IComponentContainer> ComponentContainerPtr;
	typedef boost::weak_ptr<IComponentContainer> ComponentContainerWeakPtr;

	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Component
	*  @{

	*/
	/**
		Interface that every component have to implement
		A component is the core building block in the component system,
		here we find actual functionality like vehicle models,
		graphics, network capability etc. All components that should work together is
		owned by a ComponentContainer. A ComponentContainer is the owner of 
		the components and has it's functionality in it's components.
	*/
	class GASSCoreExport IComponent
	{
	public:
		virtual ~IComponent(){}

		/**
			Return the name of the component
		*/
		virtual std::string GetName() const = 0;

		/**
			Set the name of the component
		*/
		virtual void SetName(const std::string &name) = 0;

		/**
			Set the owner of the component
		*/
		virtual void SetOwner(ComponentContainerPtr owner) = 0;

		/**
			Get the owner of the component
		*/
		virtual ComponentContainerPtr GetOwner() const = 0;
	};

	typedef boost::shared_ptr<IComponent> ComponentPtr;
	typedef boost::weak_ptr<IComponent> ComponentWeakPtr;

}
#endif // #ifndef ICOMPONENT_HH
