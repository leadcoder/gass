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
#include "Sim/Scenario/Scene/Messages/PhysicsSceneObjectMessages.h"
#include "Sim/Components/Graphics/ILocationComponent.h"
#include "Sim/Components/Network/INetworkComponent.h"

#include "Sim/Systems/Resource/IResourceSystem.h"
#include "Sim/SimEngine.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Scheduling/IRuntimeController.h"
#include "Sim/Systems/Input/ControlSettingsManager.h"
#include "Sim/Systems/Input/ControlSetting.h"


namespace GASS
{
	ArmorComponent::ArmorComponent() : m_Armor(10), m_CurrentArmor(0), m_OutOfArmorForce(0)
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
		RegisterProperty<float>("OutOfArmorForce", &ArmorComponent::GetOutOfArmorForce, &ArmorComponent::SetOutOfArmorForce);
		
	}

	void ArmorComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(ArmorComponent::OnLoad,LoadGameComponentsMessage,1));
	}
	void ArmorComponent::OnLoad(LoadGameComponentsMessagePtr message)
	{
		NetworkComponentPtr nc = GetSceneObject()->GetFirstComponentByClass<INetworkComponent>();
		if(nc && !nc->IsRemote())
		{
			GetSceneObject()->RegisterForMessage(REG_TMESS(ArmorComponent::OnHit,HitMessage,0));
		}
		GetSceneObject()->RegisterForMessage(REG_TMESS(ArmorComponent::OnOutOfArmor,OutOfArmorMessage ,0));
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
		//std::cout<< "armor=" << m_CurrentArmor << std::endl;
		//std::cout<< "armor=" << m_Armor << std::endl;

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


				if(m_OutOfArmorForce > 0)
				{
					Vec3 force = Vec3(0,1,0)*m_OutOfArmorForce;
					MessagePtr force_msg(new PhysicsBodyMessage(PhysicsBodyMessage::FORCE,force));
					GetSceneObject()->PostMessage(force_msg);
				}
			}
		}
	}

	void ArmorComponent::OnOutOfArmor(OutOfArmorMessagePtr message)
	{
		m_CurrentArmor = 0;
		//load damage mesh
		if(m_DamageMesh != "")
		{
			MessagePtr mesh_msg(new MeshFileMessage(m_DamageMesh));
			GetSceneObject()->PostMessage(mesh_msg);
		}
		//std::cout<< "Dead!!!\n";
				
		//message->GetHitDirection();
		//effect
		if(m_DamageEffect1 != "")
		{
			LocationComponentPtr location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
			Vec3 vel(0,0,0);
			Quaternion rot = Quaternion::IDENTITY;
			Vec3 pos = location->GetWorldPosition();
			MessagePtr spawn_msg(new SpawnObjectFromTemplateMessage(m_DamageEffect1,pos,rot,vel));
			GetSceneObject()->GetSceneObjectManager()->GetScenarioScene()->PostMessage(spawn_msg);
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
