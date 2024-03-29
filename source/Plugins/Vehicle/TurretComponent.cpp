/****************************************************************************
* This file is part of GASS.                                                *
* See https://github.com/leadcoder/gass                                     *
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
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

#include "TurretComponent.h"

#include <memory>
#include "Sim/Interface/GASSIMissionSceneManager.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/Math/GASSPlane.h"

#include "Sim/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Math/GASSMath.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSGraphicsSceneMessages.h"
#include "Sim/Interface/GASSISoundComponent.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Interface/GASSIPhysicsBodyComponent.h"

namespace GASS
{
	TurretComponent::TurretComponent()  :m_Controller("Yaw"),
		
		m_DesiredDir(0,0,-1)
		
	{
		m_RelTrans.MakeIdentity();
	}

	TurretComponent::~TurretComponent()
	{

	}

	void TurretComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register<TurretComponent>();
		RegisterGetSet("RotationController", &TurretComponent::GetController, &TurretComponent::SetController);
		RegisterGetSet("MaxSteerVelocity", &TurretComponent::GetMaxSteerVelocity, &TurretComponent::SetMaxSteerVelocity);
		RegisterGetSet("MaxSteerAngle", &TurretComponent::GetMaxSteerAngle, &TurretComponent::SetMaxSteerAngle);
		RegisterGetSet("SteerForce", &TurretComponent::GetSteerForce, &TurretComponent::SetSteerForce);
		RegisterGetSet("MaxMinAngle", &TurretComponent::GetMaxMinAngle, &TurretComponent::SetMaxMinAngle);
	}

	void TurretComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(TurretComponent::OnInput,InputRelayEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(TurretComponent::OnJointUpdate,ODEPhysicsHingeJointEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(TurretComponent::OnTransformation,TransformationChangedEvent,0));
		GetSceneObject()->GetParentSceneObject()->RegisterForMessage(REG_TMESS(TurretComponent::OnParentTransformation,TransformationChangedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(TurretComponent::OnPhysicsMessage, PhysicsVelocityEvent,0));

		m_Hinge = GetSceneObject()->GetFirstComponentByClass<IPhysicsHingeJointComponent>();
		m_Hinge->SetDriveForceLimit(m_SteerForce);
		m_Hinge->SetDriveTargetVelocity(0);
		//GetSceneObject()->PostRequest(PhysicsHingeJointMaxTorqueRequestPtr(new PhysicsHingeJointMaxTorqueRequest(m_SteerForce)));
		//GetSceneObject()->PostRequest(PhysicsHingeJointVelocityRequestPtr(new PhysicsHingeJointVelocityRequest(0)));

		auto sound = GetSceneObject()->GetFirstComponentByClass<ISoundComponent>();
		if (sound)
		{
			sound->SetPlay(true);
			sound->SetVolume(0);
		}

		RegisterForPostUpdate<IMissionSceneManager>();
	}

	void TurretComponent::OnDelete()
	{

	}

	void TurretComponent::OnTransformation(TransformationChangedEventPtr message)
	{
		m_Transformation.MakeTransformationRT(message->GetRotation(), message->GetPosition());
	}

	void TurretComponent::OnParentTransformation(TransformationChangedEventPtr message)
	{
		m_ParentTransformation.MakeTransformationRT(message->GetRotation(), message->GetPosition());
	}

	void TurretComponent::OnPhysicsMessage(PhysicsVelocityEventPtr message)
	{
		m_AngularVelocity = message->GetAngularVelocity().y;
	//	std::cout << "anglvel:" << ang_vel.x << " " << ang_vel.y << " " << ang_vel.z << std::endl;
	}

//#define DEBUG_DRAW_LINE(start,end,color) GetSceneObject()->GetScene()->PostMessage(SceneMessagePtr(new DrawLineRequest(start,end,color,color)));


/*	Float pitch TurretComponent::GetPitch(Mat4 &transform, Vec3 &dir)
	{
		Mat4 inv_trans = transform.Invert();
		Vec3 new_dir = inv_trans * dir;
	}*/


	Vec3 TurretComponent::GetDesiredAimDirection(double delta_time)
	{
		Mat4 rot_mat;
		if(!((m_CurrentAngle < m_MinAngle && m_TurnInput > 0) ||
				(m_CurrentAngle > m_MaxAngle && m_TurnInput < 0)))
				//outside envelope
		{
			m_RotValue = m_RotValue + m_TurnInput*m_MaxSteerVelocity*delta_time;
			//m_RotValue = m_TurnInput*m_MaxSteerVelocity*delta_time;
		}
		//else
		//	m_RotValue = 0;
		

		if(m_Controller == "Pitch")
		{
			Vec3 rot;
			m_ParentTransformation.ToEulerAnglesYXZ(rot);
			rot_mat.MakeRotationYXZ(Vec3(m_RotValue, rot.y, 0));
		}
		else
			rot_mat.MakeRotationY(m_RotValue);

		//add relative transformation, we start rotation from this transformation
		m_RelTrans.SetTranslation(Vec3(0,0,0));
		//Mat4 parent_rot = m_RelTrans;
		//parent_rot.SetTranslation(0,0,0);
		m_AimTrans = rot_mat*m_RelTrans;

		return -m_AimTrans.GetZAxis();
	}


	Float TurretComponent::GetAngleOnPlane(const Vec3 &plane_normal,const Vec3 &v1,const Vec3 &v2) const
	{
		Vec3 cross = Vec3::Cross(v1,v2);
		auto cos_angle = static_cast<float>(Vec3::Dot(v1,v2));
		if(cos_angle > 1) cos_angle = 1;
		if(cos_angle < -1) cos_angle = -1;
		auto angle = static_cast<float>(Math::Rad2Deg(acos(cos_angle)));
		if(Vec3::Dot(plane_normal,cross) > 0)
			angle *= -1;
		return angle;
	}


	Float TurretComponent::GetPitchAngle(const Vec3 v1,const Vec3 v2) const
	{
		//Vec3 cross = Vec3::Cross(v1,v2);
		auto cos_angle = static_cast<float>(Vec3::Dot(v1,v2));
		if(cos_angle > 1) cos_angle = 1;
		if(cos_angle < -1) cos_angle = -1;
		auto angle = static_cast<float>(Math::Rad2Deg(acos(cos_angle)));
		if(v1.y < v2.y)
			angle *= -1;
		return angle;
	}

	void TurretComponent::SceneManagerTick(double delta_time)
	{

		Vec3 turret_dir = -m_Transformation.GetZAxis();
		if(!m_Active)
		{
			//m_DesiredDir = turret_dir;
			m_RotValue = 0;
			m_RelTrans = m_ParentTransformation;
			m_Hinge->SetDriveForceLimit(m_SteerForce);
			m_Hinge->SetDriveTargetVelocity(0);
			//GetSceneObject()->PostRequest(PhysicsHingeJointMaxTorqueRequestPtr(new PhysicsHingeJointMaxTorqueRequest(m_SteerForce)));
			//GetSceneObject()->PostRequest(PhysicsHingeJointVelocityRequestPtr(new PhysicsHingeJointVelocityRequest(0)));
			auto sound = GetSceneObject()->GetFirstComponentByClass<ISoundComponent>();
			if (sound)
			{
				sound->SetVolume(0);
			}
			return;
		}

		Vec3 desired_aim_direction = GetDesiredAimDirection(delta_time);


		Float angle_to_aim_dir = 0;
		if(m_Controller == "Pitch")
		{
			const Vec3 plane_normal = m_ParentTransformation.GetXAxis();
			Vec3 projected_aim = Plane(Vec3(0, 0, 0), plane_normal).GetProjectedVector(desired_aim_direction);
			projected_aim.Normalize();
			angle_to_aim_dir = GetAngleOnPlane(plane_normal,turret_dir, projected_aim);

			Vec3 start = m_Transformation.GetTranslation();
			Vec3 end = start + turret_dir;
			GetSceneObject()->GetScene()->DrawDebugLine(start, end, ColorRGBA(0, 1, 0, 1), ColorRGBA(0, 1, 0, 1));
			end = start + projected_aim;
			GetSceneObject()->GetScene()->DrawDebugLine(start, end, ColorRGBA(0, 0, 1, 1), ColorRGBA(0, 0, 1, 1));
		}
		else
		{
			const Vec3 plane_normal = m_ParentTransformation.GetYAxis();
			Vec3 projected_aim = Plane(Vec3(0, 0, 0), plane_normal).GetProjectedVector(desired_aim_direction);
			projected_aim.Normalize();
			angle_to_aim_dir = GetAngleOnPlane(plane_normal,turret_dir, projected_aim);


		}



		/*Float angle1 = GetAngle(-m_ParentTransformation.GetViewDirVector(), desired_aim_direction);
		Float angle2 = GetAngle(-m_ParentTransformation.GetViewDirVector(), -m_Transformation.GetViewDirVector());

		Float angle_to_aim_dir =  angle1 - angle2;
		while(angle_to_aim_dir > 180)
			angle_to_aim_dir = angle_to_aim_dir - 180;

		while(angle_to_aim_dir < -180)
			angle_to_aim_dir = angle_to_aim_dir + 180;*/



		/*Vec3 turrent_direction(0,0,-1);

		Mat4 invers_rot_mat = m_Transformation;
		invers_rot_mat.SetTranslation(0,0,0);
		invers_rot_mat = invers_rot_mat.Invert();
		desired_aim_direction = invers_rot_mat*desired_aim_direction;

		Vec3 start = m_Transformation.GetTranslation();
		Vec3 end = start + turrent_direction;
		DEBUG_DRAW_LINE(start,end,ColorRGBA(0,1,0,1))
		end = start + desired_aim_direction;
		DEBUG_DRAW_LINE(start,end,ColorRGBA(0,0,1,1))


		Vec3 cross = Vec3::Cross(turrent_direction,desired_aim_direction);
		float cos_angle = Vec3::Dot(turrent_direction,desired_aim_direction);
		if(cos_angle > 1) cos_angle = 1;
		if(cos_angle < -1) cos_angle = -1;
		float angle_to_aim_dir = Math::Rad2Deg(acos(cos_angle));
		if(cross.y > 0)
			angle_to_aim_dir *= -1;*/

		//std::cout << "angle_to_aim_dir:" << angle_to_aim_dir << "\n";;



	/*	if(m_Controller == "Pitch")
		{
			Mat4 rot_mat;
			if((m_CurrentAngle < m_MinAngle && m_TurnInput > 0) ||
				(m_CurrentAngle > m_MaxAngle && m_TurnInput < 0))
				//ouside envelope
			{

			}
			else
			{
				m_RotValue += m_TurnInput*m_MaxSteerVelocity*delta_time;
				rot_mat.RotateX(m_RotValue);
				Mat4 trans = rot_mat*m_Transformation;
				m_DesiredDir = -trans.GetViewDirVector();


				Mat4 invers_mat = m_Transformation;
				invers_mat.SetTranslation(0,0,0);
				invers_mat = invers_mat.Invert();
				Vec3 desiredDir = invers_mat*m_DesiredDir;

				Vec3 t_dir(0,0,-1);

				Vec3 start = m_Transformation.GetTranslation();
				Vec3 end = start + t_dir;
				DEBUG_DRAW_LINE(start,end,ColorRGBA(0,1,0,1))
				end = start + desiredDir;
				DEBUG_DRAW_LINE(start,end,ColorRGBA(0,0,1,1))


				Vec3 cross = Vec3::Cross(t_dir,desiredDir);
				float cos_angle = Vec3::Dot(t_dir,desiredDir);
				if(cos_angle > 1) cos_angle = 1;
				if(cos_angle < -1) cos_angle = -1;
				float angle_to_aim_dir = Math::Rad2Deg(acos(cos_angle));

				std::cout << "angle_to_aim_dir:" << angle_to_aim_dir << "\n";;
				//m_RotValue += m_TurnInput*m_MaxSteerVelocity*delta_time;
				//rot.FromAngleAxis(m_RotValue,Vec3(1,0,0));
				//rot_mat.RotateX(m_TurnInput*m_MaxSteerVelocity*delta_time);
				//m_DesiredDir = rot*Vec3(0,0,-1);
				//m_DesiredDir.x = 0;
				//m_DesiredDir.z = sqrt(turret_dir.z*turret_dir.z+turret_dir.x*turret_dir.x);
				//if(turret_dir.z < 0)
				//	m_DesiredDir.z = -m_DesiredDir.z;


			}
		}
		else if(m_Controller == "Yaw")
		{
			Mat4 rot_mat;
			if((m_CurrentAngle < m_MinAngle && m_TurnInput > 0) ||
				(m_CurrentAngle > m_MaxAngle && m_TurnInput < 0))
				//ouside envelope
			{

			}
			else
			{
				rot_mat.RotateY(m_TurnInput*m_MaxSteerVelocity*delta_time);
				m_DesiredDir = rot_mat*m_DesiredDir;
			}
			//std::cout << "value:" << m_TurnInput << " dir:" << m_DesiredDir << "\n";
		}






		Vec3 aim_dir = m_DesiredDir;

		if(m_Controller == "Pitch")
		{


			//turret_dir.z = sqrt(turret_dir.z*turret_dir.z+turret_dir.x*turret_dir.x);
			//turret_dir.x
			//	if(turret_dir.z < 0)
			//		m_DesiredDir.z = -m_DesiredDir.z;
			//turret_dir.z = -sqrt(turret_dir.z*turret_dir.z + turret_dir.x*turret_dir.x);
			//turret_dir.x = 0;
			//turret_dir.Normalize();

		}
		else if(m_Controller == "Yaw")
		{
			turret_dir.y = 0;
			turret_dir.Normalize();
		}
		Vec3 cross = Vec3::Cross(turret_dir,aim_dir);
		float cos_angle = Vec3::Dot(turret_dir,aim_dir);
		if(cos_angle > 1) cos_angle = 1;
		if(cos_angle < -1) cos_angle = -1;
		float angle_to_aim_dir = Math::Rad2Deg(acos(cos_angle));

		if(m_Controller == "Pitch")
		{
			if(turret_dir.y > aim_dir.y)
			angle_to_aim_dir *= -1;
			//std::cout << "angle_to_aim_dir:" << angle_to_aim_dir << "\n";
		}
		else if (m_Controller == "Yaw")
		{
			if(cross.y < 0)
			angle_to_aim_dir *= -1;
			//std::cout << "CurrentAngle:" << m_CurrentAngle << "\n";
		}*/

		//std::cout << "angle diff:" << angle_to_aim_dir << "\n";



	/*	if(angle_to_aim_dir > 4)
			angle_to_aim_dir = 4;

		if(angle_to_aim_dir < -4)
			angle_to_aim_dir = -4;

			*/
		//angle_to_aim_dir = angle_to_aim_dir/4.0;
		//angle_to_aim_dir = angle_to_aim_dir*fabs(angle_to_aim_dir)*fabs(angle_to_aim_dir);






		//float turn_velocity = -angle_to_aim_dir*m_MaxSteerVelocity*4;
		m_TurnPID.SetOutputLimit(m_SteerForce*3);
		m_TurnPID.SetGain(0.00003,0.0001,0.0009);
		m_TurnPID.Set(0);
		auto turn_velocity = static_cast<float>(m_TurnPID.Update(angle_to_aim_dir,delta_time));
		//std::cout << "turn_velocity:" << turn_velocity << "\n";



		//MessagePtr force_msg(new PhysicsJointMessage(PhysicsJointMessage::AXIS1_FORCE,0));//m_SteerForce));
		//MessagePtr vel_msg(new PhysicsJointMessage(PhysicsJointMessage::AXIS1_VELOCITY,turn_velocity));
		//GetSceneObject()->PostMessage(force_msg);
		//GetSceneObject()->PostMessage(vel_msg);

		//GetSceneObject()->PostRequest(PhysicsHingeJointMaxTorqueRequestPtr(new PhysicsHingeJointMaxTorqueRequest(0)));
		m_Hinge->SetDriveForceLimit(0);
	
		if(m_Controller == "Pitch")
		{
			GetSceneObject()->GetFirstComponentByClass<IPhysicsBodyComponent>()->AddTorque(Vec3(turn_velocity, 0, 0));
			//std::cout << "angle_to_aim_dir:" << angle_to_aim_dir << "\n";;
			//std::cout << "angle_to_aim_dir:" << angle_to_aim_dir <<"\n";;
			//std::cout << "THeading:" << t_heading << " AHeading:" << a_heading << "\n";
			//std::cout << "TPitch:" << t_pitch << " APitch:" << a_pitch << "\n";
		}
		else if (m_Controller == "Yaw")
		{
			GetSceneObject()->GetFirstComponentByClass<IPhysicsBodyComponent>()->AddTorque(Vec3(0, turn_velocity, 0));
			//std::cout << "angle_to_aim_dir:" << angle_to_aim_dir << "\n";;
		}
		//MessagePtr volume_msg(new SoundParameterRequest(SoundParameterRequest::VOLUME,fabs(turn )));
		//GetSceneObject()->PostMessage(volume_msg);

	}



	void TurretComponent::OnInput(InputRelayEventPtr message)
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
				m_TurnInput = 0;
				std::cout << "deactivate\n";
			}
		}
		else if (m_Active && name == m_Controller)
		{
			m_TurnInput = value;


			//send rotaion message to physics engine

			//if((m_CurrentAngle > m_MinAngle && value > 0) || (m_CurrentAngle < m_MaxAngle && value < 0))
			/*	{
			MessagePtr force_msg(new PhysicsJointMessage(PhysicsJointMessage::AXIS1_FORCE,m_SteerForce));
			MessagePtr vel_msg(new PhysicsJointMessage(PhysicsJointMessage::AXIS1_VELOCITY,m_MaxSteerVelocity*value));

			GetSceneObject()->PostMessage(force_msg);
			GetSceneObject()->PostMessage(vel_msg);

			MessagePtr volume_msg(new SoundParameterRequest(SoundParameterRequest::VOLUME,fabs(value)));
			GetSceneObject()->PostMessage(volume_msg);
			}*/

			/*if(fabs(value) > 0)
			{
			MessagePtr play_msg(new SoundParameterRequest(SoundParameterRequest::PLAY,0));
			GetSceneObject()->PostMessage(play_msg);
			}
			else
			{
			MessagePtr play_msg(new SoundParameterRequest(SoundParameterRequest::STOP,0));
			GetSceneObject()->PostMessage(play_msg);
			}*/
		}
	}

	void TurretComponent::OnJointUpdate(ODEPhysicsHingeJointEventPtr message)
	{
		m_CurrentAngle = message->GetAngle();
		//Send turret information message
	}

	/*void TurretComponent::OnRotation(PhysicsVelocityEventPtr message)
	{
	Vec3 ang_vel  = message->GetAngularVelocity();
	const float speed = fabs(ang_vel.y);
	const float max_volume_at_speed = 1;
	if(speed < max_volume_at_speed)
	{
	//std::cout << speed << std::endl;
	//Play engine sound
	const float volume = (speed/max_volume_at_speed);
	MessagePtr sound_msg(new SoundParameterRequest(SoundParameterRequest::VOLUME,volume));
	GetSceneObject()->PostMessage(sound_msg);
	}
	if(speed > 0)
	{
	float pitch = 0.8 + speed*0.01;
	MessagePtr sound_msg(new SoundParameterRequest(SoundParameterRequest::PITCH,pitch));
	GetSceneObject()->PostMessage(sound_msg);
	}
	}*/
}
