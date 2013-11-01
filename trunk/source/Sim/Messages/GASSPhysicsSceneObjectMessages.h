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
	Event casted by joints to inform about applied forces.
	*/
	class PhysicsJointForceEvent : public BaseMessage
	{
	public:
		PhysicsJointForceEvent(const Vec3 &force, const Vec3 &torq, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Force(force), m_Torque(torq)
		  {

		  }
		  Vec3 GetForce()const {return m_Force;}
		  Vec3 GetTorque()const {return m_Torque;}
	private:
		Vec3 m_Force;
		Vec3 m_Torque;
	};
	typedef SPTR<PhysicsJointForceEvent > PhysicsJointForceEventPtr;

	/**
	Event casted by joints to inform about local velocities.
	*/
	class PhysicsJointVelocityEvent : public BaseMessage
	{
	public:
		PhysicsJointVelocityEvent(const Vec3 &linear, const Vec3 &angular, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Linear(linear), m_Angular(angular)
		  {

		  }
		  Vec3 GetLinearVelocity()const {return m_Linear;}
		  Vec3 GetAngularVelocity()const {return m_Angular;}
	private:
		Vec3 m_Linear;
		Vec3 m_Angular;
	};
	typedef SPTR<PhysicsJointVelocityEvent> PhysicsJointVelocityEventPtr;
	
	/**
	Set desired linear velocity for prismatic joint. 
	*/
	class PhysicsPrismaticJointVelocityRequest : public BaseMessage
	{
	public:
		PhysicsPrismaticJointVelocityRequest(Float velocity, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Velocity(velocity)
		  {

		  }
		  Float GetVelocity()const {return m_Velocity;}
	private:
		Float m_Velocity;
	};
	typedef SPTR<PhysicsPrismaticJointVelocityRequest> PhysicsPrismaticJointVelocityRequestPtr;

	/**
	Set desired position for prismatic joint. 
	*/
	class PhysicsPrismaticJointPositionRequest : public BaseMessage
	{
	public:
		PhysicsPrismaticJointPositionRequest(Float position, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Position(position)
		  {

		  }
		  Float GetPosition()const {return m_Position;}
	private:
		Float m_Position;
	};
	typedef SPTR<PhysicsPrismaticJointPositionRequest> PhysicsPrismaticJointPositionRequestPtr;


	/**
	Set max force used by a prismatic joint to get to it's desired velocity 
	*/

	class PhysicsPrismaticJointMaxForceRequest : public BaseMessage
	{
	public:
		PhysicsPrismaticJointMaxForceRequest(Float max_force, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_MaxForce(max_force)
		  {

		  }
		  Float GetMaxForce()const {return m_MaxForce;}
	private:
		Float m_MaxForce;
	};
	typedef SPTR<PhysicsPrismaticJointMaxForceRequest> PhysicsPrismaticJointMaxForceRequestPtr;

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
	Set max torque used by a hinge joint to get to it's desired velocity 
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
		PhysicsSuspensionJointMaxSteerTorqueRequest(Float torque, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_MaxTorque(torque)
		  {

		  }
		  Float GetMaxTorque()const {return m_MaxTorque;}
	private:
		Float m_MaxTorque;
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
	Request to change the state of a physics bodies.
	*/
	class PhysicsBodyStateRequest : public BaseMessage
	{

	public:
		enum PhysicsBodyState
		{
			ENABLE,
			DISABLE
		};

		PhysicsBodyStateRequest(PhysicsBodyState state, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Value(state)
		  {

		  }
		  PhysicsBodyState GetState()const {return m_Value;}
	private:
		PhysicsBodyState m_Value;
	};
	typedef SPTR<PhysicsBodyStateRequest> PhysicsBodyStateRequestPtr;



	/**
	Request to change the velocity of physics bodies.
	*/

	class PhysicsBodyVelocityRequest : public BaseMessage
	{
	public:
		PhysicsBodyVelocityRequest(Vec3 value, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Value(value)
		  {

		  }
		  Vec3 GetVelocity()const {return m_Value;}
	private:
		Vec3 m_Value;
	};
	typedef SPTR<PhysicsBodyVelocityRequest> PhysicsBodyVelocityRequestPtr;
	

	/**
	Request to change the angular velocity of physics bodies.
	*/

	class PhysicsBodyAngularVelocityRequest : public BaseMessage
	{
	public:
		PhysicsBodyAngularVelocityRequest(Vec3 value, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Value(value)
		  {

		  }
		  Vec3 GetAngularVelocity()const {return m_Value;}
	private:
		Vec3 m_Value;
	};
	typedef SPTR<PhysicsBodyAngularVelocityRequest> PhysicsBodyAngularVelocityRequestPtr;

	/**
	Request to add force to physics bodies.
	*/

	class PhysicsBodyAddForceRequest : public BaseMessage
	{
	public:
		PhysicsBodyAddForceRequest(Vec3 value, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Value(value)
		  {

		  }
		  Vec3 GetForce()const {return m_Value;}
	private:
		Vec3 m_Value;
	};
	typedef SPTR<PhysicsBodyAddForceRequest> PhysicsBodyAddForceRequestPtr;



	/**
	Request to add torque to physics bodies.
	*/

	class PhysicsBodyAddTorqueRequest : public BaseMessage
	{
	public:
		PhysicsBodyAddTorqueRequest(Vec3 value, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Value(value)
		  {

		  }
		  Vec3 GetTorque()const {return m_Value;}
	private:
		Vec3 m_Value;
	};
	typedef SPTR<PhysicsBodyAddTorqueRequest> PhysicsBodyAddTorqueRequestPtr;

	/**
	Message used to change mass of physics bodies.
	*/
	class PhysicsBodyMassRequest : public BaseMessage
	{
	public:
		PhysicsBodyMassRequest(Float mass, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Value(mass)
		  {

		  }
		  Float GetMass()const {return m_Value;}
	private:
		Float m_Value;
	};

	typedef SPTR<PhysicsBodyMassRequest> PhysicsBodyMassRequestPtr;

	/*class PhysicsSuspensionWheelVelocityRequest : public BaseMessage
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
	typedef SPTR<PhysicsSuspensionSteerVelocityRequest> PhysicsSuspensionSteerVelocityRequestPtr;*/

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
