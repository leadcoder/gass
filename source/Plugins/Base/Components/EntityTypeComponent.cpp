/****************************************************************************
* This file is part of GASS.                                                *
* See https://github.com/leadcoder/gass                                     *
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
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


#include "EntityTypeComponent.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSceneManagerFactory.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSGraphicsMesh.h"
#include "Sim/GASSComponentFactory.h"

namespace GASS
{

	EntityTypeComponent::EntityTypeComponent() 
			
	{

	}

	EntityTypeComponent::~EntityTypeComponent()
	{

	}

	void EntityTypeComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<EntityTypeComponent>();
		RegisterMember("EntityKind", &EntityTypeComponent::m_EntityKind);
		RegisterMember("Domain", &EntityTypeComponent::m_Domain);
		RegisterMember("CountryCode", &EntityTypeComponent::m_CountryCode);
		RegisterMember("Category", &EntityTypeComponent::m_Category);
		RegisterMember("Subcategory", &EntityTypeComponent::m_Subcategory);
		RegisterMember("Specific", &EntityTypeComponent::m_Specific);
		RegisterMember("Extra", &EntityTypeComponent::m_Extra);
	}

	void EntityTypeComponent::OnInitialize()
	{

	}
}
