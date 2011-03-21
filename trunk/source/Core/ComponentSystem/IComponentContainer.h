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

#ifndef ICOMPONENTCONTAINER_HH
#define ICOMPONENTCONTAINER_HH

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include "Core/Common.h"
#include "Core/Utils/Iterators.h"

namespace GASS
{
	class IComponent;
	class IComponentContainer;

	typedef boost::shared_ptr<IComponentContainer> ComponentContainerPtr;
	typedef boost::weak_ptr<IComponentContainer> ComponentContainerWeakPtr;
	typedef boost::shared_ptr<IComponent> ComponentPtr;
	typedef boost::weak_ptr<IComponent> ComponentWeakPtr;


	/**
		Interface that a component container should implement
		A component container is a owner of components and child
		components containers. This way a tree structure can be created
		with component containers where each node in the tree represented by a
		component container that can have it's own set of components
		giving the node unique functionality
	*/
	class GASSCoreExport IComponentContainer
	{
	public:
		typedef std::vector<ComponentPtr> ComponentVector;
		typedef VectorIterator<ComponentVector>  ComponentIterator;
		typedef std::vector<ComponentContainerPtr> ComponentContainerVector;
		typedef VectorIterator<ComponentContainerVector> ComponentContainerIterator;
	public:
		virtual ~IComponentContainer(){}
		/**
		Get component container name
		*/
		virtual std::string GetName() const = 0;

		/**
		Set component container name
		*/
		virtual void SetName(const std::string &name) = 0;

		/**
			This function is called when a
			component conatiner is instantiated
		*/
		virtual void OnCreate() = 0;

		/**
			Add a child component conatiner.
		*/
		virtual void AddChild(ComponentContainerPtr child) = 0;

		/**
			Remove a child component conatiner
		*/
		virtual void RemoveChild(ComponentContainerPtr child) = 0;

		/**
			Add a component to this component container
		*/
		virtual void AddComponent(ComponentPtr comp) = 0;

		/**
			Get child component containers,
			this will only return the ones owned by this container i.e.
			no grandchildren will be returned
		*/
		virtual ComponentContainerIterator GetChildren() = 0;
		/**
			Get component by name,
			only serach this containers components and first one is returned
		*/
		virtual ComponentPtr GetComponent(const std::string &name) const = 0;

		/**
			Get all components owned by this container
		*/
		virtual ComponentIterator GetComponents() = 0;
		/**
			Get possible parent component container
		*/
		virtual ComponentContainerPtr GetParent()const  = 0;

		/**
			Set parent component container
		*/
		virtual void SetParent(ComponentContainerWeakPtr parent) = 0;

		/**
			Get template name
		*/
		virtual std::string GetTemplateName() const = 0;
		/**
			Set template name
		*/
		virtual void SetTemplateName(const std::string& name) = 0;
	};


}
#endif // #ifndef ICOMPONENTCONTAINER_HH
