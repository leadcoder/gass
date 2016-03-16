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
#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponent.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSceneManagerFactory.h"

#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSGraphicsMesh.h"


namespace GASS
{

	EntityTypeComponent::EntityTypeComponent() : m_EntityKind (0),
			m_Domain (0),
			m_CountryCode(0), 
			m_Category (0),
			m_Subcategory (0),
			m_Specific (0),
			m_Extra(0)
	{

	}

	EntityTypeComponent::~EntityTypeComponent()
	{

	}

	void EntityTypeComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("EntityTypeComponent",new Creator<EntityTypeComponent, Component>);
		REG_PROPERTY(int,EntityKind,EntityTypeComponent)
		REG_PROPERTY(int,Domain,EntityTypeComponent)
		REG_PROPERTY(int,CountryCode,EntityTypeComponent)
		REG_PROPERTY(int,Category,EntityTypeComponent)
		REG_PROPERTY(int,Subcategory,EntityTypeComponent)
		REG_PROPERTY(int,Specific,EntityTypeComponent)
		REG_PROPERTY(int,Extra,EntityTypeComponent)
	}

	void EntityTypeComponent::OnInitialize()
	{

	}
}
