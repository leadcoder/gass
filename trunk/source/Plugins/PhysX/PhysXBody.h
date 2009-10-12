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
	class PhysXPhysicsSceneManager;
	class PhysXBody : public Reflection<PhysXBody,BaseSceneComponent>
	{
		friend class ODEJoint;
		friend class ODEGeometry;
	public:
		enum MassRepresentationType
		{
			MR_GEOMETRY,
			MR_USER,
			MR_BOX,
			MR_CYLINDER,
			MR_SPHERE
		};
		PhysXBody();
		virtual ~PhysXBody();
		static void RegisterReflection();
		virtual void OnCreate();
		void AddShape(NxShapeDesc* shape);
	protected:
		void SetTorque(const Vec3 &torque);
		Vec3 GetTorque(bool rel = false);
		void SetVelocity(const Vec3 &vel, bool rel = false);
		Vec3 GetVelocity(bool rel = false);
		void SetAngularVelocity(const Vec3 &vel, bool rel = false);
		Vec3 GetAngularVelocity(bool rel = false);
		void Enable();
		void Disable();
		bool IsEnabled();
		void AddForce(const Vec3 &force_vec, bool rel = false);
		void AddForceAtPos(const Vec3 &force_vec, const Vec3 &pos_vec, bool rel_force = false, bool rel_pos = false);
		void SetForce(const Vec3 &force);
		Vec3 GetMomentum();
		void AddTorque(const Vec3 &torque_vec, bool rel = false);
		
		//reflection functions
		float GetMass() const {return m_Mass;}
		void SetMass(float mass);

		Vec3 GetCGPosition() const {return m_CGPosition;}
		void SetCGPosition(const Vec3 value) {m_CGPosition = value;}
		Vec3 GetSymmetricInertia() const {return m_SymmetricInertia;}
		void SetSymmetricInertia(const Vec3 value) {m_SymmetricInertia = value;}
		Vec3 GetAssymetricInertia() const {return m_AssymetricInertia;}
		void SetAssymetricInertia(const Vec3 value) {m_AssymetricInertia = value;}
		bool GetEffectJoints() const {return m_EffectJoints;}
		void SetEffectJoints(bool value) {m_EffectJoints = value;}

//		void SetODEMass(dMass mass);
		
		void SetMassProperties(float mass, Vec3 &CGPosition, Vec3 &symmetricInertia, Vec3 &assymetricInertia);
		MassRepresentationType GetMassRepresentation() { return m_MassRepresentation; }

		void SetPosition(const Vec3 &value);
		Vec3 GetPosition() const;
		void SetRotation(const Quaternion &rot);
		Quaternion GetRotation();

		void BodyMoved();
		void OnLoad(LoadPhysicsComponentsMessagePtr message);
		void OnPositionChanged(PositionMessagePtr message);
		void OnRotationChanged(RotationMessagePtr message);
		void OnParameterMessage(PhysicsBodyMessagePtr message);
		
	protected:
		float m_Mass;
		float m_Density;
		bool m_Debug;
		float m_BodyMass;
		Vec3 m_CGPosition;
		Vec3 m_SymmetricInertia;
		Vec3 m_AssymetricInertia;
		bool m_Initialized;
		bool m_EffectJoints;
		MassRepresentationType m_MassRepresentation;
		PhysXPhysicsSceneManager* m_SceneManager;

		NxActor *m_Actor;
		NxActorDesc m_ActorDesc;
	};
}

