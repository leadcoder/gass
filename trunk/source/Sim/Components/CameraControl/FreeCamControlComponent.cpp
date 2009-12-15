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

#include <boost/bind.hpp>
#include "Sim/Components/CameraControl/FreeCamControlComponent.h"
#include "Sim/Components/Graphics/ILocationComponent.h"
#include "Sim/Components/Graphics/ICameraComponent.h"

#include "Sim/SimEngine.h"
#include "Sim/Systems/Input/ControlSettingsManager.h"
#include "Sim/Systems/Input/ControlSetting.h"
#include "Sim/Common.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"

#include "Sim/SimEngine.h"
#include "Sim/Scheduling/IRuntimeController.h"

#include "Core/Utils/Log.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/Math/Quaternion.h"

namespace GASS
{
	FreeCamControlComponent::FreeCamControlComponent() :
m_FovChangeSpeed(10),
m_MaxFov(120),
m_MinFov(10),
m_RunSpeed(1000),
m_WalkSpeed(20),
m_TurnSpeed(10),
m_ControlSetting (NULL),
m_Pos(0,0,0),
m_Rot(0,0,0),
m_EnableRotInput(false),
m_SpeedBoostInput(false),
m_ThrottleInput(0),
m_StrafeInput(0),
m_PitchInput(0),
m_HeadingInput(0),
m_Active(false),
m_CurrentFov(45),
m_UpDownInput(0),
m_Mode("Aircraft")
{

}

FreeCamControlComponent::~FreeCamControlComponent()
{

}

void FreeCamControlComponent::RegisterReflection()                         // static
{
	ComponentFactory::GetPtr()->Register("FreeCamControlComponent",new Creator<FreeCamControlComponent, IComponent>);
	RegisterProperty<Float>("RunSpeed", &GASS::FreeCamControlComponent::GetRunSpeed, &GASS::FreeCamControlComponent::SetRunSpeed);
	RegisterProperty<Float>("WalkSpeed", &GASS::FreeCamControlComponent::GetWalkSpeed, &GASS::FreeCamControlComponent::SetWalkSpeed);
	RegisterProperty<Float>("TurnSpeed", &GASS::FreeCamControlComponent::GetTurnSpeed, &GASS::FreeCamControlComponent::SetTurnSpeed);
	RegisterProperty<std::string>("Mode", &GASS::FreeCamControlComponent::GetMode, &GASS::FreeCamControlComponent::SetMode);

}

void FreeCamControlComponent::OnCreate()
{
	SimEngine::GetPtr()->GetRuntimeController()->Register(this);
	
	REGISTER_OBJECT_MESSAGE_TYPE(FreeCamControlComponent::PositionChange, OBJECT_RM_POSITION,0);
	REGISTER_OBJECT_MESSAGE_TYPE(FreeCamControlComponent::RotationChange,OBJECT_RM_ROTATION,0);
	REGISTER_OBJECT_MESSAGE_TYPE(FreeCamControlComponent::OnInit,OBJECT_RM_LOAD_SIM_COMPONENTS,0);
	REGISTER_OBJECT_MESSAGE_TYPE(FreeCamControlComponent::OnUnload,OBJECT_RM_UNLOAD_COMPONENTS,0);

	m_ControlSetting = SimEngine::Get().GetControlSettingsManager()->GetControlSetting("FreeCameraInputSettings");

	m_ControlSetting->GetMessageManager()->RegisterForMessage(CONTROLLER_MESSAGE_NEW_INPUT, MESSAGE_FUNC( FreeCamControlComponent::OnInput));

	ScenarioScenePtr scene = GetSceneObject()->GetSceneObjectManager()->GetScenarioScene();
	scene->RegisterForMessage(SCENARIO_RM_CHANGE_CAMERA, MESSAGE_FUNC( FreeCamControlComponent::OnChangeCamera));
}

TaskGroup FreeCamControlComponent::GetTaskGroup() const
{
	return MAIN_TASK_GROUP;
}

void FreeCamControlComponent::OnUnload(MessagePtr message)
{
	SimEngine::GetPtr()->GetRuntimeController()->Unregister(this);
}

void FreeCamControlComponent::OnChangeCamera(MessagePtr message)
{
	ChangeCameraMessagePtr cc_mess = boost::shared_dynamic_cast<ChangeCameraMessage>(message);
	if(cc_mess)
	{
		SceneObjectPtr cam_obj = cc_mess->GetCamera();

		if(GetSceneObject() == cam_obj)
		{
			m_Active = true;
		}
		else
			m_Active = false;
	}
}

void FreeCamControlComponent::PositionChange(MessagePtr message)
{
	if(message->GetSenderID() != (int) this)
	{
		PositionMessagePtr pos_mess = boost::shared_static_cast<PositionMessage>(message);
		m_Pos = pos_mess->GetPosition();
	}
}

void FreeCamControlComponent::RotationChange(MessagePtr message)
{
	if(message->GetSenderID() != (int) this)
	{
		RotationMessagePtr pos_mess = boost::shared_static_cast<RotationMessage>(message);

		Mat4 rot_mat;
		pos_mess->GetRotation().ToRotationMatrix(rot_mat);
		m_Rot.x = rot_mat.GetEulerHeading();
		m_Rot.y = rot_mat.GetEulerPitch();
		m_Rot.z = rot_mat.GetEulerRoll();
	}
}

void FreeCamControlComponent::OnInput(MessagePtr message)
{
	if(!m_Active) 
		return;
	ControllerMessagePtr control_mess = boost::shared_static_cast<ControllerMessage>(message);
	std::string name = control_mess->GetController();
	float value = control_mess->GetValue();

	if(name == "FreeCameraSpeedBoost")
	{
		if(value > 0)
			m_SpeedBoostInput = true;
		else 
			m_SpeedBoostInput = false;
	}
	else if(name == "FreeCameraEnableRot")
	{
		if(value > 0)
			m_EnableRotInput = true;
		else 
			m_EnableRotInput = false;
	}
	else if(name == "FreeCameraThrottle")
	{
		m_ThrottleInput = value;
		//std::cout << "Input: " << name << "Value:" << value << std::endl;
	}
	else if(name == "FreeCameraStrafe")
	{
		m_StrafeInput = value;
	}
	else if(name == "FreeCameraPitch")
	{
		m_PitchInput = value;

	}
	else if(name == "FreeCameraHeading")
	{
		m_HeadingInput = value;
	}
	else if(name == "FreeCameraUpDown")
	{
		m_UpDownInput = value;
		//Log::Print("Scroll wheel input %f",value );
	}


}

void FreeCamControlComponent::OnInit(MessagePtr message)
{
}


void FreeCamControlComponent::Update(double delta_time)
{
	if(!m_ControlSetting) return;
	if(m_Active)
	{
		StepPhysics(delta_time);
	}
}

void FreeCamControlComponent::StepPhysics(double delta)
{
	//Get default input, this must be moved to a input update function if we run in multi thread mode
	//std::cout << "Throttle:" << throttle << std::endl;
	//std::cout << "Strafe:" << yaw << std::endl;

	Float turn_speed_x = 0;
	Float turn_speed_y = 0;


	if(m_EnableRotInput)
	{
		turn_speed_x = -m_PitchInput*Math::Deg2Rad(m_TurnSpeed);
		turn_speed_y = -m_HeadingInput*Math::Deg2Rad(m_TurnSpeed);
	}

	static Float speed_factor = 0;

	if(fabs(m_ThrottleInput) > 0.1 || fabs(m_StrafeInput) > 0.1 || fabs(m_UpDownInput) > 0.1)
	{
		speed_factor += (fabs(m_ThrottleInput) + fabs(m_StrafeInput) + fabs(m_UpDownInput));
		speed_factor *= 1.03;
	}
	else
	{
		speed_factor *= 0.9;
	}

	if(m_SpeedBoostInput) 
	{
		if(speed_factor > m_RunSpeed)
		{
			speed_factor = m_RunSpeed;
		}
	}
	else 
	{
		if(speed_factor > m_WalkSpeed)
			speed_factor = m_WalkSpeed;
	}

	Float forward_speed = m_ThrottleInput*delta*speed_factor;
	Float strafe_speed = m_StrafeInput*delta*speed_factor;
	Float updown_speed = m_UpDownInput*delta*speed_factor*0.4;

	Float teta = m_Rot.h;
	Float beta = m_Rot.p;

	Vec3 forward_vel;
	Vec3 strafe_vel;
	Vec3 tot_vel;
	//Vec3 gravity;

	ScenarioScenePtr scene = GetSceneObject()->GetSceneObjectManager()->GetScenarioScene();

	Vec3 up = scene->GetSceneUp();
	Vec3 north = scene->GetSceneNorth();
	Vec3 east = scene->GetSceneEast();


	if(m_Mode == "Aircraft")
	{
		//gravity.Set(0,0,0);
		Vec3 cam_east = east * (-cos(beta) * sin(teta));

		Vec3 cam_up = up*sin(beta);
		Vec3 cam_north = north* cos(beta)*cos(teta);

		forward_vel = cam_east + cam_up; 
		forward_vel = forward_vel + cam_north; 

		Vec3 temp = north*forward_vel;
		Float east_strafe_vel = temp.x + temp.y +temp.z;
		temp = east*forward_vel;
		Float north_strafe_vel = temp.x + temp.y +temp.z;


		strafe_vel = east*-east_strafe_vel;
		strafe_vel = strafe_vel + (north*north_strafe_vel);

		strafe_vel = -strafe_vel;
		forward_vel.Normalize();

	}
	else if(m_Mode == "RTS")
	{
		Vec3 cam_east = east * -sin(teta);
		Vec3 cam_north = north* cos(teta);
		forward_vel = cam_east + cam_north; 

		Vec3 temp = north*forward_vel;
		
		Float east_strafe_vel = temp.x + temp.y +temp.z;
		temp = east*forward_vel;
		Float north_strafe_vel = temp.x + temp.y +temp.z;

		strafe_vel = east*-east_strafe_vel;
		strafe_vel = strafe_vel + (north*north_strafe_vel);

		strafe_vel = -strafe_vel;
		forward_vel.Normalize();
	}
	/*else
	{
	gravity.Set(0,-9.82,0);
	forward_vel.x = cos(beta) * sin(teta);
	forward_vel.y = 0;
	forward_vel.z = cos(beta) * cos(teta);
	//forward_vel.z = cos(beta) * -cos(teta);

	strafe_vel.x = -forward_vel.z;
	strafe_vel.y = 0;
	strafe_vel.z = forward_vel.x;
	forward_vel.Normalize();
	}*/

	//std::cout << "vel:" << forward_vel << std::endl;

	forward_vel = forward_vel * (forward_speed);
	strafe_vel = strafe_vel * (strafe_speed);

	tot_vel = forward_vel + strafe_vel;
	tot_vel  = tot_vel  + (up*updown_speed);

	m_Rot.h +=  turn_speed_x;
	m_Rot.p +=  turn_speed_y;

	//gravity = gravity * delta;
	//tot_vel.y += up_down_speed;

	m_Pos = m_Pos + tot_vel;
	//std::cout << "Rot:" << m_Rot.x << " " << m_Rot.y << " " << m_Rot.z << std::endl;
	//std::cout << "Pos:" << m_Pos.x << " " << m_Pos.y << " " << m_Pos.z << std::endl;
	int from_id = (int)this;
	MessagePtr pos_msg(new PositionMessage(m_Pos,from_id));
	GetSceneObject()->PostMessage(pos_msg);

	MessagePtr rot_msg(new RotationMessage(m_Rot,from_id));
	GetSceneObject()->PostMessage(rot_msg);

	m_HeadingInput = 0;
	m_PitchInput = 0;
	m_UpDownInput = 0;
}



}
