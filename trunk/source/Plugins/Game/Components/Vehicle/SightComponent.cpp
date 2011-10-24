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
	SightComponent::SightComponent() : m_YawController("Yaw"),m_PitchController("Pitch"),
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
		m_CurrentPitchAngle(0)
	{
		
		m_BaseTransformation.Identity();
		m_StartTransformation.Identity();
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
		RegisterProperty<float>("MaxYawVelocity", &SightComponent::GetMaxYawVelocity, &SightComponent::SetMaxYawVelocity);
		RegisterProperty<float>("MaxPitchVelocity", &SightComponent::GetMaxPitchVelocity, &SightComponent::SetMaxPitchVelocity);
		RegisterProperty<int>("AutoAimPriority", &SightComponent::GetAutoAimPriority, &SightComponent::SetAutoAimPriority);
		RegisterProperty<Vec2>("YawMaxMinAngle", &SightComponent::GetYawMaxMinAngle, &SightComponent::SetYawMaxMinAngle);
		RegisterProperty<Vec2>("PitchMaxMinAngle", &SightComponent::GetPitchMaxMinAngle, &SightComponent::SetPitchMaxMinAngle);
		RegisterProperty<SceneObjectLink >("AutoAimObject", &SightComponent::GetAutoAimObject, &SightComponent::SetAutoAimObject);
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
		Mat4 rot_mat;
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
		rot_mat.Identity();
		rot_mat.Rotate(m_YawValue,m_PitchValue,0);

		//add relative tranformation, we start rotation from this transformation 
		m_StartTransformation.SetTranslation(0,0,0);
		m_AimTransformation = rot_mat*m_StartTransformation;
	}


	void SightComponent::Update(double delta_time)
	{
		if(!m_Active)
		{
			m_YawValue = 0;
			m_PitchValue = 0;
			m_StartTransformation = m_BaseTransformation;
			return;
		}
		UpdateAimTransformation(delta_time);
		
		Quaternion rot;
		rot.FromRotationMatrix(m_AimTransformation.GetRotation());
		GetSceneObject()->PostMessage(MessagePtr(new WorldRotationMessage(rot)));


		//Send message to auto aim system to control weapon system

		//Create point in terrain to aim at, here we can take lead and distance into consideration
		//but for now we just extrude the aim direction 100 m
		const Vec3 aim_point =  m_BaseTransformation.GetTranslation() - m_AimTransformation.GetViewDirVector()*1000;
		m_AutoAimObject->PostMessage(MessagePtr(new AimAtPositionMessage(aim_point,m_AutoAimPriority)));

		
		Vec3 desired_aim_direction = -m_AimTransformation.GetViewDirVector();
		Vec3 start = m_BaseTransformation.GetTranslation();
		Vec3 end = start + desired_aim_direction*4;
		DEBUG_DRAW_LINE(start,end,Vec4(0,1,1,1));
		end = start + m_AimTransformation.GetRightVector()*4;
		DEBUG_DRAW_LINE(start,end,Vec4(1,0,1,1));
		end = start + m_AimTransformation.GetUpVector()*4;
		DEBUG_DRAW_LINE(start,end,Vec4(0,1,0,1));

		
		//update current angle
		m_CurrentYawAngle = m_AimTransformation.GetEulerHeading();
		m_CurrentPitchAngle = m_AimTransformation.GetEulerPitch();

		std::stringstream ss;
		ss << "Yaw:" << Math::Rad2Deg(m_CurrentYawAngle) << " MaxMin"  << m_PitchMaxAngle << m_PitchMinAngle << "\n"
		<< "Pitch:" << Math::Rad2Deg(m_CurrentPitchAngle) << "\n"; 
		DEBUG_PRINT(ss.str());
	}

	void SightComponent::OnLoad(LoadGameComponentsMessagePtr message)
	{
		SimEngine::GetPtr()->GetRuntimeController()->Register(this);
		GetSceneObject()->RegisterForMessage(REG_TMESS(SightComponent::OnBaseTransformation,TransformationNotifyMessage,0));

		m_AutoAimObject->RegisterForMessage(REG_TMESS(SightComponent::OnBarrelTransformation,BarrelTransformationMessage,0));
	}

	void SightComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
		SimEngine::GetPtr()->GetRuntimeController()->Unregister(this);
	}

	void SightComponent::OnInput(ControllerMessagePtr message)
	{
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
				m_YawInput = 0;
				m_PitchInput = 0;
				//std::cout << "deactivate\n";
			}
			//m_AutoAimObject->PostMessage(MessagePtr(new ActivateAutoAimMessage(m_Active)));
		}
		if (m_Active && name == m_SendDesiredPointController)
		{
			bool auto_aim = false;
			if(value > 0)
				auto_aim = true;
			m_AutoAimObject->PostMessage(MessagePtr(new ActivateAutoAimMessage(auto_aim,m_AutoAimPriority)));
		}

		if (name == m_ResetToBarrelController)
		{
			if(value > 0)
			{
				Quaternion rot;
				rot.FromRotationMatrix(m_BarrelTransformation.GetRotation());
				GetSceneObject()->PostMessage(MessagePtr(new WorldRotationMessage(rot)));

				m_YawValue = 0;
				m_PitchValue = 0;
				m_StartTransformation = m_BarrelTransformation.GetRotation();
			}
		}

		
		if (m_Active && name == m_YawController)
		{
			if(fabs(value) < 0.1) //clamp
				value  = 0;

			m_YawInput = value;
		}
		if (m_Active && name == m_PitchController)
		{
			if(fabs(value) < 0.1) //clamp
				value  = 0;

			m_PitchInput = value;
		}

		
	}
	
	TaskGroup SightComponent::GetTaskGroup() const
	{
		return "VEHICLE_TASK_GROUP";
	}
}
