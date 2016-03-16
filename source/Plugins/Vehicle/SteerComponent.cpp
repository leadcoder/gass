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

#include "SteerComponent.h"
#include "Sim/Messages/GASSPlatformMessages.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"

#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"

#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"


namespace GASS
{
	SteerComponent::SteerComponent() : m_SteerForce(100),m_MaxSteerVelocity(1),m_CurrentAngle(0),m_DesiredAngle(0),m_MaxSteerAngle(45),m_Speed(1)
	{

	}

	SteerComponent::~SteerComponent()
	{

	}

	void SteerComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("SteerComponent",new Creator<SteerComponent, Component>);
		RegisterProperty<float>("SteerForce", &SteerComponent::GetSteerForce, &SteerComponent::SetSteerForce);
		RegisterProperty<float>("MaxSteerVelocity", &SteerComponent::GetMaxSteerVelocity, &SteerComponent::SetMaxSteerVelocity);
		RegisterProperty<float>("MaxSteerAngle", &SteerComponent::GetMaxSteerAngle, &SteerComponent::SetMaxSteerAngle);
		RegisterProperty<float>("SpeedMultiplier", &SteerComponent::GetSpeedMultiplier, &SteerComponent::SetSpeedMultiplier);
	}

	void SteerComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(SteerComponent::OnJointUpdate,ODEPhysicsHingeJointEvent,0));
		
		//get input from parent?
		SceneObjectPtr parent = GASS_DYNAMIC_PTR_CAST<SceneObject>(GetSceneObject()->GetParent());
		parent->RegisterForMessage(REG_TMESS(SteerComponent::OnInput,InputRelayEvent,0));
	}

	void SteerComponent::OnInput(InputRelayEventPtr message)
	{

		std::string name = message->GetController();
		float value = message->GetValue();

		if (name == "Steer")
		{
			float max_rad_angle = Math::Deg2Rad(m_MaxSteerAngle);
			m_DesiredAngle = value*max_rad_angle;
		}
		/*float value = GASS_ANY_CAST<float>(any_mess->GetData("Value"));
		float angular_vel = value*m_MaxSteerVelocity;
		if (name == "Steer")
		{
			//send rotaion message to physics engine
			MessagePtr force_msg(new PhysicsJointMessage(PhysicsJointMessage::AXIS1_FORCE,m_SteerForce));
			MessagePtr vel_msg(new PhysicsJointMessage(PhysicsJointMessage::AXIS1_VELOCITY,angular_vel));

			GetSceneObject()->PostMessage(force_msg);
			GetSceneObject()->PostMessage(vel_msg);
		}*/
	}

	void SteerComponent::OnJointUpdate(ODEPhysicsHingeJointEventPtr message)
	{
		m_CurrentAngle = message->GetAngle();
		float angular_vel = (m_DesiredAngle-m_CurrentAngle)*m_Speed;

		if(angular_vel > m_MaxSteerVelocity) angular_vel = m_MaxSteerVelocity;
		if(angular_vel < -m_MaxSteerVelocity) angular_vel = -m_MaxSteerVelocity;
		//std::cout << " " <<angular_vel << " " <<m_DesiredAngle << " " << m_CurrentAngle << std::endl;
		GetSceneObject()->PostRequest(PhysicsSuspensionJointMaxSteerTorqueRequestPtr(new PhysicsSuspensionJointMaxSteerTorqueRequest(m_SteerForce)));
		GetSceneObject()->PostRequest(PhysicsSuspensionJointSteerVelocityRequestPtr(new PhysicsSuspensionJointSteerVelocityRequest(angular_vel)));

		/*float angular_vel = value*m_MaxSteerVelocity;
		if (name == "Steer")
		{
			//send rotaion message to physics engine
			MessagePtr force_msg(new PhysicsJointMessage(PhysicsJointMessage::AXIS1_FORCE,m_SteerForce));
			MessagePtr vel_msg(new PhysicsJointMessage(PhysicsJointMessage::AXIS1_VELOCITY,angular_vel));
			GetSceneObject()->PostMessage(force_msg);
			GetSceneObject()->PostMessage(vel_msg);
		}*/
	}


}
