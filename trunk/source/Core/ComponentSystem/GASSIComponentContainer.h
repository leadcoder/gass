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

#include "Core/Common.h"
#include "Core/Utils/GASSIterators.h"

namespace GASS
{
	class IComponent;
	class IComponentContainer;

	typedef SPTR<IComponentContainer> ComponentContainerPtr;
	typedef WPTR<IComponentContainer> ComponentContainerWeakPtr;
	typedef SPTR<IComponent> ComponentPtr;
	typedef WPTR<IComponent> ComponentWeakPtr;


	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Component
	*  @{
	*/

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
		typedef ConstVectorIterator<ComponentVector>  ConstComponentIterator;
		typedef std::vector<ComponentContainerPtr> ComponentContainerVector;
		typedef VectorIterator<ComponentContainerVector> ComponentContainerIterator;
		typedef ConstVectorIterator<ComponentContainerVector> ConstComponentContainerIterator;
	
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

		virtual ConstComponentContainerIterator GetChildren() const = 0;
		/**
			Get component by name,
			only serach this containers components and first one is returned
		*/
		virtual ComponentPtr GetComponent(const std::string &name) const = 0;

		/**
			Get all components owned by this container
		*/
		virtual ComponentIterator GetComponents() = 0;

		virtual ConstComponentIterator GetComponents() const = 0;
		
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
