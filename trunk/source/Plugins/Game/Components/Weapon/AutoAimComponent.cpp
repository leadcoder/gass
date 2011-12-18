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
	AutoAimComponent::AutoAimComponent() : m_SteerForce(10),
		//m_MaxSteerAngle(0),
		m_BarrelMinAngle(-1000),
		m_BarrelMaxAngle(1000),
		m_TurretMinAngle(-1000),
		m_TurretMaxAngle(1000),
		m_TurretAngle(0),
		m_BarrelAngle(0),
		m_Active(false),
		//m_AngularVelocity(0),
		m_MaxYawTorque(10),
		m_MaxPitchTorque(10),
		m_AimPoint(0,0,0),
		m_GotNewAimPoint(false),
		m_CurrentAimPriority(0)
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
		//RegisterProperty<float>("MaxSteerVelocity", &AutoAimComponent::GetMaxSteerVelocity, &AutoAimComponent::SetMaxSteerVelocity);
//		RegisterProperty<float>("MaxSteerAngle", &AutoAimComponent::GetMaxSteerAngle, &AutoAimComponent::SetMaxSteerAngle);
		RegisterProperty<float>("SteerForce", &AutoAimComponent::GetSteerForce, &AutoAimComponent::SetSteerForce);
		RegisterProperty<Vec2>("TurretMaxMinAngle", &AutoAimComponent::GetTurretMaxMinAngle, &AutoAimComponent::SetTurretMaxMinAngle);
		RegisterProperty<Vec2>("BarrelMaxMinAngle", &AutoAimComponent::GetBarrelMaxMinAngle, &AutoAimComponent::SetBarrelMaxMinAngle);

		RegisterProperty<SceneObjectLink>("BarrelObject", &AutoAimComponent::GetBarrelObject, &AutoAimComponent::SetBarrelObject);
		RegisterProperty<SceneObjectLink>("TurretObject", &AutoAimComponent::GetTurretObject, &AutoAimComponent::SetTurretObject);
		RegisterProperty<PIDControl>("YawPID", &AutoAimComponent::GetYawPID, &AutoAimComponent::SetYawPID);
		RegisterProperty<PIDControl>("PitchPID", &AutoAimComponent::GetPitchPID, &AutoAimComponent::SetPitchPID);
		
	}

	void AutoAimComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(AutoAimComponent::OnLoad,LoadGameComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(AutoAimComponent::OnUnload,UnloadComponentsMessage,0));
		//GetSceneObject()->RegisterForMessage(REG_TMESS(AutoAimComponent::OnInput,ControllerMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(AutoAimComponent::OnAimAtPosition,AimAtPositionMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(AutoAimComponent::OnActivateAutoAim,ActivateAutoAimMessage,0));
		BaseSceneComponent::OnCreate();
	}

	void AutoAimComponent::SetBarrelObject(const SceneObjectLink &value)
	{
		m_BarrelObject = value;
	}

	SceneObjectLink AutoAimComponent::GetBarrelObject() const
	{
		return m_BarrelObject;
	}

	void AutoAimComponent::SetTurretObject(const SceneObjectLink &value)
	{
		m_TurretObject = value;
	}

	SceneObjectLink AutoAimComponent::GetTurretObject() const
	{
		return m_TurretObject;
	}

	void AutoAimComponent::OnAimAtPosition(AimAtPositionMessagePtr message)
	{
		if(m_CurrentAimPriority <= message->GetPriority())
		{
			m_CurrentAimPriority = message->GetPriority();
			m_AimPoint = message->GetPosition();
			m_GotNewAimPoint = true;
		}
	}

	void AutoAimComponent::OnActivateAutoAim(ActivateAutoAimMessagePtr message)
	{
		if(m_CurrentAimPriority <= message->GetPriority())
		{
			m_CurrentAimPriority = message->GetPriority();
			m_Active = message->GetActive();
		}
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
		//send information about current barrel information, 
		//can be used by sight component to redirect sight to barrel
		GetSceneObject()->PostMessage(MessagePtr(new BarrelTransformationMessage(m_BarrelTransformation)));

		if(!m_Active)
		{
			//reset
			m_CurrentAimPriority = 0; 
			MessagePtr vel_msg(new PhysicsJointMessage(PhysicsJointMessage::AXIS1_VELOCITY,0));
			MessagePtr volume_msg(new SoundParameterMessage(SoundParameterMessage::VOLUME,0));
			GetSceneObject()->PostMessage(vel_msg);
			//GetSceneObject()->PostMessage(volume_msg);

			

			MessagePtr force_msg(new PhysicsJointMessage(PhysicsJointMessage::AXIS1_FORCE,m_SteerForce));
			m_TurretObject->PostMessage(force_msg);
			m_BarrelObject->PostMessage(force_msg);
			return;
		}
		m_GotNewAimPoint = false;
		Vec3 desired_aim_direction = m_AimPoint - m_TurretTransformation.GetTranslation();

		/*Vec3 start = m_BarrelTransformation.GetTranslation();
		Vec3 end = start + desired_aim_direction*4;
		DEBUG_DRAW_LINE(start,end,Vec4(1,0,0,1));*/

		//pitch calculation

		Vec3 barrel_plane_normal = m_TurretTransformation.GetRightVector();
		barrel_plane_normal.Normalize();
		Vec3 barrel_dir = -m_BarrelTransformation.GetViewDirVector();
		barrel_dir.Normalize();
		
		
		//rotate in world space to turret dir, this should be inproved!
		Vec3 projected_barrel_aim = barrel_dir;
		projected_barrel_aim.y = 0;
		projected_barrel_aim.Normalize();
		
		Vec3 vert_proj_aim_direction = desired_aim_direction;
		projected_barrel_aim = projected_barrel_aim*vert_proj_aim_direction.Length();
		projected_barrel_aim.y = desired_aim_direction.y;

		//project to turret plane to take roll in consideration
		projected_barrel_aim = ProjectVectorOnPlane(barrel_plane_normal,projected_barrel_aim);
		
		
		projected_barrel_aim.Normalize();
		
		Float pitch_angle_to_aim_dir = GetAngleOnPlane(barrel_plane_normal,barrel_dir, projected_barrel_aim);

		/*Vec3 start = m_BarrelTransformation.GetTranslation();
		Vec3 end = start + barrel_dir*4;
		DEBUG_DRAW_LINE(start,end,Vec4(1,0,0,1));
		end = start + projected_barrel_aim*4;
		DEBUG_DRAW_LINE(start,end,Vec4(0,1,1,1));*/

	
		//Yaw calculation

		const Vec3 base_plane_normal = m_BaseTransformation.GetUpVector();
		Vec3 projected_aim = ProjectVectorOnPlane(base_plane_normal,desired_aim_direction);
		
		projected_aim.Normalize();
		const Vec3 turret_dir = -m_TurretTransformation.GetViewDirVector();
		Float yaw_angle_to_aim_dir = GetAngleOnPlane(m_TurretTransformation.GetUpVector(),turret_dir, projected_aim);

		/*start = m_TurretTransformation.GetTranslation();
		end = start + turret_dir*4;
		DEBUG_DRAW_LINE(start,end,Vec4(0,1,0,1));
		end = start + projected_aim*4;
		DEBUG_DRAW_LINE(start,end,Vec4(0,0,1,1));*/

		m_YawPID.setOutputLimit(m_MaxYawTorque);
		//set desired yaw angle difference to 0 degrees
		m_YawPID.set(0);
		float yaw_torque = m_YawPID.update(yaw_angle_to_aim_dir,delta_time);
		//std::cout << "yaw_angle_to_aim_dir:" << yaw_angle_to_aim_dir << "\n";
		m_PitchPID.setOutputLimit(m_MaxPitchTorque);
		//m_PitchPID.setGain(0.0032,0.0001,0.001);
		//set desired pitch angle difference to 0 degrees
		m_PitchPID.set(0);
		float pitch_torque = m_PitchPID.update(pitch_angle_to_aim_dir,delta_time);
		
	/*	std::stringstream ss;
		ss << "pitch_angle_to_aim_dir:" << pitch_angle_to_aim_dir << "\n"
			<< "aimpoint:" << m_AimPoint << "\n"
		<< "pitch_torque:" << pitch_torque << "\n"
		<< "yaw_angle_to_aim_dir:" << yaw_angle_to_aim_dir << "\n"
		<< "yaw_torque:" << pitch_torque << "\n"
		<< "BarrelAngle:" << Math::Rad2Deg(m_BarrelAngle) << " MaxMin"  << m_BarrelMaxAngle << m_BarrelMinAngle << "\n"
		<< "TurretAngle:" << Math::Rad2Deg(m_TurretAngle) << "\n"; 
		
		DEBUG_PRINT(ss.str());*/
		MessagePtr force_msg(new PhysicsJointMessage(PhysicsJointMessage::AXIS1_FORCE,0));//m_SteerForce));
		//MessagePtr vel_msg(new PhysicsJointMessage(PhysicsJointMessage::AXIS1_VELOCITY,turn_velocity));
		m_TurretObject->PostMessage(force_msg);
		m_BarrelObject->PostMessage(force_msg);

		//GetSceneObject()->PostMessage(vel_msg);

		MessagePtr barrel_torque_msg(new PhysicsBodyMessage(PhysicsBodyMessage::TORQUE,Vec3(pitch_torque,0,0)));
		m_BarrelObject->PostMessage(barrel_torque_msg);

		MessagePtr yaw_torque_msg(new PhysicsBodyMessage(PhysicsBodyMessage::TORQUE,Vec3(0,yaw_torque,0)));
		m_TurretObject->PostMessage(yaw_torque_msg);

		
		
	}

	void AutoAimComponent::OnLoad(LoadGameComponentsMessagePtr message)
	{
		MessagePtr play_msg(new SoundParameterMessage(SoundParameterMessage::PLAY,0));
		GetSceneObject()->PostMessage(play_msg);

		MessagePtr volume_msg(new SoundParameterMessage(SoundParameterMessage::VOLUME,0));
		GetSceneObject()->PostMessage(volume_msg);

		SimEngine::GetPtr()->GetRuntimeController()->Register(this);

		//GetSceneObject()->GetParentSceneObject()->RegisterForMessage(REG_TMESS(AutoAimComponent::OnParentTransformation,TransformationNotifyMessage,0));
		if(m_TurretObject->GetParentSceneObject())
			m_TurretObject->GetParentSceneObject()->RegisterForMessage(REG_TMESS(AutoAimComponent::OnBaseTransformation,TransformationNotifyMessage,0));


		MessagePtr force_msg(new PhysicsJointMessage(PhysicsJointMessage::AXIS1_FORCE,m_SteerForce));
		MessagePtr vel_msg(new PhysicsJointMessage(PhysicsJointMessage::AXIS1_VELOCITY,0));
		
		m_TurretObject->RegisterForMessage(REG_TMESS(AutoAimComponent::OnTurretTransformation,TransformationNotifyMessage,0));
		m_TurretObject->RegisterForMessage(REG_TMESS(AutoAimComponent::OnTurretHingeUpdate,HingeJointNotifyMessage,0));
	
		m_TurretObject->PostMessage(force_msg);
		m_TurretObject->PostMessage(vel_msg);

		
		m_BarrelObject->RegisterForMessage(REG_TMESS(AutoAimComponent::OnBarrelTransformation,TransformationNotifyMessage,0));
		m_BarrelObject->RegisterForMessage(REG_TMESS(AutoAimComponent::OnBarrelHingeUpdate,HingeJointNotifyMessage,0));
		m_BarrelObject->PostMessage(force_msg);
		m_BarrelObject->PostMessage(vel_msg);
		
	}

	void AutoAimComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
		SimEngine::GetPtr()->GetRuntimeController()->Unregister(this);
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

	

}
