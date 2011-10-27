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

#include "SightComponent.h"
#include "GameMessages.h"
#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Utils/Log.h"

#include "Sim/Components/Network/INetworkComponent.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Systems/Resource/IResourceSystem.h"
#include "Sim/Systems/Messages/GraphicsSystemMessages.h"
#include "Sim/SimEngine.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Scheduling/IRuntimeController.h"
#include "Sim/Systems/Input/ControlSettingsManager.h"
#include "Sim/Systems/Input/ControlSetting.h"
#include "Sim/Scenario/Scene/Messages/SoundSceneObjectMessages.h"


namespace GASS
{
	SightComponent::SightComponent() : m_YawController("Yaw"),
		m_PitchController("Pitch"),
		m_ToggleZoomController("ToggleZoom"),
		m_MaxYawVelocity(1),
		m_MaxPitchVelocity(1),
		m_PitchMinAngle(-1000),
		m_PitchMaxAngle(1000),
		m_YawMinAngle(-1000),
		m_YawMaxAngle(1000),
		m_Active(false),
		//m_AngularVelocity(0),
		m_YawValue(0),
		m_PitchValue(0),
		m_YawInput(0),
		m_PitchInput(0),
		m_CurrentYawAngle(0),
		m_CurrentPitchAngle(0),
		m_CurrentZoom(0),
		m_RemoteSim(false)
	{
		
		m_BaseTransformation.Identity();
		m_StartRotation.Identity();
		//m_ZoomValues.push_back(45);
		//m_ZoomValues.push_back(20);
		//m_ZoomValues.push_back(10);
		//m_TurretTransformation.Identity();
		//m_BarrelTransformation.Identity();
	}

	SightComponent::~SightComponent()
	{

	}

	void SightComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("SightComponent",new Creator<SightComponent, IComponent>);
		RegisterProperty<std::string>("ActivateController", &SightComponent::GetActivateController, &SightComponent::SetActivateController);
		RegisterProperty<std::string>("YawController", &SightComponent::GetYawController, &SightComponent::SetYawController);
		RegisterProperty<std::string>("PitchController", &SightComponent::GetPitchController, &SightComponent::SetPitchController);
		RegisterProperty<std::string>("SendDesiredPointController", &SightComponent::GetSendDesiredPointController, &SightComponent::SetSendDesiredPointController);
		RegisterProperty<std::string>("ResetToBarrelController", &SightComponent::GetResetToBarrelController, &SightComponent::SetResetToBarrelController);
		RegisterProperty<std::string>("ToggleZoomController", &SightComponent::GetToggleZoomController, &SightComponent::SetToggleZoomController);
		RegisterProperty<float>("MaxYawVelocity", &SightComponent::GetMaxYawVelocity, &SightComponent::SetMaxYawVelocity);
		RegisterProperty<float>("MaxPitchVelocity", &SightComponent::GetMaxPitchVelocity, &SightComponent::SetMaxPitchVelocity);
		RegisterProperty<int>("AutoAimPriority", &SightComponent::GetAutoAimPriority, &SightComponent::SetAutoAimPriority);
		RegisterProperty<Vec2>("YawMaxMinAngle", &SightComponent::GetYawMaxMinAngle, &SightComponent::SetYawMaxMinAngle);
		RegisterProperty<Vec2>("PitchMaxMinAngle", &SightComponent::GetPitchMaxMinAngle, &SightComponent::SetPitchMaxMinAngle);
		RegisterProperty<SceneObjectLink >("AutoAimObject", &SightComponent::GetAutoAimObject, &SightComponent::SetAutoAimObject);
		RegisterVectorProperty<float>("ZoomValues", &SightComponent::GetZoomValues, &SightComponent::SetZoomValues);
	}

	void SightComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(SightComponent::OnLoad,LoadGameComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(SightComponent::OnUnload,UnloadComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(SightComponent::OnInput,ControllerMessage,0));
		//call this to ensure that scene object pointers get initlized
		BaseSceneComponent::OnCreate();
	}

	void SightComponent::OnBaseTransformation(TransformationNotifyMessagePtr message)
	{
		m_BaseTransformation.SetTransformation(message->GetPosition(),message->GetRotation(),Vec3(1,1,1));
	}

	void SightComponent::OnBarrelTransformation(BarrelTransformationMessagePtr message)
	{
		m_BarrelTransformation = message->GetTransformation();
	}

#define DEBUG_DRAW_LINE(start,end,color) SimEngine::Get().GetSimSystemManager()->PostMessage(MessagePtr(new DrawLineMessage(start,end,color)))
#define DEBUG_PRINT(text) SimEngine::Get().GetSimSystemManager()->PostMessage(MessagePtr(new DebugPrintMessage(text)))

	void SightComponent::UpdateAimTransformation(double delta_time)
	{
		if(!((m_CurrentYawAngle < Math::Deg2Rad(m_YawMinAngle) && m_YawInput < 0) ||
			(m_CurrentYawAngle > Math::Deg2Rad(m_YawMaxAngle) && m_YawInput > 0)))
			//ouside envelope
		{
			m_YawValue = m_YawValue + m_YawInput*m_MaxYawVelocity*delta_time;
			//m_RotValue = m_TurnInput*m_MaxSteerVelocity*delta_time;
		}
		if(!((m_CurrentPitchAngle < Math::Deg2Rad(m_PitchMinAngle) && m_PitchInput < 0) ||
			(m_CurrentPitchAngle > Math::Deg2Rad(m_PitchMaxAngle) && m_PitchInput  > 0)))
			//ouside envelope
		{
			m_PitchValue  = m_PitchValue  + m_PitchInput *m_MaxPitchVelocity*delta_time;
			//m_RotValue = m_TurnInput*m_MaxSteerVelocity*delta_time;
		}
		//create separate yaw and pitch rotation matrices.
		Mat4 yaw_rot_mat;
		yaw_rot_mat.Identity();
		yaw_rot_mat.RotateY(m_YawValue);

		Mat4 pitch_rot_mat;
		pitch_rot_mat.Identity();
		pitch_rot_mat.RotateX(m_PitchValue);

		//Important to get correct rotation order here! First start with yaw angle in world space, 
		//then add start rotation (yaw and then pitch) , last add pitch rotation. 
		//If we begin with start rotation we get tilted yaw-rotation-plane, 
		//ie the camera will roll and change pitch value during yaw movment.
		//If we add start rotation last, the same problem will arise. ie the pitch matrix
		//will tilt the way rotation part of the start rotation matrix.
		m_AimRotation = pitch_rot_mat*m_StartRotation*yaw_rot_mat;
	}


	void SightComponent::Update(double delta_time)
	{
		if(!m_Active)
		{
			m_YawValue = 0;
			m_PitchValue = 0;
			m_StartRotation = m_BaseTransformation;
			m_StartRotation.SetTranslation(0,0,0);
			return;
		}
		//damp input values every frame if we use mouse input
		//m_YawInput = m_YawInput* 0.9;
		//m_PitchInput = m_PitchInput *0.9; 

		UpdateAimTransformation(delta_time);
		

		
		
		Quaternion rot;
		rot.FromRotationMatrix(m_AimRotation.GetRotation());
		GetSceneObject()->PostMessage(MessagePtr(new WorldRotationMessage(rot)));


		//Send message to auto aim system to control weapon system

		//Create point in terrain to aim at, here we can take lead and distance into consideration
		//but for now we just extrude the aim direction 100 m
		const Vec3 aim_point =  m_BaseTransformation.GetTranslation() - m_AimRotation.GetViewDirVector()*1000;

		int id = (int) this; 
		m_AutoAimObject->PostMessage(MessagePtr(new AimAtPositionMessage(aim_point,m_AutoAimPriority,id)));

		
		/*Vec3 desired_aim_direction = -m_AimRotation.GetViewDirVector();
		Vec3 start = m_BaseTransformation.GetTranslation();
		Vec3 end = start + desired_aim_direction*4;
		DEBUG_DRAW_LINE(start,end,Vec4(0,1,1,1));
		end = start + m_AimRotation.GetRightVector()*4;
		DEBUG_DRAW_LINE(start,end,Vec4(1,0,1,1));
		end = start + m_AimRotation.GetUpVector()*4;
		DEBUG_DRAW_LINE(start,end,Vec4(0,1,0,1));*/

		
		//update current angle
		m_CurrentYawAngle = m_AimRotation.GetEulerHeading();
		m_CurrentPitchAngle = m_AimRotation.GetEulerPitch();

		/*std::stringstream ss;
		ss << "Yaw:" << Math::Rad2Deg(m_CurrentYawAngle) << " MaxMin"  << m_PitchMaxAngle << m_PitchMinAngle << "\n"
		<< "Pitch:" << Math::Rad2Deg(m_CurrentPitchAngle) << "\n"; 
		DEBUG_PRINT(ss.str());*/
	}

	void SightComponent::OnLoad(LoadGameComponentsMessagePtr message)
	{
		NetworkComponentPtr net_comp = GetSceneObject()->GetFirstComponentByClass<INetworkComponent>();
		if(net_comp)
			m_RemoteSim = net_comp->IsRemote();


		if(!m_RemoteSim)
		{
			SimEngine::GetPtr()->GetRuntimeController()->Register(this);
			GetSceneObject()->RegisterForMessage(REG_TMESS(SightComponent::OnBaseTransformation,TransformationNotifyMessage,0));

			m_AutoAimObject->RegisterForMessage(REG_TMESS(SightComponent::OnBarrelTransformation,BarrelTransformationMessage,0));
			m_AutoAimObject->RegisterForMessage(REG_TMESS(SightComponent::OnAimAtPosition,AimAtPositionMessage,0));
		}

		//set start zoom
		if(m_ZoomValues.size() > 0)
		{
			MessagePtr cam_msg(new CameraParameterMessage(CameraParameterMessage::CAMERA_FOV,m_ZoomValues[0],0));
			GetSceneObject()->PostMessage(cam_msg);
		}
	}

	void SightComponent::OnAimAtPosition(AimAtPositionMessagePtr message)
	{
		int id = (int) this; 
		//disable this sight if message not sent by us and has higher priority
		if(id != message->GetSenderID() && message->GetPriority() > m_AutoAimPriority)
		{
			m_Active = false;
		}
	}

	void SightComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
		if(!m_RemoteSim)
		{
			SimEngine::GetPtr()->GetRuntimeController()->Unregister(this);
		}
	}

	void SightComponent::OnInput(ControllerMessagePtr message)
	{
		int id = (int) this; 
		std::string name = message->GetController();
		float value = message->GetValue();
		if (name == m_ActivateController)
		{
			if(value > 0)
			{	
				m_Active = true;
				//std::cout << "activate\n";
				
			}

			else
			{
				m_Active = false;
				 
				if (!m_RemoteSim) //also release auto aim?
					m_AutoAimObject->PostMessage(MessagePtr(new ActivateAutoAimMessage(false,m_AutoAimPriority,id)));
				m_YawInput = 0;
				m_PitchInput = 0;
				//std::cout << "deactivate\n";
			}
			//m_AutoAimObject->PostMessage(MessagePtr(new ActivateAutoAimMessage(m_Active)));
		}
		if (!m_RemoteSim && m_Active && name == m_SendDesiredPointController)
		{
			bool auto_aim = false;
			if(value > 0)
				auto_aim = true;

			
			m_AutoAimObject->PostMessage(MessagePtr(new ActivateAutoAimMessage(auto_aim,m_AutoAimPriority,id)));
		}

		if (!m_RemoteSim && name == m_ResetToBarrelController)
		{
			if(value > 0)
			{
				Quaternion rot;
				rot.FromRotationMatrix(m_BarrelTransformation.GetRotation());
				GetSceneObject()->PostMessage(MessagePtr(new WorldRotationMessage(rot)));

				m_YawValue = 0;
				m_PitchValue = 0;
				m_StartRotation = m_BarrelTransformation.GetRotation();
			}
		}

		if (name == m_ToggleZoomController)
		{
			if(value > 0)
			{
				if(m_ZoomValues.size() > 0)
				{
					m_CurrentZoom++;
					m_CurrentZoom = m_CurrentZoom % m_ZoomValues.size();
					MessagePtr cam_msg(new CameraParameterMessage(CameraParameterMessage::CAMERA_FOV,m_ZoomValues[m_CurrentZoom],0));
					GetSceneObject()->PostMessage(cam_msg);
				}
			}
		}

		if (!m_RemoteSim && m_Active && name == m_YawController)
		{
			m_YawInput = value;
		}

		if (!m_RemoteSim && m_Active && name == m_PitchController)
		{
			m_PitchInput = value;
		}
	}
	
	TaskGroup SightComponent::GetTaskGroup() const
	{
		return "VEHICLE_TASK_GROUP";
	}
}
