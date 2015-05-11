/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                        *
*                                                                           *
* Copyright (c) 2008-2015 GASS team. See Contributors.txt for details.      *
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
#include "Plugins/ODE/ODEHingeComponent.h"
#include "Plugins/ODE/ODESuspensionComponent.h"

#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Core/Math/GASSVector.h"

#include <ode/ode.h>
namespace GASS
{
	class ODEBodyComponent : public Reflection<ODEBodyComponent,BaseSceneComponent>
	{
		friend class ODEJoint;
		friend class ODEHingeComponent;
		friend class ODESuspensionComponent;
	public:
		enum MassRepresentationType
		{
			MR_GEOMETRY,
			MR_USER,
			MR_BOX,
			MR_CYLINDER,
			MR_SPHERE
		};
		ODEBodyComponent();
		virtual ~ODEBodyComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		dBodyID GetODEBodyComponent(){return m_ODEBodyID;}
		dSpaceID GetSpace();
		MassRepresentationType GetMassRepresentation() { return m_MassRepresentation; }
		float GetMass() const {return m_Mass;}
		void SetMass(float mass);
		void SetODEMass(dMass mass);
		Vec3 GetCGPosition() const {return m_CGPosition;}
	protected:
		void OnVelocity(PhysicsBodyVelocityRequestPtr message);
		void OnAngularVelocity(PhysicsBodyAngularVelocityRequestPtr message);
		void OnAddForce(PhysicsBodyAddForceRequestPtr message);
		void OnAddTorque(PhysicsBodyAddTorqueRequestPtr message);

		void SetTorque(const Vec3 &torque);
		Vec3 GetTorque(bool rel = false);
		void SetVelocity(const Vec3 &vel, bool rel = false);
		Vec3 GetVelocity(bool rel = false);
		void SetAngularVelocity(const Vec3 &vel, bool rel = false);
		Vec3 GetAngularVelocity(bool rel = false);
		void SetActive(bool value);
		bool GetActive() const;
		void AddForce(const Vec3 &force_vec, bool rel = false);
		void AddForceAtPos(const Vec3 &force_vec, const Vec3 &pos_vec, bool rel_force = false, bool rel_pos = false);
		void SetForce(const Vec3 &force);
		Vec3 GetForce(bool rel = false);
		void AddTorque(const Vec3 &torque_vec, bool rel = false);
		//reflection functions
		
		void SetCGPosition(const Vec3 &value) {m_CGPosition = value;}
		Vec3 GetSymmetricInertia() const {return m_SymmetricInertia;}
		void SetSymmetricInertia(const Vec3 &value) {m_SymmetricInertia = value;}
		Vec3 GetAssymetricInertia() const {return m_AssymetricInertia;}
		void SetAssymetricInertia(const Vec3 &value) {m_AssymetricInertia = value;}
		bool GetEffectJoints() const {return m_EffectJoints;}
		void SetEffectJoints(bool value) {m_EffectJoints = value;}
		void SetMassProperties(float mass, Vec3 &CGPosition, Vec3 &symmetricInertia, Vec3 &assymetricInertia);
		void SetPosition(const Vec3 &value);
		Vec3 GetPosition() const;
		void SetRotation(const Quaternion &rot);
		Quaternion GetRotation();
		void BodyMoved();
		void static BodyMovedCallback(dBodyID id);
		void OnLocationLoaded(LocationLoadedEventPtr message);
		void OnPositionChanged(PositionRequestPtr message);
		void OnWorldPositionChanged(WorldPositionRequestPtr message);
		void OnRotationChanged(RotationRequestPtr message);
		void OnWorldRotationChanged(WorldRotationRequestPtr message);
		void OnMassMessage(PhysicsBodyMassRequestPtr message);
		void OnStateRequest(PhysicsBodyStateRequestPtr message);
		void DampenBody( dBodyID body, float vScale, float aScale );
		dSpaceID GetSecondarySpace();
	protected:
		void Wake();
		dBodyID m_ODEBodyID;
		dSpaceID m_ODESpaceID;
		
		dSpaceID m_ODESecondarySpaceID;
		dMass m_ODEMass;
		float m_Mass;
		bool m_Debug;
		bool m_AutoDisable;
		bool m_FastRotation;
		bool m_Active;
		
		Vec3 m_CGPosition;
		Vec3 m_SymmetricInertia;
		Vec3 m_AssymetricInertia;
		bool m_EffectJoints;
		Mat4 m_LastTrans;
		MassRepresentationType m_MassRepresentation;
		ODEPhysicsSceneManagerWeakPtr m_SceneManager;
	};
	typedef SPTR<ODEBodyComponent> ODEBodyComponentPtr;
}

