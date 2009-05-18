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
	
	class GASSCoreExport IComponentContainerTemplate
	{
	public:
		virtual ~IComponentContainerTemplate(){}
		virtual std::string GetName() const  = 0;
		//Manager argument needed if we want to used inheritance
		virtual ComponentContainerPtr CreateComponentContainer(int &part_id, ComponentContainerTemplateManagerPtr manager) = 0;
		virtual ComponentContainerPtr CreateCopy() = 0;
	protected:
	};

	typedef boost::shared_ptr<IComponentContainerTemplate> ComponentContainerTemplatePtr;
	typedef boost::weak_ptr<IComponentContainerTemplate> ComponentContainerTemplateWeakPtr;
}
