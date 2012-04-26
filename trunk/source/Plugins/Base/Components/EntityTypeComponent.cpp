/****************************************************************************
*                                                                           *
* GASS                                                                *
* Copyright (C)2003 - 2005 Johan Hedstrom                                   *
* Email: GASS@gmail.com                                               *
* Web page: http://n00b.dyndns.org/GASS                               *
*                                                                           *
* GASS is only used with knowledge from the author. This software     *
* is not allowed to redistribute without permission from the author.        *
* For further license information, please turn to the product home page or  *
* contact author. Abuse against the GASS license is prohibited by law.*
*                                                                           *
*****************************************************************************/ 

#include <boost/bind.hpp>
#include "EntityTypeComponent.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/Scene/GASSScene.h"
#include "Sim/Scene/GASSSceneObject.h"
#include "Sim/Scene/GASSSceneManagerFactory.h"
#include "Sim/Scene/GASSSceneObjectManager.h"
#include "Sim/Components/Graphics/GASSILocationComponent.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "Sim/Components/Graphics/GASSMeshData.h"


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

	void EntityTypeComponent::OnCreate()
	{

	}
}
