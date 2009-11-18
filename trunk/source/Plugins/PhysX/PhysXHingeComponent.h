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

#include "Plugins/PhysX/PhysXGeometryComponent.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Scenario/Scene/SceneObjectMessages.h"
#include "Core/Math/Vector.h"
namespace GASS
{
	class PhysXPhysicsSceneManager;
	class PhysXHingeComponent : public Reflection<PhysXHingeComponent,BaseSceneComponent>
	{
	public:
		PhysXHingeComponent();
		virtual ~PhysXHingeComponent();
		static void RegisterReflection();
		virtual void OnCreate();
	protected:
		
		void OnParameterMessage(PhysicsJointMessagePtr message);
		void OnLoad(LoadPhysicsComponentsMessagePtr message);

		//Helpers
		float GetAngle();
		float GetAngleRate();
		void SetAxisVel(float velocity);
		float GetAxisForce()const {return m_JointForce;}
		void SetAxisForce(float value);
	
		void CreateJoint();
		void UpdateSuspension();
		void UpdateLimits();
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
		Vec3 GetAxis()const {return m_Axis;}
		void SetAxis(const Vec3 &value);
		Vec3 GetAnchor()const {return m_Anchor;}
		void SetAnchor(const Vec3 &value);
		float GetHighStop()const {return m_HighStop;}
		void SetHighStop(float value);
		float GetLowStop()const {return m_LowStop;}
		void SetLowStop(float value);

	private:
		std::string m_Body1Name;
		std::string m_Body2Name;
		
		float m_JointForce;
		float m_AngularVelocity;

		float m_Strength;
		float m_Damping;

		float m_HighStop;
		float m_LowStop;

		Vec3 m_Anchor; 
		Vec3 m_Axis;

		PhysXPhysicsSceneManager* m_SceneManager;
		NxRevoluteJoint* m_RevoluteJoint;

	};
	typedef boost::shared_ptr<PhysXHingeComponent> PhysXHingeComponentPtr;
}

