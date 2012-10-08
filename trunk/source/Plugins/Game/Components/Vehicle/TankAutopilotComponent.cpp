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

#include "TankAutopilotComponent.h"
#include "GameMessages.h"
#include "Plugins/Game/GameSceneManager.h"

#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/Scene/GASSScene.h"
#include "Sim/Scene/GASSSceneObject.h"
#include "Sim/Systems/Resource/GASSIResourceSystem.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "Sim/Scheduling/GASSIRuntimeController.h"
#include "Sim/Systems/Input/GASSIControlSettingsSystem.h"
#include "Sim/Systems/Input/GASSIControlSettingsSystem.h"


namespace GASS
{
	TankAutopilotComponent::TankAutopilotComponent()  : m_ThrottleInput("Throttle"),
		m_SteerInput("Steer"),
		m_DesiredPosRadius ( 4),
		m_DesiredPos(0,0,0),
		m_CurrentPos(0,0,0),
		m_LastPos(0,0,0),
		m_DesiredSpeed(0),
		m_Enable(false)

	{
		m_TurnPID.setGain(2.0,0.02,0.01);
		m_TrottlePID.setGain(1.0,0,0);
	}

	TankAutopilotComponent::~TankAutopilotComponent()
	{

	}

	void TankAutopilotComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("TankAutopilotComponent",new Creator<TankAutopilotComponent, IComponent>);
		RegisterProperty<std::string>("SteerInput", &TankAutopilotComponent::GetSteerInput, &TankAutopilotComponent::SetSteerInput);
		RegisterProperty<std::string>("ThrottleInput", &TankAutopilotComponent::GetThrottleInput, &TankAutopilotComponent::SetThrottleInput);
		RegisterProperty<float>("DesiredSpeed", &TankAutopilotComponent::GetDesiredSpeed, &TankAutopilotComponent::SetDesiredSpeed);
		RegisterProperty<bool>("Enable", &TankAutopilotComponent::GetEnable, &TankAutopilotComponent::SetEnable);
		RegisterProperty<float>("DesiredPosRadius", &TankAutopilotComponent::GetDesiredPosRadius, &TankAutopilotComponent::SetDesiredPosRadius);
		RegisterProperty<PIDControl>("TurnPID", &TankAutopilotComponent::GetTurnPID, &TankAutopilotComponent::SetTurnPID);
		RegisterProperty<PIDControl>("TrottlePID", &TankAutopilotComponent::GetTrottlePID, &TankAutopilotComponent::SetTrottlePID);
		
		
		
	}

	void TankAutopilotComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(TankAutopilotComponent::OnInput,InputControllerMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(TankAutopilotComponent::OnGotoPosition,GotoPositionMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(TankAutopilotComponent::OnSetDesiredSpeed,DesiredSpeedMessage,0));
		
		GetSceneObject()->RegisterForMessage(REG_TMESS(TankAutopilotComponent::OnLoad,LoadComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(TankAutopilotComponent::OnUnload,UnloadComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(TankAutopilotComponent::OnPhysicsMessage,VelocityNotifyMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(TankAutopilotComponent::OnTransMessage,TransformationNotifyMessage,0));
	}

	void TankAutopilotComponent::OnLoad(LoadComponentsMessagePtr message)
	{
		SceneManagerListenerPtr listener = shared_from_this();
		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<GameSceneManager>()->Register(listener);
		
	}

	void TankAutopilotComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
	}

	void TankAutopilotComponent::OnGotoPosition(GotoPositionMessagePtr message)
	{
		Vec3 pos = message->GetPosition();
		m_DesiredPos.Set(pos.x,0,pos.z);
	}

	void TankAutopilotComponent::OnSetDesiredSpeed(DesiredSpeedMessagePtr message)
	{
		m_DesiredSpeed = message->GetSpeed();
		
	}

	void TankAutopilotComponent::OnTransMessage(TransformationNotifyMessagePtr message)
	{
		m_LastPos = m_CurrentPos;
		m_CurrentPos = message->GetPosition();
		Quaternion  rot = message->GetRotation();
		m_Transformation.Identity();
		rot.ToRotationMatrix(m_Transformation);
		m_Transformation.SetTranslation(m_CurrentPos.x,m_CurrentPos.y,m_CurrentPos.z);
	}

	void TankAutopilotComponent::OnPhysicsMessage(VelocityNotifyMessagePtr message)
	{
		Vec3 ang_vel  = message->GetAngularVelocity();
		m_AngularVelocity = ang_vel;
		m_VehicleSpeed  = message->GetLinearVelocity();
	}

	void TankAutopilotComponent::OnInput(InputControllerMessagePtr message)
	{

		std::string name = message->GetController();
		float value = message->GetValue();
	}

	void TankAutopilotComponent::SceneManagerTick(double delta)
	{
		if(m_Enable)
			DriveTo(m_DesiredPos,m_LastPos, m_DesiredSpeed, delta);
	}

	void TankAutopilotComponent::DriveTo(const Vec3 &pos,const Vec3 &last_pos, float desired_speed, float time)
	{
		Vec3 v_pos = m_CurrentPos;
		v_pos.y = 0;
		//pos.y = 0;
		//go to current way point
		Vec3 follow_line = pos - last_pos;
		float speed = follow_line.Length();

		Vec3 dir_to_wp = pos - v_pos;
		float dist_to_wp = dir_to_wp.Length();

		Vec3 target_pos;

		
		const float current_speed = -m_VehicleSpeed.z;

		if(speed > 0.01)
		{
			follow_line.Normalize();
			Vec3 closest_point_on_line = Math::ClosestPointOnLine(pos,last_pos, v_pos);

			if(Math::Dot(follow_line,dir_to_wp) > 0) // check that we are not "behide" the waypoint
			{
				//Try to goto a postion 20m ahead
				float look_ahead = fabs(current_speed);
				if(look_ahead > 20.0f)
					look_ahead = 20.0f;

				if(look_ahead < 4)
					look_ahead = 4;
				

				target_pos = closest_point_on_line + follow_line*look_ahead;
			}
			else
				target_pos = pos;
		}
		else
			target_pos = pos;

		//float dist_to_line = (closest_point_on_line - m_UGV->GetAbsPos()).Length();
		//Vec3 drive_dir = pos - m_UGV->GetAbsPos();
		Vec3 drive_dir = target_pos - v_pos;

		float drive_dist = drive_dir.Length();

		//Font::DebugPrint("drive_dist %f ",drive_dist);
		if(drive_dist > 0.1 && dist_to_wp > m_DesiredPosRadius)
		{
			drive_dir.y = 0;
			drive_dir.Normalize();
			Mat4 trans = m_Transformation;
			Vec3 hull_dir = -trans.GetViewDirVector();
			hull_dir.y = 0;
			hull_dir.Normalize();
			
			Vec3 cross = Math::Cross(hull_dir,drive_dir);
			float cos_angle = Math::Dot(hull_dir,drive_dir);

			if(cos_angle > 1) cos_angle = 1;
			if(cos_angle < -1) cos_angle = -1;
			float angle_to_drive_dir = Math::Rad2Deg(acos(cos_angle));
			if(cross.y < 0) angle_to_drive_dir *= -1;

			
			m_TurnPID.set(0);
			float turn = m_TurnPID.update(angle_to_drive_dir,time);
			
			float m_TurnRadius = 3.0;
			float m_BrakeDist= 10.0;

			//std::cout << "Drive dir angle:" << angle_to_drive_dir << "turn:" << turn << std::endl;


		
			//if(m_ActionHandler->GetOwner()->GetFirstPhysicsBody())
			//current_speed = m_ActionHandler->GetOwner()->GetFirstPhysicsBody()->GetVelocity().Length();
		/*/	if(drive_dist < m_BrakeDist)
			{
				desired_speed = desired_speed*(1-(m_BrakeDist  - drive_dist)/m_BrakeDist);
			}*/



			/*if(fabs(sin(Math::Deg2Rad(angle_to_drive_dir))* m_TurnRadius*2) > dist_to_wp)// || fabs(angle_to_drive_dir) > 80) //back up
			{
				desired_speed *= -1;
				turn *=-1;
			}*/

			
			m_TrottlePID.set(desired_speed);
			float throttle = m_TrottlePID.update(current_speed,time);

			if(throttle > 1) throttle = 1;
			if(throttle < -1) throttle = -1;


		/*	if(fabs(sin(Math::Deg2Rad(angle_to_drive_dir))* m_TurnRadius*2) > dist_to_wp || fabs(angle_to_drive_dir) > 80) //back up
			{
				throttle = -1;
				turn *=-1;
			}*/

			//turn = -turn;
			if(turn > 1) turn  = 1;
			if(turn < -1) turn  = -1;



			//Send input message

			MessagePtr throttle_message(new InputControllerMessage("",m_ThrottleInput,throttle,CT_AXIS));
			GetSceneObject()->SendImmediate(throttle_message);

			MessagePtr steering_message(new InputControllerMessage("",m_SteerInput,turn,CT_AXIS));
			GetSceneObject()->SendImmediate(steering_message);

		}
	}

}
