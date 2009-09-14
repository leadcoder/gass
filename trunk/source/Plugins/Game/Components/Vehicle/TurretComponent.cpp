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

#include "TurretComponent.h"
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
	TurretComponent::TurretComponent() 
	{

	}

	TurretComponent::~TurretComponent()
	{

	}

	void TurretComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("TurretComponent",new Creator<TurretComponent, IComponent>);
		//RegisterProperty<std::string>("DriveWheel", &TurretComponent::GetDriveWheel, &TurretComponent::SetDriveWheel);
		//RegisterProperty<Vec2>("AnimationSpeedFactor", &TurretComponent::GetAnimationSpeedFactor, &TurretComponent::SetAnimationSpeedFactor);
	}

	void TurretComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(SceneObject::OBJECT_RM_LOAD_SIM_COMPONENTS, MESSAGE_FUNC(TurretComponent::OnLoad));
		//SceneObjectPtr parent = boost::dynamic_pointer_cast<SceneObject>(GetSceneObject()->GetParent());
		GetSceneObject()->RegisterForMessage((SceneObject::ObjectMessage) OBJECT_NM_PLAYER_INPUT, MESSAGE_FUNC(TurretComponent::OnInput));
	}

	void TurretComponent::OnLoad(MessagePtr message)
	{
		

	}

	void TurretComponent::OnInput(MessagePtr message)
	{
		std::string name = boost::any_cast<std::string>(message->GetData("Controller"));
		float value = boost::any_cast<float>(message->GetData("Value"));
		if (name == "Yaw")
		{
			//send rotaion message to physics engine
			MessagePtr force_msg(new Message(SceneObject::OBJECT_RM_PHYSICS_JOINT_PARAMETER));
			force_msg->SetData("Parameter",SceneObject::AXIS1_FORCE);
			force_msg->SetData("Value",float(10));

			MessagePtr vel_msg(new Message(SceneObject::OBJECT_RM_PHYSICS_JOINT_PARAMETER));

			vel_msg->SetData("Parameter",SceneObject::AXIS1_VELOCITY);
			vel_msg->SetData("Value",value);

			GetSceneObject()->PostMessage(force_msg);
			GetSceneObject()->PostMessage(vel_msg);
		}
	}

	
}
