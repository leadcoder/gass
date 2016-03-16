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
#include <ode/ode.h>
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/GASSSceneObjectRef.h"
#include "Core/MessageSystem/GASSIMessage.h"

namespace GASS
{
	class ODEPhysicsSceneManager;
	class ODEBodyComponent;
	typedef GASS_WEAK_PTR<ODEPhysicsSceneManager> ODEPhysicsSceneManagerWeakPtr;

	class ODEHingeComponent : public Reflection<ODEHingeComponent,BaseSceneComponent>
	{
	public:
		ODEHingeComponent();
		virtual ~ODEHingeComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
	protected:
		void OnBody1Loaded(PhysicsBodyLoadedEventPtr message);
		void OnBody2Loaded(PhysicsBodyLoadedEventPtr message);
		void OnVelocityRequest(PhysicsHingeJointVelocityRequestPtr message);
		void OnMaxTorqueRequest(PhysicsHingeJointMaxTorqueRequestPtr message);
		//void OnBodyLoaded(PhysicsBodyLoadedEventPtr message);
		void SetAxisVel(float velocity);
		float GetMaxTorque()const {return m_MaxTorque;}
		void SetMaxTorque(float value);
		//Helpers

		void CreateJoint();
		void UpdateLimits();
		void UpdateAnchor();
		void UpdateJointAxis();

		//get set section
		std::string GetType()const;
		void SetType(const std::string &type);
		
		Vec3 GetAxis()const {return m_Axis;}
		void SetAxis(const Vec3 &value);
		Vec3 GetAnchor()const {return m_Anchor;}
		void SetAnchor(const Vec3 &value);

		float GetHighStop()const {return m_HighStop;}
		void SetHighStop(float value);
		float GetLowStop()const {return m_LowStop;}
		void SetLowStop(float value);

		SceneObjectRef GetBody1() const {return m_Body1;}
		void SetBody1(SceneObjectRef value);
		SceneObjectRef GetBody2()const {return m_Body2;}
		void SetBody2(SceneObjectRef value);


	private:
		void SendJointUpdate(PhysicsVelocityEventPtr message);
		dJointID m_ODEJoint;


		SceneObjectRef m_Body1;
		SceneObjectRef m_Body2;
		bool m_Body1Loaded;
		bool m_Body2Loaded;
		dBodyID m_ODEBody1;
		dBodyID m_ODEBody2;
		
		float m_MaxTorque;
		float m_SwayForce;
		float m_Strength;
		float m_Damping;

		float m_HighStop;
		float m_LowStop;

		Vec3 m_Anchor; 
		Vec3 m_Axis;
		
		ODEPhysicsSceneManagerWeakPtr m_SceneManager;
	};
}

