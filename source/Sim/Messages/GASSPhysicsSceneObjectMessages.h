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

#pragma once

#include "Sim/GASSCommon.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Math/GASSVector.h"

namespace GASS
{
	class SimSceneManager;
	class ISceneManager;
	class IGeometryComponent;
	typedef GASS_SHARED_PTR<ISceneManager> SceneManagerPtr;
	typedef GASS_SHARED_PTR<IGeometryComponent> GeometryComponentPtr;

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
	typedef GASS_SHARED_PTR<CollisionSettingsRequest> CollisionSettingsRequestPtr;

	
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
	typedef GASS_SHARED_PTR<PhysicsPrismaticJointVelocityRequest> PhysicsPrismaticJointVelocityRequestPtr;

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
	typedef GASS_SHARED_PTR<PhysicsPrismaticJointPositionRequest> PhysicsPrismaticJointPositionRequestPtr;


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
	typedef GASS_SHARED_PTR<PhysicsPrismaticJointMaxForceRequest> PhysicsPrismaticJointMaxForceRequestPtr;

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
	typedef GASS_SHARED_PTR<PhysicsHingeJointVelocityRequest> PhysicsHingeJointVelocityRequestPtr;

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
	typedef GASS_SHARED_PTR<PhysicsHingeJointMaxTorqueRequest> PhysicsHingeJointMaxTorqueRequestPtr;
	

	///////////Event section///////////
	
	/**
	Event casted by body components to inform that body is created and loaded.
	*/

	class PhysicsBodyLoadedEvent : public SceneObjectEventMessage
	{
	public:
		PhysicsBodyLoadedEvent(SenderID sender_id = -1, double delay= 0) :
		  SceneObjectEventMessage( sender_id , delay){}
	};
	typedef GASS_SHARED_PTR<PhysicsBodyLoadedEvent> PhysicsBodyLoadedEventPtr;


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
	typedef GASS_SHARED_PTR<PhysicsHingeJointReportEvent> PhysicsHingeJointReportEventPtr;

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
	typedef GASS_SHARED_PTR<PhysicsJointVelocityEvent> PhysicsJointVelocityEventPtr;

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
	typedef GASS_SHARED_PTR<PhysicsVelocityEvent> PhysicsVelocityEventPtr;


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
	typedef GASS_SHARED_PTR<ODEPhysicsHingeJointEvent> ODEPhysicsHingeJointEventPtr;
}
