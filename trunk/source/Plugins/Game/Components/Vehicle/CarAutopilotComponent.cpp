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

#include "CarAutopilotComponent.h"
#include "GameMessages.h"
#include "Plugins/Game/GameSceneManager.h"

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
	CarAutopilotComponent::CarAutopilotComponent()  : m_ThrottleInput("Throttle"),
		m_SteerInput("Steer"),
		m_DesiredPosRadius(0),
		m_DesiredPos(0,0,0),
		m_CurrentPos(0,0,0),
		m_DesiredSpeed(0),
		m_Enable(false),
		m_WPReached(false),
		m_VehicleSpeed(0,0,0),
		m_BrakeDistanceFactor(1.0)
	{
		m_TurnPID.setGain(2.0,0.02,0.01);
		m_TrottlePID.setGain(1.0,0,0);
	}

	CarAutopilotComponent::~CarAutopilotComponent()
	{

	}

	void CarAutopilotComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("CarAutopilotComponent",new Creator<CarAutopilotComponent, IComponent>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("CarAutopilotComponent", OF_VISIBLE)));

		RegisterProperty<std::string>("SteerInput", &CarAutopilotComponent::GetSteerInput, &CarAutopilotComponent::SetSteerInput,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Input mapping for steer",PF_VISIBLE)));
		RegisterProperty<std::string>("ThrottleInput", &CarAutopilotComponent::GetThrottleInput, &CarAutopilotComponent::SetThrottleInput,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Input mapping for throttle",PF_VISIBLE)));
		RegisterProperty<Float>("DesiredSpeed", &CarAutopilotComponent::GetDesiredSpeed, &CarAutopilotComponent::SetDesiredSpeed,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Desired speed",PF_VISIBLE  | PF_EDITABLE)));
		RegisterProperty<bool>("Enable", &CarAutopilotComponent::GetEnable, &CarAutopilotComponent::SetEnable,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Enable/Disable this component",PF_VISIBLE  | PF_EDITABLE)));
		RegisterProperty<Float>("DesiredPosRadius", &CarAutopilotComponent::GetDesiredPosRadius, &CarAutopilotComponent::SetDesiredPosRadius,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Enable/Disable this component",PF_VISIBLE  | PF_EDITABLE)));
		RegisterProperty<Float>("BrakeDistanceFactor", &CarAutopilotComponent::GetBrakeDistanceFactor, &CarAutopilotComponent::SetBrakeDistanceFactor,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Multiplier for default linear brake distance (1m at 1m/s) ",PF_VISIBLE  | PF_EDITABLE)));
		
		RegisterProperty<PIDControl>("TurnPID", &CarAutopilotComponent::GetTurnPID, &CarAutopilotComponent::SetTurnPID,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Steer PID regulator values",PF_VISIBLE  | PF_EDITABLE)));
		RegisterProperty<PIDControl>("TrottlePID", &CarAutopilotComponent::GetTrottlePID, &CarAutopilotComponent::SetTrottlePID,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Throttle PID regulator values",PF_VISIBLE  | PF_EDITABLE)));
	}

	void CarAutopilotComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(CarAutopilotComponent::OnInput,InputControllerMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(CarAutopilotComponent::OnGotoPosition,GotoPositionMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(CarAutopilotComponent::OnSetDesiredSpeed,DesiredSpeedMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(CarAutopilotComponent::OnPhysicsMessage,VelocityNotifyMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(CarAutopilotComponent::OnTransMessage,TransformationNotifyMessage,0));

		SceneManagerListenerPtr listener = shared_from_this();
		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<GameSceneManager>()->Register(listener);
	}

	void CarAutopilotComponent::OnDelete()
	{

	}

	void CarAutopilotComponent::OnGotoPosition(GotoPositionMessagePtr message)
	{
		Vec3 pos = message->GetPosition();
		m_DesiredPos.Set(pos.x,pos.y,pos.z);
		m_WPReached = false;
	}

	void CarAutopilotComponent::OnSetDesiredSpeed(DesiredSpeedMessagePtr message)
	{
		m_DesiredSpeed = message->GetSpeed();
	}

	void CarAutopilotComponent::OnTransMessage(TransformationNotifyMessagePtr message)
	{
		m_CurrentPos = message->GetPosition();
		Quaternion  rot = message->GetRotation();
		m_Transformation.Identity();
		rot.ToRotationMatrix(m_Transformation);
		m_Transformation.SetTranslation(m_CurrentPos.x,m_CurrentPos.y,m_CurrentPos.z);
	}

	void CarAutopilotComponent::OnPhysicsMessage(VelocityNotifyMessagePtr message)
	{
		Vec3 ang_vel  = message->GetAngularVelocity();
		m_AngularVelocity = ang_vel;
		m_VehicleSpeed  = message->GetLinearVelocity();
	}

	void CarAutopilotComponent::OnInput(InputControllerMessagePtr message)
	{

		std::string name = message->GetController();
		float value = message->GetValue();
	}

	void CarAutopilotComponent::SceneManagerTick(double delta)
	{
		if(m_Enable)
		{
			_UpdateDrive(delta);
		}
	}

	void CarAutopilotComponent::_UpdateDrive(double delta_time)
	{
		Vec3 target_pos  = m_DesiredPos;
		Float desired_speed	= m_DesiredSpeed;
		Vec3 current_dir = -m_Transformation.GetViewDirVector();
		float current_speed = -m_VehicleSpeed.z; 
		Vec3 current_pos = m_CurrentPos;
		
		Vec3 drive_dir = target_pos - current_pos;
		float drive_dist = drive_dir.Length();
		if(!m_WPReached && drive_dist > 0)// && dist_to_wp > m_DesiredPosRadius)
		{
			drive_dir.y = 0;
			drive_dir.Normalize();
			
			current_dir.y = 0;
			current_dir.Normalize();
			
			Vec3 cross = Math::Cross(current_dir,drive_dir);
			float cos_angle = Math::Dot(current_dir,drive_dir);

			if(cos_angle > 1) 
				cos_angle = 1;
			if(cos_angle < -1) 
				cos_angle = -1;
			float angle_to_drive_dir = Math::Rad2Deg(acos(cos_angle));
			if(cross.y < 0) 
				angle_to_drive_dir *= -1;
			
			m_TurnPID.set(0);
			float turn = m_TurnPID.update(angle_to_drive_dir, delta_time);
	
			// damp speed if we have to turn sharp
			if(fabs(angle_to_drive_dir) > 90 && drive_dist > 5)// do three point turn if more than 20 meters turn on point
			{
				desired_speed *= -1;
				if(current_speed < 0)  //check that not rolling forward
					turn *=-1;
				//else
				//	turn = 0;

			}
			else if(fabs(angle_to_drive_dir) > 80 && drive_dist < 5)// do three point turn or just reverse
			{
				desired_speed *= -1;
				if(current_speed < 0 && fabs(angle_to_drive_dir) < 120) //if less than 110 deg do three point turn
					turn *=-1;
				else //damp turn, we try to reverse!
					turn *= 0.03;
			}
			else
			{
				//slow down if we are turning sharp and speed is to high
				Float min_steer_speed = 5;
				Float max_steer_speed = 20;

				if(current_speed > min_steer_speed)
				{
					Float w = (current_speed - min_steer_speed)/(max_steer_speed - min_steer_speed);
					desired_speed = desired_speed * (1.0 - w) + w * (desired_speed * fabs(cos_angle)); 
				}
				
				if(current_speed < 0) //you want to go forward but rolling backward, invert steering
					turn *=-1;
			}

			//Linear damp speed if we are inside radius from waypoint, 
			//the radius is dynamic and is based on the desired speed of the vehicle.
			//The vehicle will have longer brake distance at high speed
			//and this is compensated by taking the speed in consideration.
			//The user can tweak this radius with the m_BrakeDistanceFactor based
			//on the properties of the vehicle, better brakes == lower m_BrakeDistanceFactor value
			//By default this value is 1 which means that the vehicle can come to rest
			//after 1m if traveling at 1 m/s. This formula should probably be expanded to support
			//more non linear behavior 
			Float radius = fabs(desired_speed)*m_BrakeDistanceFactor;
			if(drive_dist > 0 && drive_dist < radius)
			{
				 desired_speed = desired_speed * (drive_dist/radius);
			}

			
			//if(drive_dist < fabs(desired_speed))
			//	desired_speed *= 0.5;

	
			if(m_DesiredPosRadius > 0  && drive_dist < m_DesiredPosRadius)
			{
				desired_speed = 0;
			}

			m_TrottlePID.set(desired_speed);
			float throttle = m_TrottlePID.update(current_speed,delta_time);

			if(throttle > 1) throttle = 1;
			if(throttle < -1) throttle = -1;

			//turn = -turn;
			if(turn > 1) turn  = 1;
			if(turn < -1) turn  = -1;

			//Send input message

			MessagePtr throttle_message(new InputControllerMessage("",m_ThrottleInput,throttle,CT_AXIS));
			GetSceneObject()->SendImmediate(throttle_message);

			MessagePtr steering_message(new InputControllerMessage("",m_SteerInput,-turn,CT_AXIS));
			GetSceneObject()->SendImmediate(steering_message);
		}
		else
		{
			//std::cout << "Dist 0\n";
			MessagePtr throttle_message(new InputControllerMessage("",m_ThrottleInput,0,CT_AXIS));
			GetSceneObject()->SendImmediate(throttle_message);

			MessagePtr steering_message(new InputControllerMessage("",m_SteerInput,0,CT_AXIS));
			GetSceneObject()->SendImmediate(steering_message);

		}
	}
}
