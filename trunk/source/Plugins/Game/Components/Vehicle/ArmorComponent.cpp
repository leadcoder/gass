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
#include "Core/MessageSystem/Message.h"
#include "Core/Utils/Log.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
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
		
	}

	void ArmorComponent::OnCreate()
	{
		//GetSceneObject()->RegisterForMessage(SceneObject::OBJECT_RM_LOAD_SIM_COMPONENTS, MESSAGE_FUNC(ArmorComponent::OnLoad));
		GetSceneObject()->RegisterForMessage(SceneObject::ObjectMessage(OBJECT_NM_HIT), MESSAGE_FUNC(ArmorComponent::OnHit));
		//register for physics messages on engine?
	}

	void ArmorComponent::OnHit(MessagePtr message)
	{
		if(m_CurrentArmor > 0)
		{
			float damage = boost::any_cast<float>(message->GetData("Damage"));
			m_CurrentArmor -= damage;
			if(m_CurrentArmor <= 0)
			{
				//Send armor message
				MessagePtr armor_msg(new Message(OBJECT_RM_OUT_OF_ARMOR));
				GetSceneObject()->PostMessage(armor_msg);

				//load damage mesh
				MessagePtr mesh_msg(new Message(SceneObject::OBJECT_RM_MESH_PARAMETER));
				mesh_msg->SetData("Parameter",SceneObject::CHANGE_MESH);
				mesh_msg->SetData("MeshName",m_DamageMesh);
				GetSceneObject()->PostMessage(mesh_msg);

				
			

					/*Vec3 vel(0,0,0);
					MessagePtr spawn_msg(new Message(ScenarioScene::SCENARIO_RM_SPAWN_OBJECT_FROM_TEMPLATE));
					spawn_msg->SetData("Template",effect);
					spawn_msg->SetData("Position",m_Pos);
					spawn_msg->SetData("Rotation",m_Rot);
					spawn_msg->SetData("Velocity",vel);
					GetSceneObject()->GetSceneObjectManager()->GetScenarioScene()->PostMessage(spawn_msg);*/
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
}
