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
		m_MaxSteerVelocity(1),
		m_SteerForce(10),
		m_MaxSteerAngle(0),
		m_BarrelMinAngle(-1000),
		m_BarrelMaxAngle(1000),
		m_TurretMinAngle(-1000),
		m_TurretMaxAngle(1000),
		m_TurretAngle(0),
		m_BarrelAngle(0),
		m_Active(false),
		m_AngularVelocity(0),
		m_YawValue(0),
		m_PitchValue(0),
		m_YawInput(0),
		m_PitchInput(0),
		m_MaxYawTorque(10),
		m_MaxPitchTorque(10)
	{
		m_BaseTransformation.Identity();
		m_TurretTransformation.Identity();
		m_BarrelTransformation.Identity();
	}

	SightComponent::~SightComponent()
	{

	}

	void SightComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("SightComponent",new Creator<SightComponent, IComponent>);
		RegisterProperty<std::string>("YawController", &SightComponent::GetYawController, &SightComponent::SetYawController);
		RegisterProperty<std::string>("PitchController", &SightComponent::GetPitchController, &SightComponent::SetPitchController);
		RegisterProperty<float>("MaxSteerVelocity", &SightComponent::GetMaxSteerVelocity, &SightComponent::SetMaxSteerVelocity);
		RegisterProperty<float>("MaxSteerAngle", &SightComponent::GetMaxSteerAngle, &SightComponent::SetMaxSteerAngle);
		RegisterProperty<float>("SteerForce", &SightComponent::GetSteerForce, &SightComponent::SetSteerForce);
		RegisterProperty<Vec2>("TurretMaxMinAngle", &SightComponent::GetTurretMaxMinAngle, &SightComponent::SetTurretMaxMinAngle);
		RegisterProperty<Vec2>("BarrelMaxMinAngle", &SightComponent::GetBarrelMaxMinAngle, &SightComponent::SetBarrelMaxMinAngle);

		RegisterProperty<std::string>("BarrelHinge", &SightComponent::GetBarrelHinge, &SightComponent::SetBarrelHinge);
		RegisterProperty<std::string>("TurretHinge", &SightComponent::GetTurretHinge, &SightComponent::SetTurretHinge);
		RegisterProperty<PIDControl>("YawPID", &SightComponent::GetYawPID, &SightComponent::SetYawPID);
		RegisterProperty<PIDControl>("PitchPID", &SightComponent::GetPitchPID, &SightComponent::SetPitchPID);

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

	void SightComponent::SetBarrelHinge(const std::string &name)
	{
		m_BarrelHingeName = name;
	}

	std::string SightComponent::GetBarrelHinge() const
	{
		return m_BarrelHingeName;
	}

	void SightComponent::SetTurretHinge(const std::string &name)
	{
		m_TurretHingeName = name;
	}

	std::string SightComponent::GetTurretHinge() const
	{
		return m_TurretHingeName;
	}

	void SightComponent::OnTurretTransformation(TransformationNotifyMessagePtr message)
	{
		m_TurretTransformation.SetTransformation(message->GetPosition(),message->GetRotation(),Vec3(1,1,1));
	}

	void SightComponent::OnBarrelTransformation(TransformationNotifyMessagePtr message)
	{
		m_BarrelTransformation.SetTransformation(message->GetPosition(),message->GetRotation(),Vec3(1,1,1));
	}

	void SightComponent::OnBaseTransformation(TransformationNotifyMessagePtr message)
	{
		m_BaseTransformation.SetTransformation(message->GetPosition(),message->GetRotation(),Vec3(1,1,1));
	}

#define DEBUG_DRAW_LINE(start,end,color) SimEngine::Get().GetSimSystemManager()->PostMessage(MessagePtr(new DrawLineMessage(start,end,color)))
#define DEBUG_PRINT(text) SimEngine::Get().GetSimSystemManager()->PostMessage(MessagePtr(new DebugPrintMessage(text)))

	Vec3 SightComponent::ProjectVectorOnPlane(const Vec3 plane_normal,const Vec3 &v)
	{
		return  v - Math::Dot(v, plane_normal) * plane_normal;
	}

	void SightComponent::UpdateAimTransformation(double delta_time)
	{
		Mat4 rot_mat;
		if(!((m_TurretAngle < Math::Deg2Rad(m_TurretMinAngle) && m_YawInput < 0) ||
			(m_TurretAngle > Math::Deg2Rad(m_TurretMaxAngle) && m_YawInput > 0)))
			//ouside envelope
		{
			m_YawValue = m_YawValue + m_YawInput*m_MaxSteerVelocity*delta_time;
			//m_RotValue = m_TurnInput*m_MaxSteerVelocity*delta_time;
		}
		if(!((m_BarrelAngle < Math::Deg2Rad(m_BarrelMinAngle) && m_PitchInput < 0) ||
			(m_BarrelAngle > Math::Deg2Rad(m_BarrelMaxAngle) && m_PitchInput  > 0)))
			//ouside envelope
		{
			m_PitchValue  = m_PitchValue  + m_PitchInput *m_MaxSteerVelocity*delta_time;
			//m_RotValue = m_TurnInput*m_MaxSteerVelocity*delta_time;
		}
		rot_mat.Identity();
		rot_mat.Rotate(m_YawValue,m_PitchValue,0);

		//add relative tranformation, we start rotation from this transformation 
		m_StartTransformation.SetTranslation(0,0,0);
		m_AimTransformation = rot_mat*m_StartTransformation;
	}


	Float SightComponent::GetAngleOnPlane(const Vec3 &plane_normal,const Vec3 &v1,const Vec3 &v2)
	{
		Vec3 cross = Math::Cross(v1,v2);
		float cos_angle = Math::Dot(v1,v2);
		if(cos_angle > 1) cos_angle = 1;
		if(cos_angle < -1) cos_angle = -1;
		float angle = Math::Rad2Deg(acos(cos_angle));
		if(Math::Dot(plane_normal,cross) > 0) 
			angle *= -1;
		return angle;
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
		//m_AutoAimObject->PostMessage(MessagePtr(new WorldRotationMessage(rot)));
		
		
		Vec3 desired_aim_direction = -m_AimTransformation.GetViewDirVector();
		Vec3 start = m_BaseTransformation.GetTranslation();
		Vec3 end = start + desired_aim_direction*4;
		DEBUG_DRAW_LINE(start,end,Vec4(0,1,1,1));
		end = start + m_AimTransformation.GetRightVector()*4;
		DEBUG_DRAW_LINE(start,end,Vec4(1,0,1,1));
		end = start + m_AimTransformation.GetUpVector()*4;
		DEBUG_DRAW_LINE(start,end,Vec4(0,1,0,1));

		
		m_BarrelAngle = m_AimTransformation.GetEulerPitch();
		m_TurretAngle = m_AimTransformation.GetEulerHeading();
		std::stringstream ss;
		ss << "BarrelAngle:" << Math::Rad2Deg(m_BarrelAngle) << " MaxMin"  << m_BarrelMaxAngle << m_BarrelMinAngle << "\n"
		<< "TurretAngle:" << Math::Rad2Deg(m_TurretAngle) << "\n"; 
		
		DEBUG_PRINT(ss.str());
		
	}

	void SightComponent::OnLoad(LoadGameComponentsMessagePtr message)
	{
		SimEngine::GetPtr()->GetRuntimeController()->Register(this);
		GetSceneObject()->RegisterForMessage(REG_TMESS(SightComponent::OnBaseTransformation,TransformationNotifyMessage,0));
	}

	void SightComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
		SimEngine::GetPtr()->GetRuntimeController()->Unregister(this);
	}

	void SightComponent::OnInput(ControllerMessagePtr message)
	{
		std::string name = message->GetController();
		float value = message->GetValue();
		if (name == "ActivateTurret")
		{
			if(value > 0)
			{	
				m_Active = true;
				std::cout << "activate\n";
			}

			else
			{
				m_Active = false;
				m_YawInput = 0;
				m_PitchInput = 0;
				std::cout << "deactivate\n";
			}
		}
		else if (m_Active && name == m_YawController)
		{
			if(fabs(value) < 0.1) //clamp
				value  = 0;

			m_YawInput = value;
		}
		else if (m_Active && name == m_PitchController)
		{
			if(fabs(value) < 0.1) //clamp
				value  = 0;

			m_PitchInput = value;
		}
	}

	void SightComponent::OnTurretHingeUpdate(HingeJointNotifyMessagePtr message)
	{
		m_TurretAngle = message->GetAngle();
		//Send turret information message
	}

	void SightComponent::OnBarrelHingeUpdate(HingeJointNotifyMessagePtr message)
	{
		m_BarrelAngle = message->GetAngle();
		//Send turret information message
	}

	
	TaskGroup SightComponent::GetTaskGroup() const
	{
		return "VEHICLE_TASK_GROUP";
	}

	SceneObjectPtr  SightComponent::GetTurretHingeObject() const
	{
		return SceneObjectPtr(m_TurretHingeObject,boost::detail::sp_nothrow_tag());
	}

	SceneObjectPtr  SightComponent::GetBarrelHingeObject() const
	{
		return SceneObjectPtr(m_BarrelHingeObject,boost::detail::sp_nothrow_tag());
	}

}
