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
	Messages used to interact with joint components usually found in
	the physics system. This message should probably be divided into
	separate messages (one for force, one for velocity ec.) but
	for now we use enums to specify what parameter we want to change.
	*/

	class PhysicsJointMessage : public BaseMessage
	{
	public:
		enum PhysicsJointParameterType
		{
			AXIS1_VELOCITY,
			AXIS2_VELOCITY,
			AXIS1_FORCE,
			AXIS2_FORCE,
			AXIS1_DESIRED_ANGLE
		};
	public:
		PhysicsJointMessage(PhysicsJointParameterType parameter, float value, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay), m_Value(value), m_Parameter(parameter)
		  {

		  }
		  float GetValue()const {return m_Value;}
		  PhysicsJointParameterType GetParameter()const {return m_Parameter;}
	private:
		PhysicsJointParameterType m_Parameter;
		float m_Value;
	};
	typedef SPTR<PhysicsJointMessage> PhysicsJointMessagePtr;

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
