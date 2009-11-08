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

#include "Plugins/PhysX/PhysXGeometry.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Scenario/Scene/SceneObjectMessages.h"
#include "Core/Math/Vector.h"
namespace GASS
{

	enum JointType
	{
		UNIVERSAL_JOINT,
		BALL_JOINT,
		SLIDER_JOINT,
		HINGE_JOINT
	};

	class PhysXPhysicsSceneManager;
	class PhysXJoint : public Reflection<PhysXJoint,BaseSceneComponent>
	{
	public:
		PhysXJoint();
		virtual ~PhysXJoint();
		static void RegisterReflection();
		virtual void OnCreate();
		void SetPosition(const Vec3 &value);
	protected:
		NxJoint* m_PhysXJoint;

		void OnParameterMessage(PhysicsJointMessagePtr message);
		void OnLoad(LoadPhysicsComponentsMessagePtr message);

		//virtual void UpdateTransformation();
		virtual float GetAngle();
		virtual float GetAngleRate();
		virtual void SetAxis1Vel(float velocity);
		virtual void SetAxis2Vel(float value);
		virtual float GetAxis2Vel(){return 0;}
		float GetAxis1Force()const {return m_JointForce1;}
		void SetAxis1Force(float value);//{ m_JointForce = value;}
		virtual void SetAxis2Force(float value);
		float GetAxis2Force()const;
		
		virtual void Enable();
		virtual void Disable();


		//Helpers

		void CreateJoint();
		void UpdateSuspension();
		void UpdateLimits();
		void UpdateAnchor();
		void UpdateJointAxis();


		void UpdateMotor();

		//get set section

		std::string GetType()const;
		void SetType(const std::string &type);
		std::string GetBody1Name()const {return m_Body1Name;}
		void SetBody1Name(const std::string &name) {m_Body1Name = name;}
		std::string GetBody2Name()const {return m_Body2Name;}
		void SetBody2Name(const std::string &name) {m_Body2Name = name;}
		
		float GetDamping()const {return m_Damping;}
		void SetDamping(float value){m_Damping =value;}
		float GetStrength()const {return m_Strength;}
		void SetStrength(float value){m_Strength =value;}
		float GetSwayForce()const {return m_SwayForce;}
		void SetSwayForce(float value);
		Vec3 GetAxis1()const {return m_Axis1;}
		void SetAxis1(const Vec3 &value);
		Vec3 GetAxis2()const {return m_Axis2;}
		void SetAxis2(const Vec3 &value);
		Vec3 GetAnchor()const {return m_Anchor;}
		void SetAnchor(const Vec3 &value);

		float GetHighStop()const {return m_HighStop;}
		void SetHighStop(float value);
		float GetLowStop()const {return m_LowStop;}
		void SetLowStop(float value);


		

		
	private:
		std::string m_Body1Name;
		std::string m_Body2Name;
		
		JointType m_Type;
		
		float m_JointForce1;
		float m_JointForce2;
		float m_AngularVelocity1;
		float m_AngularVelocity2;


		float m_SwayForce;
		float m_Strength;
		float m_Damping;

		float m_HighStop;
		float m_LowStop;

		Vec3 m_Anchor; 
		Vec3 m_Axis1;
		Vec3 m_Axis2;

		PhysXPhysicsSceneManager* m_SceneManager;

		//axis actor
		NxActor *m_AxisActor;
	};
	typedef boost::shared_ptr<PhysXJoint> PhysXJointPtr;
}

