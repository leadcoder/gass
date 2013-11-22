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
		m_LastPos(0,0,0),
		m_DesiredSpeed(0),
		m_Enable(false),
		m_WPReached(false),
		m_VehicleSpeed(0,0,0)

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
		RegisterProperty<float>("DesiredSpeed", &CarAutopilotComponent::GetDesiredSpeed, &CarAutopilotComponent::SetDesiredSpeed,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Desired speed",PF_VISIBLE  | PF_EDITABLE)));
		RegisterProperty<bool>("Enable", &CarAutopilotComponent::GetEnable, &CarAutopilotComponent::SetEnable,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Enable/Disable this component",PF_VISIBLE  | PF_EDITABLE)));
		RegisterProperty<float>("DesiredPosRadius", &CarAutopilotComponent::GetDesiredPosRadius, &CarAutopilotComponent::SetDesiredPosRadius,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Enable/Disable this component",PF_VISIBLE  | PF_EDITABLE)));

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
		m_LastPos = m_CurrentPos;
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
			DriveTo(m_DesiredPos,m_LastPos, m_DesiredSpeed, delta);
	}

	void CarAutopilotComponent::DriveTo(const Vec3 &pos,const Vec3 &last_pos, float desired_speed, float time)
	{
		Vec3 v_pos = m_CurrentPos;
		const float current_speed = -m_VehicleSpeed.z;

		/*v_pos.y = 0;
		//pos.y = 0;
		//go to current way point
		Vec3 follow_line = pos - last_pos;
		float speed = follow_line.Length();

		
		Vec3 dir_to_wp = pos - v_pos;
		float dist_to_wp = dir_to_wp.Length();*/

		Vec3 target_pos = pos;

		
		

		/*if(speed > 0.01)
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
		else*/
		
		

		//float dist_to_line = (closest_point_on_line - m_UGV->GetAbsPos()).Length();
		//Vec3 drive_dir = pos - m_UGV->GetAbsPos();
		Vec3 drive_dir = target_pos - v_pos;
		float drive_dist = drive_dir.Length();

		if(m_DesiredPosRadius > 0  && drive_dist < m_DesiredPosRadius)
			m_WPReached = true;

		//Font::DebugPrint("drive_dist %f ",drive_dist);
		if(!m_WPReached && drive_dist > 0)// && dist_to_wp > m_DesiredPosRadius)
		{
			drive_dir.y = 0;
			drive_dir.Normalize();
			Mat4 trans = m_Transformation;
			Vec3 hull_dir = -trans.GetViewDirVector();
			//Vec3 hull_dir = trans.GetViewDirVector();
			hull_dir.y = 0;
			hull_dir.Normalize();
			
			Vec3 cross = Math::Cross(hull_dir,drive_dir);
			float cos_angle = Math::Dot(hull_dir,drive_dir);

			if(cos_angle > 1) 
				cos_angle = 1;
			if(cos_angle < -1) 
				cos_angle = -1;
			float angle_to_drive_dir = Math::Rad2Deg(acos(cos_angle));
			if(cross.y < 0) 
				angle_to_drive_dir *= -1;

			
			m_TurnPID.set(0);
			float turn = m_TurnPID.update(angle_to_drive_dir,time);
			
			//float m_TurnRadius = 3.0;
			//float m_BrakeDist= 10.0;

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

			// damp speed if we have to turn sharp
			desired_speed = desired_speed * 0.2 + 0.8 * (desired_speed * fabs(cos_angle)); 
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

			if(current_speed < 0.01 &&  throttle < 0)
				throttle  = 0;
				
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
