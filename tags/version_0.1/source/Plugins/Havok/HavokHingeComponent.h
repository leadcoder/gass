/****************************************************************************
* This file is part of GASS.                                                *
* See http://cHavok.google.com/p/gass/                                 *
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
#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Scenario/Scene/Messages/PhysicsSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/GraphicsSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/CoreSceneObjectMessages.h"
#include "Core/MessageSystem/IMessage.h"

class hkpHingeConstraintData;
class hkpMotorAction;

namespace GASS
{
	class HavokPhysicsSceneManager;
	class HavokBodyComponent;
	typedef boost::weak_ptr<HavokPhysicsSceneManager> HavokPhysicsSceneManagerWeakPtr;


	class HavokHingeComponent : public Reflection<HavokHingeComponent,BaseSceneComponent>
	{
	public:
		HavokHingeComponent();
		virtual ~HavokHingeComponent();
		static void RegisterReflection();
		virtual void OnCreate();
	protected:
		void OnParameterMessage(PhysicsJointMessagePtr message);
		void OnLoad(LoadPhysicsComponentsMessagePtr message);

		void SetAxisVel(float velocity);
		float GetAxisForce()const;
		void SetAxisForce(float value);

		

		void CreateJoint();
		void UpdateSuspension();
		void UpdateLimits();
		void UpdateAnchor();
		void UpdateJointAxis();

		//get set section
		std::string GetBody1Name()const {return m_Body1Name;}
		void SetBody1Name(const std::string &name) {m_Body1Name = name;}
		std::string GetBody2Name()const {return m_Body2Name;}
		void SetBody2Name(const std::string &name) {m_Body2Name = name;}
		
		Vec3 GetAnchor()const {return m_Anchor;}
		void SetAnchor(const Vec3 &value);

		float GetHighStop()const {return m_HighStop;}
		void SetHighStop(float value);
		float GetLowStop()const {return m_LowStop;}
		void SetLowStop(float value);

		Vec3 GetAxis()const {return m_Axis;}
		void SetAxis(const Vec3 &value);

		
	private:
		void JointCorrectHinge2();
		void UpdateSwayBars(VelocityNotifyMessagePtr message);
		void SendJointUpdate(VelocityNotifyMessagePtr message);
		hkpHingeConstraintData *m_HingeConstraintData ;
		hkpMotorAction* m_MotorAction;

		std::string m_Body1Name;
		std::string m_Body2Name;

		HavokBodyComponent* m_Body1;
		HavokBodyComponent* m_Body2;
		
		float m_JointForce;
		float m_HighStop;
		float m_LowStop;

		Vec3 m_Anchor; 
		Vec3 m_Axis;

		HavokPhysicsSceneManagerWeakPtr m_SceneManager;
	};
}

