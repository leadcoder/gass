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
#include <ode/ode.h>
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Core/MessageSystem/GASSIMessage.h"


namespace GASS
{
	class ODEPhysicsSceneManager;
	class ODEBodyComponent;
	typedef boost::weak_ptr<ODEPhysicsSceneManager> ODEPhysicsSceneManagerWeakPtr;


	class ODESuspensionComponent : public Reflection<ODESuspensionComponent,BaseSceneComponent>
	{
	public:
		ODESuspensionComponent();
		virtual ~ODESuspensionComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
	protected:
		void OnParameterMessage(PhysicsJointMessagePtr message);
		void OnBodyLoaded(BodyLoadedMessagePtr message);

		void SetAxis1Vel(float velocity);
		void SetAxis2Vel(float value);
		float GetAxis1Force()const {return m_JointForce;}
		void SetAxis1Force(float value);
		void SetAxis2Force(float value);
		float GetAxis2Force()const;
		
		//Helpers

		void CreateJoint();
		void UpdateSuspension();
		void UpdateLimits();
		void UpdateAnchor();
		void UpdateJointAxis();

		//get set section
		
		float GetDamping()const {return m_Damping;}
		void SetDamping(float value);
		float GetStrength()const {return m_Strength;}
		void SetStrength(float value);
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
		void JointCorrectHinge2();
		//void UpdateSwayBars(VelocityNotifyMessagePtr message);
		void SendJointUpdate(VelocityNotifyMessagePtr message);
		dJointID m_ODEJoint;
		
		ODEBodyComponent* m_Body1;
		ODEBodyComponent* m_Body2;
		
		float m_JointForce;
		float m_SwayForce;
		float m_Strength;
		float m_Damping;

		float m_HighStop;
		float m_LowStop;

		Vec3 m_Anchor; 
		Vec3 m_Axis1;
		Vec3 m_Axis2;
		ODEPhysicsSceneManagerWeakPtr m_SceneManager;
	};
}

