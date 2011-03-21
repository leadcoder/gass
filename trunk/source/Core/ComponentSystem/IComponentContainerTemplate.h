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

#ifndef ICOMPONENTCONTAINERTEMPLATE_HH
#define ICOMPONENTCONTAINERTEMPLATE_HH

#include "Core/Common.h"
#include "Core/Utils/Iterators.h"

namespace GASS
{

	class IComponent;
	class IComponentContainer;
	class IComponentContainerTemplate;
	class IComponentContainerTemplateManager;
	typedef boost::shared_ptr<IComponent> ComponentPtr;
	typedef boost::shared_ptr<IComponentContainerTemplateManager> ComponentContainerTemplateManagerPtr;
	typedef boost::shared_ptr<IComponentContainerTemplate> ComponentContainerTemplatePtr;
	typedef boost::weak_ptr<IComponentContainerTemplate> ComponentContainerTemplateWeakPtr;
	typedef boost::shared_ptr<IComponentContainer> ComponentContainerPtr;

	/**
	Interface that component-containers should derive from
	if template functionality is desired.
	This interface should be seen as a extension to
	the component-container interface and
	is by design not intended to be used on it's own.

	A component-container that is derived from this interface
	has the ability to instance new component-containers
	of the exact same configuration. This way archetypes of objects
	can be created trough configuration files and the goal of
	more data driven design is achieved.

	@remarks This interface should probably change name if we want to have a
	separate component-container-template implementation. By current design it's
	not possible to separate component-container-template implementation
	from component-container implementation.
	This choice was made to make new implementations as convenient as possible,
	otherwise the two implementations would have been nearly identical.
	*/


	class GASSCoreExport IComponentContainerTemplate
	{
	public:
		typedef std::vector<ComponentPtr> ComponentVector;
		typedef VectorIterator<ComponentVector>  ComponentIterator;
		typedef std::vector<ComponentContainerTemplatePtr> ComponentContainerTemplateVector;
		typedef VectorIterator<ComponentContainerTemplateVector> ComponentContainerTemplateIterator;

		virtual ~IComponentContainerTemplate(){}

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
		virtual void AddChild(ComponentContainerTemplatePtr child) = 0;

		/**
			Remove a child component conatiner
		*/
		virtual void RemoveChild(ComponentContainerTemplatePtr child) = 0;

		/**
			Add a component to this component container
		*/
		virtual void AddComponent(ComponentPtr comp) = 0;

		/**
			Get child component containers templates,
			this will only return the ones owned by this container i.e.
			no grandchildren will be returned
		*/
		virtual ComponentContainerTemplateIterator GetChildren() = 0;
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
		virtual ComponentContainerTemplatePtr GetParent()const  = 0;

		/**
			Set parent component container
		*/
		virtual void SetParent(ComponentContainerTemplateWeakPtr parent) = 0;

		/**
			Return a component container created from this template.
		*/
		//Manager argument needed if we want to used inheritance
		virtual ComponentContainerPtr CreateComponentContainer(int &part_id, ComponentContainerTemplateManagerPtr manager) = 0;

		/**
			Return a component container created from this template
			without including child component containers.
		*/
		//virtual ComponentContainerPtr CreateCopy() = 0;
	protected:
	};

	typedef boost::shared_ptr<IComponentContainerTemplate> ComponentContainerTemplatePtr;
	typedef boost::weak_ptr<IComponentContainerTemplate> ComponentContainerTemplateWeakPtr;
}
#endif // #ifndef ICOMPONENTCONTAINERTEMPLATE_HH
