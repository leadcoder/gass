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

#include "ArmorComponent.h"
#include "GameMessages.h"
#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Utils/Log.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"

#include "Sim/Systems/Resource/IResourceSystem.h"
#include "Sim/SimEngine.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Scheduling/IRuntimeController.h"
#include "Sim/Systems/Input/ControlSettingsManager.h"
#include "Sim/Systems/Input/ControlSetting.h"


namespace GASS
{
	ArmorComponent::ArmorComponent() : m_Armor(10), m_CurrentArmor(0)
	{
	}

	ArmorComponent::~ArmorComponent()
	{

	}

	void ArmorComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("ArmorComponent",new Creator<ArmorComponent, IComponent>);
		RegisterProperty<float>("Armor", &ArmorComponent::GetArmor, &ArmorComponent::SetArmor);
		RegisterProperty<std::string>("DamageMesh", &ArmorComponent::GetDamageMesh, &ArmorComponent::SetDamageMesh);
		RegisterProperty<std::string>("DamageEffect1", &ArmorComponent::GetDamageEffect1, &ArmorComponent::SetDamageEffect1);
		
	}

	void ArmorComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(ArmorComponent::OnHit,HitMessage,0));
	}

	void ArmorComponent::OnHit(HitMessagePtr message)
	{
		//notify all children also
		IComponentContainer::ComponentContainerIterator cc_iter1 = GetSceneObject()->GetChildren();
		while(cc_iter1.hasMoreElements())
		{
			SceneObjectPtr child = boost::shared_dynamic_cast<GASS::SceneObject>(cc_iter1.getNext());
			child->PostMessage(message);
		}
		std::cout<< "armor=" << m_CurrentArmor << std::endl;
		std::cout<< "armor=" << m_Armor << std::endl;

		if(m_CurrentArmor > 0)
		{
			float damage = message->GetDamage();
			m_CurrentArmor -= damage;
			std::cout<< "damage=" << damage << std::endl;
			if(m_CurrentArmor <= 0)
			{
				//Send armor message
				MessagePtr armor_msg(new OutOfArmorMessage());
				GetSceneObject()->PostMessage(armor_msg);

				//load damage mesh
				if(m_DamageMesh != "")
				{
					MessagePtr mesh_msg(new MeshFileMessage(m_DamageMesh));
					GetSceneObject()->PostMessage(mesh_msg);
				}
				std::cout<< "HIT!!!\n";

				
				message->GetHitDirection();
				//effect
				if(m_DamageEffect1 != "")
				{
					Vec3 vel(0,0,0);
					Quaternion rot = Quaternion::IDENTITY;
					MessagePtr spawn_msg(new SpawnObjectFromTemplateMessage(m_DamageEffect1,message->GetHitPosition(),rot,vel));
					GetSceneObject()->GetSceneObjectManager()->GetScenarioScene()->PostMessage(spawn_msg);
				}
			
			}
		}
	}

	float ArmorComponent::GetArmor() const
	{
		return m_Armor;
	}

	void ArmorComponent::SetArmor(float value)
	{
		m_Armor = value;
		m_CurrentArmor = value;
	}

	
	std::string ArmorComponent::GetDamageMesh() const
	{
		return m_DamageMesh;
	}

	void ArmorComponent::SetDamageMesh(const std::string &value)
	{
		m_DamageMesh = value;
	}

	std::string ArmorComponent::GetDamageEffect1() const
	{
		return m_DamageEffect1;
	}

	void ArmorComponent::SetDamageEffect1(const std::string &value)
	{
		m_DamageEffect1 = value;
	}
	
}
