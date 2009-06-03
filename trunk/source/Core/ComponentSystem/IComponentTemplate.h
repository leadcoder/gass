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
	class IComponent;
	typedef boost::shared_ptr<IComponent> ComponentPtr;


	/** 
	Interface that component should derive from 
	if template functionality is desired. 
	This interface should be seen as a extension to 
	the component interface and 
	is by design not intended to be used on it's own. 
	
	A component that is derived from this interface
	has the ability to instance new components 
	of the exact same configuration. This way archetypes of components
	can be created trough configuration files and the goal of   
	more data driven design is achieved.

	@remarks This interface should probably change name if we want to have a 
	separate component-template implementation. By current design it's 
	not possible to separate component-template implementation 
	from component implementation. 
	This choice was made to make new implementations as convenient as possible.
	Separating the implementations will only create two almost identical ones.
	*/
	
	class GASSCoreExport IComponentTemplate
	{
	public:
		virtual ~IComponentTemplate(){}

		virtual ComponentPtr CreateCopy() = 0;
		virtual void Assign(ComponentPtr) = 0;
	protected:
	};

	typedef boost::shared_ptr<IComponentTemplate> ComponentTemplatePtr;
	typedef boost::weak_ptr<IComponentTemplate> ComponentTemplateWeakPtr;
}
