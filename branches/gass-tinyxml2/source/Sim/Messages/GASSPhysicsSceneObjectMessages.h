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
	typedef SPTR<ISceneManager> SceneManagerPtr;
	typedef SPTR<IGeometryComponent> GeometryComponentPtr;

	/**
	Message used to change collision settings,
	Typically the physics system has
	components that respond to this message
	by disable/enable collision models.
	*/

	class CollisionSettingsRequest : public SceneObjectRequestMessage
	{
	public:
		CollisionSettingsRequest(bool enable, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_Enable(enable)
		  {

		  }
		  bool EnableCollision() const {return m_Enable;}
	private:
		bool m_Enable;
	};
	typedef SPTR<CollisionSettingsRequest> CollisionSettingsRequestPtr;

	/**
	Message used to request debugging info
	*/

	class PhysicsDebugRequest : public SceneObjectRequestMessage
	{
	public:
		PhysicsDebugRequest(bool show_collision_geometry, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_DebugGeometry(show_collision_geometry)
		  {

		  }
		  bool DebugGeometry() const {return m_DebugGeometry;}
	private:
		bool m_DebugGeometry;
	};
	typedef SPTR<PhysicsDebugRequest> PhysicsDebugRequestPtr;


	
	/**
	Set desired linear velocity for prismatic joint. 
	*/
	class PhysicsPrismaticJointVelocityRequest : public SceneObjectRequestMessage
	{
	public:
		PhysicsPrismaticJointVelocityRequest(Float velocity, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_Velocity(velocity)
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
	class PhysicsPrismaticJointPositionRequest : public SceneObjectRequestMessage
	{
	public:
		PhysicsPrismaticJointPositionRequest(Float position, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_Position(position)
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

	class PhysicsPrismaticJointMaxForceRequest : public SceneObjectRequestMessage
	{
	public:
		PhysicsPrismaticJointMaxForceRequest(Float max_force, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_MaxForce(max_force)
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
	class PhysicsHingeJointVelocityRequest : public SceneObjectRequestMessage
	{
	public:
		PhysicsHingeJointVelocityRequest(Float velocity, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_Velocity(velocity)
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

	class PhysicsHingeJointMaxTorqueRequest : public SceneObjectRequestMessage
	{
	public:
		PhysicsHingeJointMaxTorqueRequest(Float max_torque, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_MaxTorque(max_torque)
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
	class PhysicsSuspensionJointDriveVelocityRequest: public SceneObjectRequestMessage
	{
	public:
		PhysicsSuspensionJointDriveVelocityRequest(Float velocity, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_Velocity(velocity)
		  {

		  }
		  Float GetVelocity()const {return m_Velocity;}
	private:
		Float m_Velocity;
	};
	typedef SPTR<PhysicsSuspensionJointDriveVelocityRequest> PhysicsSuspensionJointDriveVelocityRequestPtr;


	/**
	Set max torque used by the suspension joints drive axis to get to it's desired velocity 
	*/
	class PhysicsSuspensionJointMaxDriveTorqueRequest : public SceneObjectRequestMessage
	{
	public:
		PhysicsSuspensionJointMaxDriveTorqueRequest(Float max_torque, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_MaxTorque(max_torque)
		  {

		  }
		  Float GetMaxTorque()const {return m_MaxTorque;}
	private:
		Float m_MaxTorque;
	};
	typedef SPTR<PhysicsSuspensionJointMaxDriveTorqueRequest> PhysicsSuspensionJointMaxDriveTorqueRequestPtr;
	
	/**
	Set max torque used by the suspension joints steer axis to get to it's desired velocity 
	*/
	class PhysicsSuspensionJointMaxSteerTorqueRequest : public SceneObjectRequestMessage
	{
	public:
		PhysicsSuspensionJointMaxSteerTorqueRequest(Float torque, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_MaxTorque(torque)
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
	
	class PhysicsSuspensionJointSteerVelocityRequest: public SceneObjectRequestMessage
	{
	public:
		PhysicsSuspensionJointSteerVelocityRequest(Float velocity, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_Velocity(velocity)
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
	class PhysicsBodyStateRequest : public SceneObjectRequestMessage
	{

	public:
		enum PhysicsBodyState
		{
			ENABLE,
			DISABLE
		};

		PhysicsBodyStateRequest(PhysicsBodyState state, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_Value(state)
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

	class PhysicsBodyVelocityRequest : public SceneObjectRequestMessage
	{
	public:
		PhysicsBodyVelocityRequest(Vec3 value, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_Value(value)
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

	class PhysicsBodyAngularVelocityRequest : public SceneObjectRequestMessage
	{
	public:
		PhysicsBodyAngularVelocityRequest(Vec3 value, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_Value(value)
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

	class PhysicsBodyAddForceRequest : public SceneObjectRequestMessage
	{
	public:
		PhysicsBodyAddForceRequest(Vec3 value, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_Value(value)
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

	class PhysicsBodyAddTorqueRequest : public SceneObjectRequestMessage
	{
	public:
		PhysicsBodyAddTorqueRequest(Vec3 value, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_Value(value)
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
	class PhysicsBodyMassRequest : public SceneObjectRequestMessage
	{
	public:
		PhysicsBodyMassRequest(Float mass, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_Value(mass)
		  {

		  }
		  Float GetMass()const {return m_Value;}
	private:
		Float m_Value;
	};
	typedef SPTR<PhysicsBodyMassRequest> PhysicsBodyMassRequestPtr;

	///////////Event section///////////
	
	/**
	Event casted by body components to inform that body is created and loaded.
	*/

	class PhysicsBodyLoadedEvent : public SceneObjectEventMessage
	{
	public:
		PhysicsBodyLoadedEvent(SenderID sender_id = -1, double delay= 0) :
		  SceneObjectEventMessage( sender_id , delay){}
	private:
	};
	typedef SPTR<PhysicsBodyLoadedEvent> PhysicsBodyLoadedEventPtr;


	/**
	Event casted by hinge joints to inform about applied forces and velocities.
	*/
	class PhysicsHingeJointReportEvent : public SceneObjectEventMessage
	{
	public:
		PhysicsHingeJointReportEvent(Float target_velocity, const Vec3 &force, const Vec3 &torq, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectEventMessage(sender_id , delay), m_TargetVelocity(target_velocity), m_Force(force), m_Torque(torq)
		  {

		  }
		  
		  Float GetTargetVelocity()const {return m_TargetVelocity;}
		  Vec3 GetForce()const {return m_Force;}
		  Vec3 GetTorque()const {return m_Torque;}
	private:
		Vec3 m_Force;
		Vec3 m_Torque;
		Float m_TargetVelocity;
	};
	typedef SPTR<PhysicsHingeJointReportEvent> PhysicsHingeJointReportEventPtr;

	/**
	Event casted by joints to inform about local velocities.
	*/

	class PhysicsJointVelocityEvent : public SceneObjectEventMessage
	{
	public:
		PhysicsJointVelocityEvent(const Vec3 &linear, const Vec3 &angular, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectEventMessage(sender_id , delay), m_Linear(linear), m_Angular(angular)
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
		Event holding body linear and rotation velocity
	*/
	class PhysicsVelocityEvent : public SceneObjectEventMessage
	{
	public:
		PhysicsVelocityEvent(const Vec3  &linear_velocity, const Vec3  &angular_velocity, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectEventMessage(sender_id , delay), m_LinearVel(linear_velocity), m_AngularVel(angular_velocity){}
		  Vec3 GetLinearVelocity() const {return m_LinearVel;}
		  Vec3 GetAngularVelocity() const {return m_AngularVel;}
	private:
		Vec3 m_LinearVel;
		Vec3 m_AngularVel;
	};
	typedef SPTR<PhysicsVelocityEvent> PhysicsVelocityEventPtr;


	/**
		Deprecated event casted only by ode joint
	*/

	class ODEPhysicsHingeJointEvent : public SceneObjectEventMessage
	{
	public:
		ODEPhysicsHingeJointEvent(float angle,float angle_rate, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectEventMessage(sender_id , delay), m_Angle(angle), m_AngleRate(angle_rate){}
		  float GetAngle() const {return m_Angle;}
		  float GetAngleRate() const {return m_AngleRate;}
	private:
		float m_Angle;
		float m_AngleRate;
	};
	typedef SPTR<ODEPhysicsHingeJointEvent> ODEPhysicsHingeJointEventPtr;
}
