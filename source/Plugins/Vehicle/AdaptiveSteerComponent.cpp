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

#include "AdaptiveSteerComponent.h"
#include "Sim/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Math/GASSMath.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Interface/GASSIPhysicsSuspensionComponent.h"

namespace GASS
{
	AdaptiveSteerComponent::AdaptiveSteerComponent() : 
		m_MaxSteerAngleAtSpeed(45,1)
		,m_MinSteerAngleAtSpeed(45,20)
		
	{

	}

	AdaptiveSteerComponent::~AdaptiveSteerComponent()
	{

	}

	void AdaptiveSteerComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register<AdaptiveSteerComponent>();
		RegisterGetSet("SteerForce", &AdaptiveSteerComponent::GetSteerForce, &AdaptiveSteerComponent::SetSteerForce);
		RegisterGetSet("MaxSteerAngleAtSpeed", &AdaptiveSteerComponent::GetMaxSteerAngleAtSpeed, &AdaptiveSteerComponent::SetMaxSteerAngleAtSpeed);
		RegisterGetSet("MinSteerAngleAtSpeed", &AdaptiveSteerComponent::GetMinSteerAngleAtSpeed, &AdaptiveSteerComponent::SetMinSteerAngleAtSpeed);
		RegisterGetSet("MaxSteerVelocity", &AdaptiveSteerComponent::GetMaxSteerVelocity, &AdaptiveSteerComponent::SetMaxSteerVelocity);
		RegisterGetSet("SpeedMultiplier", &AdaptiveSteerComponent::GetSpeedMultiplier, &AdaptiveSteerComponent::SetSpeedMultiplier);
		RegisterGetSet("DynamicInputPower", &AdaptiveSteerComponent::GetDynamicInputPower, &AdaptiveSteerComponent::SetDynamicInputPower);
		
	}

	void AdaptiveSteerComponent::OnInitialize()
	{
		
		GetSceneObject()->RegisterForMessage(REG_TMESS(AdaptiveSteerComponent::OnJointUpdate,ODEPhysicsHingeJointEvent,0));
	
		//get input from parent?
		SceneObjectPtr parent = GetSceneObject()->GetParent();
		parent->RegisterForMessage(REG_TMESS(AdaptiveSteerComponent::OnInput,InputRelayEvent,0));
		parent->RegisterForMessage(REG_TMESS(AdaptiveSteerComponent::OnVelocityMessage,PhysicsVelocityEvent,0));
	}

	void AdaptiveSteerComponent::OnVelocityMessage(PhysicsVelocityEventPtr message)
	{
		m_VehicleSpeed = static_cast<float>(message->GetLinearVelocity().Length());
	}

	void AdaptiveSteerComponent::OnInput(InputRelayEventPtr message)
	{
		std::string name = message->GetController();
		float value = message->GetValue();
		float sign = 1;

		if(value < 0)
		{
			sign = -1;
			value = fabs(value);
		}

		if (name == "Steer")
		{
			auto interp = static_cast<float>((m_VehicleSpeed - m_MaxSteerAngleAtSpeed.y)/(m_MinSteerAngleAtSpeed.y - m_MaxSteerAngleAtSpeed.y));
			if(interp < 0 )
				interp = 0.0;
			if(interp > 1)
				interp = 1.0;
			
			
			
			auto steer_angle = static_cast<float>(m_MinSteerAngleAtSpeed.x  + (1.0- interp)*(m_MaxSteerAngleAtSpeed.x - m_MinSteerAngleAtSpeed.x));
			auto rad_angle = static_cast<float>(Math::Deg2Rad(steer_angle));

			if(m_DynamicInputPower > 0)
			{
				m_DesiredAngle = value*(1.0f-interp)* rad_angle;
				m_DesiredAngle += pow(value, static_cast<float>(m_DynamicInputPower))*(interp)* rad_angle;
			}
			else
				m_DesiredAngle = value* rad_angle;
			m_DesiredAngle *= sign;
		}
	}

	void AdaptiveSteerComponent::OnJointUpdate(ODEPhysicsHingeJointEventPtr message)
	{
		m_CurrentAngle = message->GetAngle();
		float angular_vel = (m_DesiredAngle-m_CurrentAngle)*m_Speed;

		if(angular_vel > m_MaxSteerVelocity) angular_vel = m_MaxSteerVelocity;
		if(angular_vel < -m_MaxSteerVelocity) angular_vel = -m_MaxSteerVelocity;
		//std::cout << " " <<angular_vel << " " <<m_DesiredAngle << " " << m_CurrentAngle << std::endl;
		PhysicsSuspensionComponentPtr suspension = GetSceneObject()->GetFirstComponentByClass<IPhysicsSuspensionComponent>();
		suspension->SetMaxSteerTorque(m_SteerForce);
		suspension->SetAngularSteerVelocity(angular_vel);
	}
}
