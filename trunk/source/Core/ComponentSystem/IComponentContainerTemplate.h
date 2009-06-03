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
	/*

	*/
	class IComponent;
	class IComponentContainer;
	class IComponentContainerTemplateManager;
	typedef boost::shared_ptr<IComponentContainerTemplateManager> ComponentContainerTemplateManagerPtr;
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
		virtual ~IComponentContainerTemplate(){}

		/**
			Return the name of this template
		*/
		virtual std::string GetName() const  = 0;
		
		/**
			Return a component container created from this template.
		*/
		//Manager argument needed if we want to used inheritance
		virtual ComponentContainerPtr CreateComponentContainer(int &part_id, ComponentContainerTemplateManagerPtr manager) = 0;

		/**
			Return a component container created from this template 
			without including child component containers.
		*/
		virtual ComponentContainerPtr CreateCopy() = 0;
	protected:
	};

	typedef boost::shared_ptr<IComponentContainerTemplate> ComponentContainerTemplatePtr;
	typedef boost::weak_ptr<IComponentContainerTemplate> ComponentContainerTemplateWeakPtr;
}
