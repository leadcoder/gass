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

#ifndef COMPONENTCONTAINERTEMPLATEFACTORY_HH
#define COMPONENTCONTAINERTEMPLATEFACTORY_HH

#include "Core/Common.h"
#include "Core/ComponentSystem/GASSComponentContainerTemplate.h"
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
		The one and only factory holding component containers.
		This is a singleton implementation of the Factory template.
		By using the singleton pattern each component container
		type can register it self in a easy way.
		A component container have to reigister it self to this
		factory	if it want any type of serialization behavior.
		See Factory class for more information on how to
		do the actual registration.
	*/
	class GASSCoreExport ComponentContainerTemplateFactory : public Factory<ComponentContainerTemplate,std::string,void>
	{
	public:
		ComponentContainerTemplateFactory();
		virtual ~ComponentContainerTemplateFactory();
		static ComponentContainerTemplateFactory* GetPtr();
		static ComponentContainerTemplateFactory& Get();
	protected:
		static ComponentContainerTemplateFactory* m_Instance;
	protected:
	};
}

#endif // #ifndef COMPONENTCONTAINERTEMPLATEFACTORY_HH
