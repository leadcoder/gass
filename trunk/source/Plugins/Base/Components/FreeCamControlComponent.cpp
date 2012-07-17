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
#include "Plugins/Base/Components/FreeCamControlComponent.h"
#include "Plugins/Base/CoreMessages.h"
#include "Sim/Components/Graphics/GASSILocationComponent.h"
#include "Sim/Components/Graphics/GASSICameraComponent.h"

#include "Sim/GASSSimEngine.h"
#include "Sim/Systems/Input/GASSControlSettingsManager.h"
#include "Sim/Systems/Input/GASSControlSetting.h"
#include "Sim/GASSCommon.h"
#include "Sim/Scene/GASSScene.h"
#include "Sim/Scene/GASSSceneObject.h"

#include "Sim/Scene/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Scene/GASSGraphicsSceneMessages.h"

#include "Sim/GASSSimEngine.h"
#include "Sim/Scheduling/GASSIRuntimeController.h"

#include "Core/Utils/GASSLogManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/Math/GASSQuaternion.h"

#include "Sim/Scene/GASSCoreSceneManager.h"

namespace GASS
{
	FreeCamControlComponent::FreeCamControlComponent() : m_FovChangeSpeed(10),
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
		m_Mode("Aircraft"),
		m_Debug(false),
		m_AltControlSetting(NULL)
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
		RegisterProperty<bool>("Debug", &GASS::FreeCamControlComponent::GetDebug, &GASS::FreeCamControlComponent::SetDebug);

	}

	void FreeCamControlComponent::OnInitialize()
	{
		
		GetSceneObject()->RegisterForMessage(REG_TMESS(FreeCamControlComponent::PositionChange, PositionMessage ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(FreeCamControlComponent::RotationChange,RotationMessage ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(FreeCamControlComponent::OnLoad,LoadComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(FreeCamControlComponent::OnUnload,UnloadComponentsMessage,0));

		m_ControlSetting = SimEngine::Get().GetControlSettingsManager()->GetControlSetting("FreeCameraInputSettings");
		assert(m_ControlSetting);
		m_ControlSetting->GetMessageManager()->RegisterForMessage(typeid(ControllerMessage), MESSAGE_FUNC( FreeCamControlComponent::OnInput));

		m_AltControlSetting = SimEngine::Get().GetControlSettingsManager()->GetControlSetting("FreeCameraAltInputSettings");
		if(m_AltControlSetting)
			m_AltControlSetting->GetMessageManager()->RegisterForMessage(typeid(ControllerMessage), MESSAGE_FUNC( FreeCamControlComponent::OnInput));


		ScenePtr scene = GetSceneObject()->GetScene();

		scene->RegisterForMessage(REG_TMESS( FreeCamControlComponent::OnChangeCamera, ChangeCameraMessage, 0 ));
	}

	void FreeCamControlComponent::OnLoad(LoadComponentsMessagePtr message)
	{
		//register for updates
		SceneManagerListenerPtr listener = shared_from_this();
		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<CoreSceneManager>()->Register(listener);
	}

	void FreeCamControlComponent::OnUnload(MessagePtr message)
	{
		m_ControlSetting->GetMessageManager()->UnregisterForMessage(typeid(ControllerMessage), MESSAGE_FUNC( FreeCamControlComponent::OnInput));

		if(m_AltControlSetting)
		{
			m_AltControlSetting->GetMessageManager()->UnregisterForMessage(typeid(ControllerMessage), MESSAGE_FUNC( FreeCamControlComponent::OnInput));
		}

		ScenePtr scene = GetSceneObject()->GetScene();
		scene->UnregisterForMessage(UNREG_TMESS( FreeCamControlComponent::OnChangeCamera, ChangeCameraMessage));
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
			//FileLog::Print("Scroll wheel input %f",value );
		}


	}


	void FreeCamControlComponent::SceneManagerTick(double delta_time)
	{
		if(!m_ControlSetting) return;
		if(m_Active)
		{
			StepPhysics(delta_time);
		}
	}

	void FreeCamControlComponent::StepPhysics(double delta_time)
	{
		Float turn_speed_x = 0;
		Float turn_speed_y = 0;

		if(m_EnableRotInput)
		{
			turn_speed_x = -m_PitchInput*Math::Deg2Rad(m_TurnSpeed);
			turn_speed_y = -m_HeadingInput*Math::Deg2Rad(m_TurnSpeed);
		}

		static Float speed_factor = 0;

		if(fabs(m_ThrottleInput) > 0 || fabs(m_StrafeInput) > 0 || fabs(m_UpDownInput) > 0)
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

		Float forward_speed = m_ThrottleInput*delta_time*speed_factor;
		//LogManager::Get().stream() << "speed:" << forward_speed << " T:" << m_ThrottleInput << " d:"<< delta_time <<   " f:" << speed_factor << "\n";
		Float strafe_speed = m_StrafeInput*delta_time*speed_factor;
		Float updown_speed = m_UpDownInput*delta_time*speed_factor;

		Float teta = m_Rot.x; //heading
		Float beta = m_Rot.y; //pitch

		Vec3 forward_vel;
		Vec3 strafe_vel;
		Vec3 tot_vel;
		//Vec3 gravity;

		ScenePtr scene = GetSceneObject()->GetScene();

		Vec3 up(0,1,0);
		Vec3 north (0,0,-1);
		Vec3 east (1,0,0);


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

		//m_Rot = m_Rot +  up*turn_speed_x;
		//m_Rot = m_Rot +  east*turn_speed_y;

		m_Rot.x +=  turn_speed_x;
		m_Rot.y +=  turn_speed_y;
		//m_Rot.h +=  turn_speed_x;
		//m_Rot.p +=  turn_speed_y;

		//gravity = gravity * delta;
		//tot_vel.y += up_down_speed;

		m_Pos = m_Pos + tot_vel;
		//std::cout << "Rot:" << m_Rot.x << " " << m_Rot.y << " " << m_Rot.z << std::endl;
		//std::cout << "Pos:" << m_Pos.x << " " << m_Pos.y << " " << m_Pos.z << std::endl;
		int from_id = (int)this;
		MessagePtr pos_msg(new PositionMessage(m_Pos,from_id));
		GetSceneObject()->PostMessage(pos_msg);

		Quaternion rot_to_send(m_Rot);
		if(up.z == 1)
		{
			//fix rotation if we want z as up axis in gass scene
			rot_to_send = Quaternion(Vec3(0,0,m_Rot.x));
			rot_to_send = Quaternion(Vec3(0,m_Rot.y,0))*rot_to_send;
			//we have to rotate 90 deg if z is up
			//rot_to_send = Quaternion(Vec3(0,Math::Deg2Rad(90),0))*rot_to_send;
		}
		MessagePtr rot_msg(new RotationMessage(rot_to_send,from_id));
		GetSceneObject()->PostMessage(rot_msg);
	
		m_HeadingInput = 0;
		m_PitchInput = 0;
		//m_UpDownInput = m_UpDownInput*0.9;
	
		if(m_Debug)
		{
			std::cout << "FreeCameraComponent Position:" << m_Pos << " Rotation:" << m_Rot << std::endl;
		}
	}
}
