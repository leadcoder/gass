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

#pragma once

#include "Sim/GASSCommon.h"
#include "Core/MessageSystem/GASSBaseMessage.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSQuaternion.h"

namespace GASS
{
	class SimSceneManager;
	class ISceneManager;
	class IGeometryComponent;
	struct ManualMeshData;
	typedef SPTR<ManualMeshData> ManualMeshDataPtr;
	typedef SPTR<ISceneManager> SceneManagerPtr;
	typedef SPTR<IGeometryComponent> GeometryComponentPtr;

	//*********************************************************
	// ALL MESSAGES IN THIS SECTION CAN BE POSTED BY USER
	//*********************************************************

	/**
	Message used to change collisiion settings,
	Typically the physics system has
	components that respond to this message
	by disable/enable collision models.

	*/
	class CollisionSettingsMessage : public BaseMessage
	{
	public:
		CollisionSettingsMessage(bool enable, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Enable(enable)
		  {

		  }
		  bool EnableCollision() const {return m_Enable;}
	private:
		bool m_Enable;
	};
	typedef SPTR<CollisionSettingsMessage> CollisionSettingsMessagePtr;

	/**
	Message used to enable/disable physics debugging
	*/

	class PhysicsDebugMessage : public BaseMessage
	{
	public:
		PhysicsDebugMessage(bool show_collision_geometry, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_DebugGeometry(show_collision_geometry)
		  {

		  }
		  bool DebugGeometry() const {return m_DebugGeometry;}
	private:
		bool m_DebugGeometry;
	};

	typedef SPTR<PhysicsDebugMessage> PhysicsDebugMessagePtr;

	/**
	Set desired angular velocity for hinge joint. 
	*/
	class PhysicsHingeJointVelocityRequest : public BaseMessage
	{
	public:
		PhysicsHingeJointVelocityRequest(Float velocity, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Velocity(velocity)
		  {

		  }
		  Float GetVelocity()const {return m_Velocity;}
	private:
		Float m_Velocity;
	};
	typedef SPTR<PhysicsHingeJointVelocityRequest> PhysicsHingeJointVelocityRequestPtr;

	/**
	Set max torque used by a hinge joint to get to i'ts desired velocity 
	*/
	class PhysicsHingeJointMaxTorqueRequest : public BaseMessage
	{
	public:
		PhysicsHingeJointMaxTorqueRequest(Float max_torque, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_MaxTorque(max_torque)
		  {

		  }
		  Float GetMaxTorque()const {return m_MaxTorque;}
	private:
		Float m_MaxTorque;
	};
	typedef SPTR<PhysicsHingeJointMaxTorqueRequest> PhysicsHingeJointMaxTorqueRequestPtr;

	/**
	Set desired angular velocity of the suspension joints drive axis
	*/
	class PhysicsSuspensionJointDriveVelocityRequest: public BaseMessage
	{
	public:
		PhysicsSuspensionJointDriveVelocityRequest(Float velocity, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Velocity(velocity)
		  {

		  }
		  Float GetVelocity()const {return m_Velocity;}
	private:
		Float m_Velocity;
	};
	typedef SPTR<PhysicsSuspensionJointDriveVelocityRequest> PhysicsSuspensionJointDriveVelocityRequestPtr;


	/**
	Set max torque used by the suspension joints drive axis to get to i'ts desired velocity 
	*/
	class PhysicsSuspensionJointMaxDriveTorqueRequest : public BaseMessage
	{
	public:
		PhysicsSuspensionJointMaxDriveTorqueRequest(Float max_torque, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_MaxTorque(max_torque)
		  {

		  }
		  Float GetMaxTorque()const {return m_MaxTorque;}
	private:
		Float m_MaxTorque;
	};
	typedef SPTR<PhysicsSuspensionJointMaxDriveTorqueRequest> PhysicsSuspensionJointMaxDriveTorqueRequestPtr;
	
	/**
	Set max torque used by the suspension joints steer axis to get to i'ts desired velocity 
	*/
	class PhysicsSuspensionJointMaxSteerTorqueRequest : public BaseMessage
	{
	public:
		PhysicsSuspensionJointMaxSteerTorqueRequest(Float force, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Force(force)
		  {

		  }
		  Float GetForce()const {return m_Force;}
	private:
		Float m_Force;
	};
	typedef SPTR<PhysicsSuspensionJointMaxSteerTorqueRequest> PhysicsSuspensionJointMaxSteerTorqueRequestPtr;

	/**
	Set desired angular velocity of the suspension joints steer axis
	*/
	
	class PhysicsSuspensionJointSteerVelocityRequest: public BaseMessage
	{
	public:
		PhysicsSuspensionJointSteerVelocityRequest(Float velocity, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Velocity(velocity)
		  {

		  }
		  Float GetVelocity()const {return m_Velocity;}
	private:
		Float m_Velocity;
	};
	typedef SPTR<PhysicsSuspensionJointSteerVelocityRequest> PhysicsSuspensionJointSteerVelocityRequestPtr;



	/**
	Message used to interact with physics bodies.
	The user provide a paramerter type and a
	value for that type. Physics body implementations
	are responsible to suscribe to this message.
	*/
	class PhysicsBodyMessage : public BaseMessage
	{
	public:
		enum PhysicsBodyParameterType
		{
			TORQUE,
			FORCE,
			VELOCITY,
			ENABLE,
			DISABLE
		};
	public:
		PhysicsBodyMessage(PhysicsBodyParameterType parameter, Vec3 value, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Value(value), m_Parameter(parameter)
		  {

		  }
		  Vec3 GetValue()const {return m_Value;}
		  PhysicsBodyParameterType GetParameter()const {return m_Parameter;}
	private:
		PhysicsBodyParameterType m_Parameter;
		Vec3 m_Value;
	};
	typedef SPTR<PhysicsBodyMessage> PhysicsBodyMessagePtr;

	class PhysicsVelocityRequest : public BaseMessage
	{
	public:
		PhysicsVelocityRequest(Vec3 value, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Value(value)
		  {

		  }
		  Vec3 GetVelocity()const {return m_Value;}
	private:
		Vec3 m_Value;
	};
	typedef SPTR<PhysicsVelocityRequest> PhysicsVelocityRequestPtr;
	
	class PhysicsAngularVelocityRequest : public BaseMessage
	{
	public:
		PhysicsAngularVelocityRequest(Vec3 value, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Value(value)
		  {

		  }
		  Vec3 GetAngularVelocity()const {return m_Value;}
	private:
		Vec3 m_Value;
	};
	typedef SPTR<PhysicsAngularVelocityRequest> PhysicsAngularVelocityRequestPtr;

	
	class PhysicsForceRequest : public BaseMessage
	{
	public:
		PhysicsForceRequest(Vec3 value, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Value(value)
		  {

		  }
		  Vec3 GetForce()const {return m_Value;}
	private:
		Vec3 m_Value;
	};
	typedef SPTR<PhysicsForceRequest> PhysicsForceRequestPtr;

	class PhysicsTorqueRequest : public BaseMessage
	{
	public:
		PhysicsTorqueRequest(Vec3 value, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Value(value)
		  {

		  }
		  Vec3 GetTorque()const {return m_Value;}
	private:
		Vec3 m_Value;
	};
	typedef SPTR<PhysicsTorqueRequest> PhysicsTorqueRequestPtr;

	/**
	Message used to change mass of physics bodies.
	*/
	class PhysicsMassMessage : public BaseMessage
	{
	public:
		PhysicsMassMessage(Float mass, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Value(mass)
		  {

		  }
		  Float GetMass()const {return m_Value;}
	private:
		Float m_Value;
	};

	typedef SPTR<PhysicsMassMessage> PhysicsMassMessagePtr;

	class PhysicsSuspensionWheelVelocityRequest : public BaseMessage
	{
	public:
		PhysicsSuspensionWheelVelocityRequest(float target_velocity, float max_force = -1, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_TargetVel(target_velocity), m_MaxForce(max_force)
		  {

		  }
		  float GetTargetVelocity()const {return m_TargetVel;}
		  float GetMaxForce()const {return m_MaxForce;}
	private:
		float m_TargetVel;
		float m_MaxForce;
	};
	typedef SPTR<PhysicsSuspensionWheelVelocityRequest> PhysicsSuspensionWheelVelocityRequestPtr;
	
	class PhysicsSuspensionSteerVelocityRequest : public BaseMessage
	{
	public:
		PhysicsSuspensionSteerVelocityRequest(float target_velocity, float max_force = -1, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_TargetVel(target_velocity), m_MaxForce(max_force)
		  {

		  }
		  float GetTargetVelocity()const {return m_TargetVel;}
		  float GetMaxForce()const {return m_MaxForce;}
	private:
		float m_TargetVel;
		float m_MaxForce;
	};
	typedef SPTR<PhysicsSuspensionSteerVelocityRequest> PhysicsSuspensionSteerVelocityRequestPtr;

	//*********************************************************
	// ALL MESSAGES BELOW SHOULD ONLY BE POSTED GASS INTERNALS
	//*********************************************************

	class BodyLoadedMessage : public BaseMessage
	{
	public:
		BodyLoadedMessage(SenderID sender_id = -1, double delay= 0) :
		  BaseMessage( sender_id , delay){}
	private:
	};
	typedef SPTR<BodyLoadedMessage> BodyLoadedMessagePtr;


	class VelocityNotifyMessage : public BaseMessage
	{
	public:
		VelocityNotifyMessage(const Vec3  &linear_velocity, const Vec3  &angular_velocity, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_LinearVel(linear_velocity), m_AngularVel(angular_velocity){}
		  Vec3 GetLinearVelocity() const {return m_LinearVel;}
		  Vec3 GetAngularVelocity() const {return m_AngularVel;}
	private:
		Vec3 m_LinearVel;
		Vec3 m_AngularVel;
	};
	typedef SPTR<VelocityNotifyMessage> VelocityNotifyMessagePtr;

	class HingeJointNotifyMessage : public BaseMessage
	{
	public:
		HingeJointNotifyMessage(float angle,float angle_rate, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Angle(angle), m_AngleRate(angle_rate){}
		  float GetAngle() const {return m_Angle;}
		  float GetAngleRate() const {return m_AngleRate;}
	private:
		float m_Angle;
		float m_AngleRate;
	};
	typedef SPTR<HingeJointNotifyMessage> HingeJointNotifyMessagePtr;

	
}
