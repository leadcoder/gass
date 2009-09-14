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
	TankAutopilotComponent::TankAutopilotComponent()  : m_ThrottleInput("Throttle"), 
		m_SteerInput("Steer"),
		m_DesiredPosRadius ( 4),
		m_DesiredPos(0,0,0),
		m_CurrentPos(0,0,0),
		m_LastPos(0,0,0),
		m_DesiredSpeed(10)

	{
		
	}

	TankAutopilotComponent::~TankAutopilotComponent()
	{

	}

	void TankAutopilotComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("TankAutopilotComponent",new Creator<TankAutopilotComponent, IComponent>);
		RegisterProperty<std::string>("SteerInput", &GetSteerInput, &SetSteerInput);
		RegisterProperty<std::string>("ThrottleInput", &GetThrottleInput, &SetThrottleInput);
	}

	void TankAutopilotComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage((SceneObject::ObjectMessage) OBJECT_NM_PLAYER_INPUT, MESSAGE_FUNC(TankAutopilotComponent::OnInput));
		GetSceneObject()->RegisterForMessage((SceneObject::ObjectMessage) OBJECT_RM_GOTO_POSITION, MESSAGE_FUNC(TankAutopilotComponent::OnGotoPosition));
		GetSceneObject()->RegisterForMessage(SceneObject::OBJECT_RM_LOAD_SIM_COMPONENTS, MESSAGE_FUNC(TankAutopilotComponent::OnLoad));
		GetSceneObject()->RegisterForMessage(SceneObject::OBJECT_RM_UNLOAD_COMPONENTS, MESSAGE_FUNC(TankAutopilotComponent::OnUnload));
		GetSceneObject()->RegisterForMessage(SceneObject::OBJECT_NM_PHYSICS_VELOCITY, MESSAGE_FUNC(TankAutopilotComponent::OnPhysicsMessage));
		GetSceneObject()->RegisterForMessage(SceneObject::OBJECT_NM_TRANSFORMATION_CHANGED, MESSAGE_FUNC(TankAutopilotComponent::OnTransMessage));
	}

	void TankAutopilotComponent::OnLoad(MessagePtr message)
	{
		SimEngine::GetPtr()->GetRuntimeController()->Register(this);	
	}

	void TankAutopilotComponent::OnUnload(MessagePtr message)
	{
		SimEngine::GetPtr()->GetRuntimeController()->Unregister(this);		
	}

	void TankAutopilotComponent::OnGotoPosition(MessagePtr message)
	{
		Vec3 pos = boost::any_cast<Vec3>(message->GetData("Position"));
		m_DesiredPos.Set(pos.x,0,pos.z);
	}

	void TankAutopilotComponent::OnTransMessage(MessagePtr message)
	{
		m_LastPos = m_CurrentPos;

		m_CurrentPos = boost::any_cast<Vec3>(message->GetData("Position"));
		Quaternion  rot = boost::any_cast<Quaternion>(message->GetData("Rotation"));
		m_Transformation.Identity();
		rot.ToRotationMatrix(m_Transformation);
		m_Transformation.SetTranslation(m_CurrentPos.x,m_CurrentPos.y,m_CurrentPos.z);
	}

	void TankAutopilotComponent::OnPhysicsMessage(MessagePtr message)
	{
		Vec3 ang_vel  = boost::any_cast<Vec3>(message->GetData("AngularVelocity"));
		m_AngularVelocity = ang_vel;
		m_VehicleSpeed  = boost::any_cast<Vec3>(message->GetData("Velocity"));
	}

	void TankAutopilotComponent::OnInput(MessagePtr message)
	{
		std::string name = boost::any_cast<std::string>(message->GetData("Controller"));
		float value = boost::any_cast<float>(message->GetData("Value"));
	}

	void TankAutopilotComponent::Update(double delta)
	{
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
		
		if(speed > 0.01)
		{
			follow_line.Normalize();
			Vec3 closest_point_on_line = Math::ClosestPointOnLine(pos,last_pos, v_pos);

			if(Math::Dot(follow_line,dir_to_wp) > 0) // check that we are not "behide" the waypoint
			{	
				//Try to goto a postion 3m ahead
				target_pos = closest_point_on_line + follow_line*20;
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
			if(cross.y > 0) angle_to_drive_dir *= -1;

			m_TurnPID.setGain(2.0,0.02,0.01);
			m_TurnPID.set(0);
			float turn = -m_TurnPID.update(angle_to_drive_dir/180.0f,time);
			float throttle = 1;
			float m_TurnRadius = 3.0;
			float m_BrakeDist= 10.0;

			//std::cout << "Drive dir angle:" << angle_to_drive_dir << "turn:" << turn << std::endl;
			

			float current_speed = 0;
			current_speed = m_VehicleSpeed.Length();
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
			
			m_TrottlePID.setGain(1.0,0,0);
			m_TrottlePID.set(desired_speed);
			throttle = m_TrottlePID.update(current_speed,time);

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

			MessagePtr throttle_message(new Message(OBJECT_NM_PLAYER_INPUT));
			throttle_message->SetData("Controller",m_ThrottleInput);
			throttle_message->SetData("Value",throttle);
			GetSceneObject()->SendImmediate(throttle_message);	

			MessagePtr steering_message(new Message(OBJECT_NM_PLAYER_INPUT));
			steering_message->SetData("Controller",m_SteerInput);
			steering_message->SetData("Value",turn);
			GetSceneObject()->SendImmediate(steering_message);	
		}
	}

	TaskGroup TankAutopilotComponent::GetTaskGroup() const
	{
		return "VEHICLE_TASK_GROUP";
	}
}
