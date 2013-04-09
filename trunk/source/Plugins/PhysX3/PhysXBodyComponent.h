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
#include "PhysXCommon.h"
namespace GASS
{
	class PhysXPhysicsSceneManager;
	typedef WPTR<PhysXPhysicsSceneManager> PhysXPhysicsSceneManagerWeakPtr;

	class PhysXBodyComponent : public Reflection<PhysXBodyComponent,BaseSceneComponent>
	{
	public:
		enum MassRepresentationType
		{
			MR_GEOMETRY,
			MR_USER,
			MR_BOX,
			MR_CYLINDER,
			MR_SPHERE
		};
		PhysXBodyComponent();
		virtual ~PhysXBodyComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void SceneManagerTick(double delta_time);
		
		void SendTransformation();
		physx::PxRigidDynamic* GetPxActor() {return m_Actor;}
		float GetMass() const {return m_Mass;}
		void SetMass(float mass);

	protected:
		void OnLocationLoaded(LocationLoadedMessagePtr message);
		void OnPositionChanged(PositionMessagePtr message);
		void OnWorldPositionChanged(WorldPositionMessagePtr message);
		void OnRotationChanged(RotationMessagePtr message);
		void OnVelocity(PhysicsVelocityRequestPtr message);
		void OnAngularVelocity(PhysicsAngularVelocityRequestPtr message);
		void OnAddForce(PhysicsForceRequestPtr message);
		void OnAddTorque(PhysicsTorqueRequestPtr message);
		void OnParameterMessage(PhysicsBodyMessagePtr message);
		void OnMassMessage(PhysicsMassMessagePtr message);
		
		void SetPosition(const Vec3 &value);
		Vec3 GetPosition() const;
		void SetRotation(const Quaternion &rot);
		Quaternion GetRotation();
		void SetTorque(const Vec3 &torque);
		Vec3 GetTorque(bool rel = false);
		void SetVelocity(const Vec3 &vel);
		Vec3 GetVelocity();
		void SetAngularVelocity(const Vec3 &vel);
		Vec3 GetAngularVelocity();
		void AddForce(const Vec3 &force_vec, bool rel = false);
		void AddForceAtPos(const Vec3 &force_vec, const Vec3 &pos_vec, bool rel_force = false, bool rel_pos = false);
		void AddTorque(const Vec3 &torque_vec);
		void WakeUp();
		void SetKinematic(bool value);
		bool GetKinematic() const;
		
		//check
		Vec3 GetCGPosition() const {return m_CGPosition;}
		void SetCGPosition(const Vec3 value) {m_CGPosition = value;}
		Vec3 GetSymmetricInertia() const {return m_SymmetricInertia;}
		void SetSymmetricInertia(const Vec3 value) {m_SymmetricInertia = value;}
		Vec3 GetAssymetricInertia() const {return m_AssymetricInertia;}
		void SetAssymetricInertia(const Vec3 value) {m_AssymetricInertia = value;}
		bool GetEffectJoints() const {return m_EffectJoints;}
		void SetEffectJoints(bool value) {m_EffectJoints = value;}
		void SetMassProperties(float mass, Vec3 &CGPosition, Vec3 &symmetricInertia, Vec3 &assymetricInertia);
		MassRepresentationType GetMassRepresentation() { return m_MassRepresentation; }
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
		bool m_Kinematic;

		MassRepresentationType m_MassRepresentation;
		PhysXPhysicsSceneManagerWeakPtr m_SceneManager;
		physx::PxRigidDynamic* m_Actor;
	};
}

