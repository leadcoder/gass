/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                        *
*                                                                           *
* Copyright (c) 2008-2015 GASS team. See Contributors.txt for details.      *
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
#include "Sim/Messages/GASSSoundSceneObjectMessages.h"


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
		m_MaxYawVelocity(5),
		m_MaxPitchVelocity(10),
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
		ComponentFactory::GetPtr()->Register("AutoAimComponent",new Creator<AutoAimComponent, Component>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("AutoAimComponent", OF_VISIBLE)));
		RegisterProperty<float>("SteerForce", &AutoAimComponent::GetSteerForce, &AutoAimComponent::SetSteerForce,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<Vec2>("TurretMaxMinAngle", &AutoAimComponent::GetTurretMaxMinAngle, &AutoAimComponent::SetTurretMaxMinAngle,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Max and min angles that the turret can rotate, zero means free rotation",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<Vec2>("BarrelMaxMinAngle", &AutoAimComponent::GetBarrelMaxMinAngle, &AutoAimComponent::SetBarrelMaxMinAngle,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Max and min angles that the barrel can pitch, zero means free rotation",PF_VISIBLE | PF_EDITABLE)));

		RegisterProperty<SceneObjectRef>("BarrelObject", &AutoAimComponent::GetBarrelObject, &AutoAimComponent::SetBarrelObject,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("ID of the barrel to control",PF_VISIBLE)));
		RegisterProperty<SceneObjectRef>("TurretObject", &AutoAimComponent::GetTurretObject, &AutoAimComponent::SetTurretObject,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("ID of the turret to control",PF_VISIBLE)));
		RegisterProperty<PIDControl>("YawPID", &AutoAimComponent::GetYawPID, &AutoAimComponent::SetYawPID,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("PID values for turret rotation",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<PIDControl>("PitchPID", &AutoAimComponent::GetPitchPID, &AutoAimComponent::SetPitchPID,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("PID values for barrel pitch",PF_VISIBLE | PF_EDITABLE)));


		RegisterProperty<Float>("MaxYawVelocity", &AutoAimComponent::GetMaxYawVelocity, &AutoAimComponent::SetMaxYawVelocity,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Max velocity for turret rotation",PF_VISIBLE | PF_EDITABLE)));

		RegisterProperty<Float>("MaxPitchVelocity", &AutoAimComponent::GetMaxPitchVelocity, &AutoAimComponent::SetMaxPitchVelocity,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Max velocity for barel pitch",PF_VISIBLE | PF_EDITABLE)));
		
	}

	void AutoAimComponent::OnInitialize()
	{
		BaseSceneComponent::OnInitialize();

		GetSceneObject()->RegisterForMessage(REG_TMESS(AutoAimComponent::OnAimAtPosition,AimAtPositionMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(AutoAimComponent::OnActivateAutoAim,ActivateAutoAimMessage,0));
		
		SceneManagerListenerPtr listener = shared_from_this();
		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<GameSceneManager>()->Register(listener);
		GetSceneObject()->PostRequest(SoundParameterRequestPtr(new SoundParameterRequest(SoundParameterRequest::PLAY,0)));
		GetSceneObject()->PostRequest(SoundParameterRequestPtr(new SoundParameterRequest(SoundParameterRequest::VOLUME,0)));
		if(m_TurretObject->GetParentSceneObject())
			m_TurretObject->GetParentSceneObject()->RegisterForMessage(REG_TMESS(AutoAimComponent::OnBaseTransformation,TransformationChangedEvent,0));

		m_TurretObject->RegisterForMessage(REG_TMESS(AutoAimComponent::OnTurretTransformation,TransformationChangedEvent,0));
		m_TurretObject->RegisterForMessage(REG_TMESS(AutoAimComponent::OnTurretHingeUpdate,ODEPhysicsHingeJointEvent,0));
	
		m_TurretObject->PostRequest(PhysicsHingeJointMaxTorqueRequestPtr(new PhysicsHingeJointMaxTorqueRequest(m_SteerForce)));
		m_TurretObject->PostRequest(PhysicsHingeJointVelocityRequestPtr(new PhysicsHingeJointVelocityRequest(0)));

		
		m_BarrelObject->RegisterForMessage(REG_TMESS(AutoAimComponent::OnBarrelTransformation,TransformationChangedEvent,0));
		m_BarrelObject->RegisterForMessage(REG_TMESS(AutoAimComponent::OnBarrelHingeUpdate,ODEPhysicsHingeJointEvent,0));
		m_BarrelObject->PostRequest(PhysicsHingeJointMaxTorqueRequestPtr(new PhysicsHingeJointMaxTorqueRequest(m_SteerForce)));
		m_BarrelObject->PostRequest(PhysicsHingeJointVelocityRequestPtr(new PhysicsHingeJointVelocityRequest(0)));
		
	}

	void AutoAimComponent::OnDelete()
	{

	}


	void AutoAimComponent::SetBarrelObject(const SceneObjectRef &value)
	{
		m_BarrelObject = value;
	}

	SceneObjectRef AutoAimComponent::GetBarrelObject() const
	{
		return m_BarrelObject;
	}

	void AutoAimComponent::SetTurretObject(const SceneObjectRef &value)
	{
		m_TurretObject = value;
	}

	SceneObjectRef AutoAimComponent::GetTurretObject() const
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

	void AutoAimComponent::OnTurretTransformation(TransformationChangedEventPtr message)
	{
		m_TurretTransformation.SetTransformation(message->GetPosition(),message->GetRotation(),Vec3(1,1,1));
	}

	void AutoAimComponent::OnBarrelTransformation(TransformationChangedEventPtr message)
	{
		m_BarrelTransformation.SetTransformation(message->GetPosition(),message->GetRotation(),Vec3(1,1,1));
	}

	void AutoAimComponent::OnBaseTransformation(TransformationChangedEventPtr message)
	{
		m_BaseTransformation.SetTransformation(message->GetPosition(),message->GetRotation(),Vec3(1,1,1));
		
	}

	/*void AutoAimComponent::OnPhysicsMessage(PhysicsVelocityEventPtr message)
	{
		m_AngularVelocity = message->GetAngularVelocity().y;
		//	std::cout << "anglvel:" << ang_vel.x << " " << ang_vel.y << " " << ang_vel.z << std::endl;
	}*/

#define DEBUG_DRAW_LINE(start,end,color) GetSceneObject()->GetScene()->PostMessage(SceneMessagePtr(new DrawLineRequest(start,end,color,color)))
#define DEBUG_PRINT(text) SimEngine::Get().GetSimSystemManager()->PostMessage(SystemRequestMessagePtr(new DebugPrintRequest(text)))

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

	void AutoAimComponent::SceneManagerTick(double delta_time)
	{
		//send information about current barrel information, 
		//can be used by sight component to redirect sight to barrel
		GetSceneObject()->PostEvent(BarrelTransformationMessagePtr(new BarrelTransformationMessage(m_BarrelTransformation)));

		if(!m_Active)
		{
			//reset
			m_CurrentAimPriority = 0; 
			//GetSceneObject()->PostMessage(MessagePtr(new PhysicsHingeJointVelocityRequest(0)));
			PhysicsHingeJointMaxTorqueRequestPtr force_msg(new PhysicsHingeJointMaxTorqueRequest(m_SteerForce));
			m_TurretObject->PostRequest(force_msg);
			m_BarrelObject->PostRequest(force_msg);
			//m_BarrelObject->PostMessage(force_msg);
			m_TurretObject->PostRequest(PhysicsHingeJointVelocityRequestPtr(new PhysicsHingeJointVelocityRequest(0)));
			m_BarrelObject->PostRequest(PhysicsHingeJointVelocityRequestPtr(new PhysicsHingeJointVelocityRequest(0)));

			return;
		}
		m_GotNewAimPoint = false;
		Vec3 desired_aim_direction = m_AimPoint - m_TurretTransformation.GetTranslation();

		/*Vec3 start = m_BarrelTransformation.GetTranslation();
		Vec3 end = start + desired_aim_direction*4;
		DEBUG_DRAW_LINE(start,end,ColorRGBA(1,0,0,1));*/

		//pitch calculation

		Vec3 barrel_plane_normal = m_TurretTransformation.GetXAxis();
		barrel_plane_normal.Normalize();
		Vec3 barrel_dir = -m_BarrelTransformation.GetZAxis();
		barrel_dir.Normalize();
		
		
		//rotate in world space to turret dir, this should be inproved!

		//solution, project aimdirection to barrel up direction plane and take dot product
		Vec3 projected_barrel_aim = barrel_dir;
		projected_barrel_aim.y = 0;
		projected_barrel_aim.Normalize();
		
		Vec3 vert_proj_aim_direction = desired_aim_direction;
		projected_barrel_aim = projected_barrel_aim*vert_proj_aim_direction.Length();
		projected_barrel_aim.y = desired_aim_direction.y;

		//project to turret plane to take roll in consideration
		projected_barrel_aim = Math::ProjectVectorOnPlane(barrel_plane_normal,projected_barrel_aim);
		
		
		projected_barrel_aim.Normalize();
		
		Float pitch_angle_to_aim_dir = GetAngleOnPlane(barrel_plane_normal,barrel_dir, projected_barrel_aim);

		/*Vec3 start = m_BarrelTransformation.GetTranslation();
		Vec3 end = start + barrel_dir*4;
		DEBUG_DRAW_LINE(start,end,ColorRGBA(1,0,0,1));
		end = start + projected_barrel_aim*4;
		DEBUG_DRAW_LINE(start,end,ColorRGBA(0,1,1,1));*/

	
		//Yaw calculation

		const Vec3 base_plane_normal = m_BaseTransformation.GetYAxis();
		Vec3 projected_aim = Math::ProjectVectorOnPlane(base_plane_normal,desired_aim_direction);
		
		projected_aim.Normalize();
		const Vec3 turret_dir = -m_TurretTransformation.GetZAxis();
		Float yaw_angle_to_aim_dir = GetAngleOnPlane(m_TurretTransformation.GetYAxis(),turret_dir, projected_aim);

		/*start = m_TurretTransformation.GetTranslation();
		end = start + turret_dir*4;
		DEBUG_DRAW_LINE(start,end,ColorRGBA(0,1,0,1));
		end = start + projected_aim*4;
		DEBUG_DRAW_LINE(start,end,ColorRGBA(0,0,1,1));*/

		m_YawPID.setOutputLimit(m_MaxYawVelocity);
		//set desired yaw angle difference to 0 degrees
		m_YawPID.set(0);
		float yaw_torque = m_YawPID.update(yaw_angle_to_aim_dir,delta_time);
		//std::cout << "yaw_angle_to_aim_dir:" << yaw_angle_to_aim_dir << "\n";
		m_PitchPID.setOutputLimit(m_MaxPitchVelocity);
		//m_PitchPID.setGain(0.0032,0.0001,0.001);
		//set desired pitch angle difference to 0 degrees
		m_PitchPID.set(0);
		float pitch_torque = m_PitchPID.update(pitch_angle_to_aim_dir,delta_time);
		
		std::stringstream ss;
		ss << "pitch_angle_to_aim_dir:" << pitch_angle_to_aim_dir << "\n"
			<< "aimpoint:" << m_AimPoint << "\n"
		<< "pitch_torque:" << pitch_torque << "\n"
		<< "yaw_angle_to_aim_dir:" << yaw_angle_to_aim_dir << "\n"
		<< "yaw_torque:" << yaw_torque << "\n"
		<< "BarrelAngle:" << Math::Rad2Deg(m_BarrelAngle) << " MaxMin"  << m_BarrelMaxAngle << m_BarrelMinAngle << "\n"
		<< "TurretAngle:" << Math::Rad2Deg(m_TurretAngle) << "\n"; 
		
		DEBUG_PRINT(ss.str());

		//MessagePtr force_msg(new PhysicsHingeJointMaxTorqueRequest(0));//m_SteerForce));
	
		//MessagePtr vel_msg(new PhysicsJointMessage(PhysicsJointMessage::AXIS1_VELOCITY,turn_velocity));
		

		//m_TurretObject->PostMessage(force_msg);
		//m_BarrelObject->PostMessage(force_msg);

		//GetSceneObject()->PostMessage(vel_msg);

		//MessagePtr barrel_torque_msg(new PhysicsBodyAddTorqueRequest(Vec3(pitch_torque,0,0)));
		//m_BarrelObject->PostMessage(barrel_torque_msg);


		//MessagePtr yaw_torque_msg(new PhysicsBodyAddTorqueRequest(Vec3(0,yaw_torque,0)));
		
		//m_TurretObject->PostMessage(yaw_torque_msg);
		m_TurretObject->PostRequest(PhysicsHingeJointVelocityRequestPtr(new PhysicsHingeJointVelocityRequest(-yaw_torque)));
		m_BarrelObject->PostRequest(PhysicsHingeJointVelocityRequestPtr(new PhysicsHingeJointVelocityRequest(-pitch_torque)));
		
	}

	
	void AutoAimComponent::OnTurretHingeUpdate(ODEPhysicsHingeJointEventPtr message)
	{
		m_TurretAngle = -message->GetAngle();
		//Send turret information message
	}

	void AutoAimComponent::OnBarrelHingeUpdate(ODEPhysicsHingeJointEventPtr message)
	{
		m_BarrelAngle = -message->GetAngle();
		//Send turret information message
	}

	
	
	

}
