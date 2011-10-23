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

#include "AutoAimComponent.h"
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
	AutoAimComponent::AutoAimComponent() : m_YawController("Yaw"),m_PitchController("Pitch"),
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

	AutoAimComponent::~AutoAimComponent()
	{

	}

	void AutoAimComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("AutoAimComponent",new Creator<AutoAimComponent, IComponent>);
		RegisterProperty<std::string>("YawController", &AutoAimComponent::GetYawController, &AutoAimComponent::SetYawController);
		RegisterProperty<std::string>("PitchController", &AutoAimComponent::GetPitchController, &AutoAimComponent::SetPitchController);
		RegisterProperty<float>("MaxSteerVelocity", &AutoAimComponent::GetMaxSteerVelocity, &AutoAimComponent::SetMaxSteerVelocity);
		RegisterProperty<float>("MaxSteerAngle", &AutoAimComponent::GetMaxSteerAngle, &AutoAimComponent::SetMaxSteerAngle);
		RegisterProperty<float>("SteerForce", &AutoAimComponent::GetSteerForce, &AutoAimComponent::SetSteerForce);
		RegisterProperty<Vec2>("TurretMaxMinAngle", &AutoAimComponent::GetTurretMaxMinAngle, &AutoAimComponent::SetTurretMaxMinAngle);
		RegisterProperty<Vec2>("BarrelMaxMinAngle", &AutoAimComponent::GetBarrelMaxMinAngle, &AutoAimComponent::SetBarrelMaxMinAngle);

		RegisterProperty<std::string>("BarrelHinge", &AutoAimComponent::GetBarrelHinge, &AutoAimComponent::SetBarrelHinge);
		RegisterProperty<std::string>("TurretHinge", &AutoAimComponent::GetTurretHinge, &AutoAimComponent::SetTurretHinge);
		RegisterProperty<PIDControl>("YawPID", &AutoAimComponent::GetYawPID, &AutoAimComponent::SetYawPID);
		RegisterProperty<PIDControl>("PitchPID", &AutoAimComponent::GetPitchPID, &AutoAimComponent::SetPitchPID);
		
	}

	void AutoAimComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(AutoAimComponent::OnLoad,LoadGameComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(AutoAimComponent::OnUnload,UnloadComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(AutoAimComponent::OnInput,ControllerMessage,0));
		//GetSceneObject()->RegisterForMessage(REG_TMESS(AutoAimComponent::OnJointUpdate,HingeJointNotifyMessage,0));
		//GetSceneObject()->RegisterForMessage(REG_TMESS(AutoAimComponent::OnTransformation,TransformationNotifyMessage,0));
		//GetSceneObject()->RegisterForMessage(REG_TMESS(AutoAimComponent::OnPhysicsMessage, VelocityNotifyMessage,0));

	}

	void AutoAimComponent::SetBarrelHinge(const std::string &name)
	{
		m_BarrelHingeName = name;
	}

	std::string AutoAimComponent::GetBarrelHinge() const
	{
		return m_BarrelHingeName;
	}

	void AutoAimComponent::SetTurretHinge(const std::string &name)
	{
		m_TurretHingeName = name;
	}

	std::string AutoAimComponent::GetTurretHinge() const
	{
		return m_TurretHingeName;
	}

	void AutoAimComponent::OnTurretTransformation(TransformationNotifyMessagePtr message)
	{
		m_TurretTransformation.SetTransformation(message->GetPosition(),message->GetRotation(),Vec3(1,1,1));
	}

	void AutoAimComponent::OnBarrelTransformation(TransformationNotifyMessagePtr message)
	{
		m_BarrelTransformation.SetTransformation(message->GetPosition(),message->GetRotation(),Vec3(1,1,1));
	}

	void AutoAimComponent::OnBaseTransformation(TransformationNotifyMessagePtr message)
	{
		m_BaseTransformation.SetTransformation(message->GetPosition(),message->GetRotation(),Vec3(1,1,1));
	}

	/*void AutoAimComponent::OnPhysicsMessage(VelocityNotifyMessagePtr message)
	{
		m_AngularVelocity = message->GetAngularVelocity().y;
		//	std::cout << "anglvel:" << ang_vel.x << " " << ang_vel.y << " " << ang_vel.z << std::endl;
	}*/

#define DEBUG_DRAW_LINE(start,end,color) SimEngine::Get().GetSimSystemManager()->PostMessage(MessagePtr(new DrawLineMessage(start,end,color)))
#define DEBUG_PRINT(text) SimEngine::Get().GetSimSystemManager()->PostMessage(MessagePtr(new DebugPrintMessage(text)))

	Vec3 AutoAimComponent::ProjectVectorOnPlane(const Vec3 plane_normal,const Vec3 &v)
	{
		return  v - Math::Dot(v, plane_normal) * plane_normal;
	}

	Vec3 AutoAimComponent::GetDesiredAimDirection(double delta_time)
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
		//else
		//	m_RotValue = 0;
		rot_mat.Identity();
		rot_mat.Rotate(m_YawValue,m_PitchValue,0);

		//add relative tranformation, we start rotation from this transformation 
		m_StartTransformation.SetTranslation(0,0,0);
		//Mat4 parent_rot = m_RelTrans;
		//parent_rot.SetTranslation(0,0,0);
		m_AimTransformation = rot_mat*m_StartTransformation;
		return -m_AimTransformation.GetViewDirVector();
	}


	Float AutoAimComponent::GetAngleOnPlane(const Vec3 &plane_normal,const Vec3 &v1,const Vec3 &v2)
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

	void AutoAimComponent::Update(double delta_time)
	{
		//Vec3 turret_dir = -m_TurretTransformation.GetViewDirVector();
		if(!m_Active)
		{
			m_YawValue = 0;
			m_PitchValue = 0;
			m_StartTransformation = m_BarrelTransformation;

			MessagePtr vel_msg(new PhysicsJointMessage(PhysicsJointMessage::AXIS1_VELOCITY,0));
			MessagePtr volume_msg(new SoundParameterMessage(SoundParameterMessage::VOLUME,0));
			GetSceneObject()->PostMessage(vel_msg);
			GetSceneObject()->PostMessage(volume_msg);

			MessagePtr force_msg(new PhysicsJointMessage(PhysicsJointMessage::AXIS1_FORCE,m_SteerForce));
			GetTurretHingeObject()->PostMessage(force_msg);
			GetBarrelHingeObject()->PostMessage(force_msg);

			return;
		}

		Vec3 desired_aim_direction = GetDesiredAimDirection(delta_time);


		//Float angle_to_aim_dir = 0;

		//Vec3 projected_aim = ProjectVectorOnPlane(plane_normal,desired_aim_direction);
		//projected_aim.Normalize();
		//pitch calcaultaion
		const Vec3 barrel_plane_normal = m_TurretTransformation.GetRightVector();
		const Vec3 barrel_dir = -m_BarrelTransformation.GetViewDirVector();
		Vec3 projected_barrel_aim = ProjectVectorOnPlane(barrel_plane_normal,desired_aim_direction);
		
		Float pitch_angle_to_aim_dir = GetAngleOnPlane(barrel_plane_normal,barrel_dir, projected_barrel_aim);

		Vec3 start = m_BarrelTransformation.GetTranslation();
		Vec3 end = start + barrel_dir*4;
		DEBUG_DRAW_LINE(start,end,Vec4(1,0,0,1));
		end = start + projected_barrel_aim*4;
		DEBUG_DRAW_LINE(start,end,Vec4(0,1,1,1));

		//Yaw calculation

		const Vec3 base_plane_normal = m_BaseTransformation.GetUpVector();
		Vec3 projected_aim = ProjectVectorOnPlane(base_plane_normal,desired_aim_direction);
		projected_aim.Normalize();
		const Vec3 turret_dir = -m_TurretTransformation.GetViewDirVector();
		Float yaw_angle_to_aim_dir = GetAngleOnPlane(m_TurretTransformation.GetUpVector(),turret_dir, projected_aim);

		start = m_TurretTransformation.GetTranslation();
		end = start + turret_dir*4;
		DEBUG_DRAW_LINE(start,end,Vec4(0,1,0,1));
		end = start + projected_aim*4;
		DEBUG_DRAW_LINE(start,end,Vec4(0,0,1,1));

		m_YawPID.setOutputLimit(m_MaxYawTorque);
		//m_YawPID.setGain(0.03,0.0001,0.009);
		m_YawPID.set(0);
		float yaw_torque = m_YawPID.update(yaw_angle_to_aim_dir,delta_time);
		//std::cout << "yaw_angle_to_aim_dir:" << yaw_angle_to_aim_dir << "\n";
		m_PitchPID.setOutputLimit(m_MaxPitchTorque);
		//m_PitchPID.setGain(0.0032,0.0001,0.001);
		m_PitchPID.set(0);
		float pitch_torque = m_PitchPID.update(pitch_angle_to_aim_dir,delta_time);
		
		std::stringstream ss;
		ss << "pitch_angle_to_aim_dir:" << pitch_angle_to_aim_dir << "\n"
		<< "pitch_torque:" << pitch_torque << "\n"
		<< "yaw_angle_to_aim_dir:" << yaw_angle_to_aim_dir << "\n"
		<< "yaw_torque:" << pitch_torque << "\n"
		<< "BarrelAngle:" << Math::Rad2Deg(m_BarrelAngle) << " MaxMin"  << m_BarrelMaxAngle << m_BarrelMinAngle << "\n"
		<< "TurretAngle:" << Math::Rad2Deg(m_TurretAngle) << "\n"; 
		
		DEBUG_PRINT(ss.str());
		MessagePtr force_msg(new PhysicsJointMessage(PhysicsJointMessage::AXIS1_FORCE,0));//m_SteerForce));
		//MessagePtr vel_msg(new PhysicsJointMessage(PhysicsJointMessage::AXIS1_VELOCITY,turn_velocity));
		GetTurretHingeObject()->PostMessage(force_msg);
		GetBarrelHingeObject()->PostMessage(force_msg);

		//GetSceneObject()->PostMessage(vel_msg);

		MessagePtr barrel_torque_msg(new PhysicsBodyMessage(PhysicsBodyMessage::TORQUE,Vec3(pitch_torque,0,0)));
		GetBarrelHingeObject()->PostMessage(barrel_torque_msg);

		MessagePtr yaw_torque_msg(new PhysicsBodyMessage(PhysicsBodyMessage::TORQUE,Vec3(0,yaw_torque,0)));
		GetTurretHingeObject()->PostMessage(yaw_torque_msg);
	}

	void AutoAimComponent::OnLoad(LoadGameComponentsMessagePtr message)
	{
		MessagePtr play_msg(new SoundParameterMessage(SoundParameterMessage::PLAY,0));
		GetSceneObject()->PostMessage(play_msg);

		MessagePtr volume_msg(new SoundParameterMessage(SoundParameterMessage::VOLUME,0));
		GetSceneObject()->PostMessage(volume_msg);

		SimEngine::GetPtr()->GetRuntimeController()->Register(this);

		//GetSceneObject()->GetParentSceneObject()->RegisterForMessage(REG_TMESS(AutoAimComponent::OnParentTransformation,TransformationNotifyMessage,0));
		IComponentContainer::ComponentVector comps;
		GetSceneObject()->GetComponentsByClass(comps,"ODEHingeComponent");

		for(int i = 0; i < comps.size() ; i++)
		{
			BaseSceneComponentPtr bsc = boost::shared_dynamic_cast<BaseSceneComponent>(comps[i]);
			if(comps[i]->GetName() == m_TurretHingeName)
			{
				m_TurretHingeObject = bsc->GetSceneObject();
			}
			if(comps[i]->GetName() == m_BarrelHingeName)
			{
				m_BarrelHingeObject = bsc->GetSceneObject();
			}
		}

		if(GetTurretHingeObject() && GetTurretHingeObject()->GetParentSceneObject())
			GetTurretHingeObject()->GetParentSceneObject()->RegisterForMessage(REG_TMESS(AutoAimComponent::OnBaseTransformation,TransformationNotifyMessage,0));


		MessagePtr force_msg(new PhysicsJointMessage(PhysicsJointMessage::AXIS1_FORCE,m_SteerForce));
		MessagePtr vel_msg(new PhysicsJointMessage(PhysicsJointMessage::AXIS1_VELOCITY,0));
		
		if(GetTurretHingeObject())
		{
			GetTurretHingeObject()->RegisterForMessage(REG_TMESS(AutoAimComponent::OnTurretTransformation,TransformationNotifyMessage,0));
			GetTurretHingeObject()->RegisterForMessage(REG_TMESS(AutoAimComponent::OnTurretHingeUpdate,HingeJointNotifyMessage,0));
	
			GetTurretHingeObject()->PostMessage(force_msg);
			GetTurretHingeObject()->PostMessage(vel_msg);

		}
		if(GetBarrelHingeObject())
		{
			GetBarrelHingeObject()->RegisterForMessage(REG_TMESS(AutoAimComponent::OnBarrelTransformation,TransformationNotifyMessage,0));
			GetBarrelHingeObject()->RegisterForMessage(REG_TMESS(AutoAimComponent::OnBarrelHingeUpdate,HingeJointNotifyMessage,0));
	
			GetBarrelHingeObject()->PostMessage(force_msg);
			GetBarrelHingeObject()->PostMessage(vel_msg);
		}
	}

	void AutoAimComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
		SimEngine::GetPtr()->GetRuntimeController()->Unregister(this);
	}

	void AutoAimComponent::OnInput(ControllerMessagePtr message)
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

	void AutoAimComponent::OnTurretHingeUpdate(HingeJointNotifyMessagePtr message)
	{
		m_TurretAngle = -message->GetAngle();
		//Send turret information message
	}

	void AutoAimComponent::OnBarrelHingeUpdate(HingeJointNotifyMessagePtr message)
	{
		m_BarrelAngle = -message->GetAngle();
		//Send turret information message
	}

	
	TaskGroup AutoAimComponent::GetTaskGroup() const
	{
		return "VEHICLE_TASK_GROUP";
	}

	SceneObjectPtr  AutoAimComponent::GetTurretHingeObject() const
	{
		return SceneObjectPtr(m_TurretHingeObject,boost::detail::sp_nothrow_tag());
	}

	SceneObjectPtr  AutoAimComponent::GetBarrelHingeObject() const
	{
		return SceneObjectPtr(m_BarrelHingeObject,boost::detail::sp_nothrow_tag());
	}

}
