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

#ifndef GASS_COMPONENT_FACTORY_H
#define GASS_COMPONENT_FACTORY_H

#include "Core/Common.h"
#include "Core/ComponentSystem/GASSComponent.h"
#include "Core/Utils/GASSFactory.h"

namespace GASS
{

	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Component
	*  @{

	*/
	/**
		The one and only factory holding components.
		This is a singleton implementation of the Factory template.
		By using the singleton pattern each new component type can
		register it	self in a easy way.
		A component have to reigister it self to this
		factory	if it want any type of serialization behavior.
		See Factory class for more information on how to
		do the actual registration.
	*/
	class GASSCoreExport ComponentFactory : public Factory<Component,std::string,void>
	{
	public:
		ComponentFactory();
		virtual ~ComponentFactory();
		static ComponentFactory* GetPtr();
		static ComponentFactory& Get();
	protected:
		static ComponentFactory* m_Instance;
	protected:
	};
}

#endif // #ifndef COMPONENTFACTORY_HH
