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
#include "Sim/GASSSceneObjectRef.h"


namespace GASS
{
	class PhysXPhysicsSceneManager;
	typedef WPTR<PhysXPhysicsSceneManager> PhysXPhysicsSceneManagerWeakPtr;

	class PhysXHingeComponent : public Reflection<PhysXHingeComponent,BaseSceneComponent>
	{
	public:
		PhysXHingeComponent();
		virtual ~PhysXHingeComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		void SetPosition(const Vec3 &value);
	protected:
		void OnParameterMessage(PhysicsJointMessagePtr message);
		void OnBody1Loaded(BodyLoadedMessagePtr message);
		void OnBody2Loaded(BodyLoadedMessagePtr message);
	
		float GetRollAngle();
		float GetRollAngleRate();
		void SetRollAxisVel(float velocity);
		//void SetSpringAxisVel(float value);
		//float GetSpringAxisVel(){return 0;}
		float GetRollAxisForce()const {return m_RevoluteJointForce;}
		void SetRollAxisForce(float value);		

		//Helpers
		void CreateJoint();
		void UpdateSuspension();
		void UpdateAnchor();
		void UpdateJointAxis();
		void UpdateMotor();

		//get set section
		SceneObjectRef GetBody1() const {return m_Body1;}
		void SetBody1(SceneObjectRef value);
		SceneObjectRef GetBody2()const {return m_Body2;}
		void SetBody2(SceneObjectRef value);
		float GetDamping()const {return m_Damping;}
		void SetDamping(float value){m_Damping =value;}
		float GetStrength()const {return m_Strength;}
		void SetStrength(float value){m_Strength =value;}
		Vec3 GetRollAxis()const {return m_RollAxis;}
		void SetRollAxis(const Vec3 &value);
		float GetHighStop()const {return m_HighStop;}
		void SetHighStop(float value);
		float GetLowStop()const {return m_LowStop;}
		void SetLowStop(float value);
	private:
		SceneObjectRef m_Body1;
		SceneObjectRef m_Body2;
		float m_RevoluteJointForce;
		float m_SpringJointForce;
		float m_RollAngularVelocity;
		float m_Strength;
		float m_Damping;
		float m_HighStop;
		float m_LowStop;
		bool m_Body1Loaded;
		bool m_Body2Loaded;
		Vec3 m_RollAxis;
		PhysXPhysicsSceneManagerWeakPtr m_SceneManager;
		physx::PxRevoluteJoint *m_RevoluteJoint;
	};
	typedef SPTR<PhysXHingeComponent> PhysXHingeComponentPtr;
}

