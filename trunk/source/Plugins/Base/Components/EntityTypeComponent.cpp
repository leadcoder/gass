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


#include "EntityTypeComponent.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSceneManagerFactory.h"

#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSMeshData.h"


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
		ComponentFactory::GetPtr()->Register("EntityTypeComponent",new Creator<EntityTypeComponent, IComponent>);
		REG_ATTRIBUTE(int,EntityKind,EntityTypeComponent)
		REG_ATTRIBUTE(int,Domain,EntityTypeComponent)
		REG_ATTRIBUTE(int,CountryCode,EntityTypeComponent)
		REG_ATTRIBUTE(int,Category,EntityTypeComponent)
		REG_ATTRIBUTE(int,Subcategory,EntityTypeComponent)
		REG_ATTRIBUTE(int,Specific,EntityTypeComponent)
		REG_ATTRIBUTE(int,Extra,EntityTypeComponent)
	}

	void EntityTypeComponent::OnInitialize()
	{

	}
}
