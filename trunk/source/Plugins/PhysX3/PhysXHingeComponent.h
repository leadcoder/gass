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

#include "Plugins/PhysX3/PhysXBaseGeometryComponent.h"
#include <extensions/PxRevoluteJoint.h>
#include <extensions/PxPrismaticJoint.h>
#include "PhysXCommon.h"
#include "Sim/GASS.h"

namespace GASS
{
	class PhysXPhysicsSceneManager;
	typedef boost::weak_ptr<PhysXPhysicsSceneManager> PhysXPhysicsSceneManagerWeakPtr;

	class PhysXHingeComponent : public Reflection<PhysXHingeComponent,BaseSceneComponent>
	{
	public:
		PhysXHingeComponent();
		virtual ~PhysXHingeComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		void SetPosition(const Vec3 &value);
	protected:
		
		void OnPositionChanged(PositionMessagePtr message);
		void OnWorldPositionChanged(WorldPositionMessagePtr message);
		
		void OnParameterMessage(PhysicsJointMessagePtr message);
		void OnLoad(BodyLoadedMessagePtr message);

		//virtual void UpdateTransformation();
		float GetRollAngle();
		float GetRollAngleRate();
		void SetRollAxisVel(float velocity);
		//void SetSpringAxisVel(float value);
		//float GetSpringAxisVel(){return 0;}
		float GetRollAxisForce()const {return m_RollJointForce;}
		void SetRollAxisForce(float value);//{ m_JointForce = value;}
		//virtual void SetSpringAxisForce(float value);
		//float GetSpringAxisForce()const;
		
	

		//Helpers

		void CreateJoint();
		void UpdateSuspension();
		//void UpdateLimits();
		void UpdateAnchor();
		void UpdateJointAxis();


		void UpdateMotor();

		//get set section

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
		Vec3 GetRollAxis()const {return m_RollAxis;}
		void SetRollAxis(const Vec3 &value);
		Vec3 GetSpringAxis()const {return m_SpringAxis;}
		void SetSpringAxis(const Vec3 &value);
		Vec3 GetAnchor()const {return m_Anchor;}
		void SetAnchor(const Vec3 &value);

		float GetHighStop()const {return m_HighStop;}
		void SetHighStop(float value);
		float GetLowStop()const {return m_LowStop;}
		void SetLowStop(float value);


		

		
	private:
		//PxJoint* m_PhysXHingeComponent;

		std::string m_Body1Name;
		std::string m_Body2Name;
		
		
		float m_RollJointForce;
		float m_SpringJointForce;
		float m_RollAngularVelocity;

		float m_SwayForce;
		float m_Strength;
		float m_Damping;

		float m_HighStop;
		float m_LowStop;

		Vec3 m_Anchor; 
		Vec3 m_RollAxis;
		Vec3 m_SpringAxis;

		PhysXPhysicsSceneManagerWeakPtr m_SceneManager;
		
		physx::PxRevoluteJoint *m_RollJoint;
		physx::PxD6Joint *m_Joint;
	};
	typedef boost::shared_ptr<PhysXHingeComponent> PhysXHingeComponentPtr;
}

