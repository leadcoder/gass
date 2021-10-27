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
#include "Plugins/PhysX/PhysXBaseGeometryComponent.h"
#include "PhysXCommon.h"
#include "IPhysXRigidDynamic.h"
#include "Sim/Interface/GASSIPhysicsBodyComponent.h"

namespace GASS
{
	class PhysXPhysicsSceneManager;
	using PhysXPhysicsSceneManagerWeakPtr = std::weak_ptr<PhysXPhysicsSceneManager>;

	class PhysXBodyComponent : public Reflection<PhysXBodyComponent,Component>, public IPhysXRigidDynamic, public IPhysicsBodyComponent
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
		~PhysXBodyComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;
		void SceneManagerTick(double delta_time) override;
		
		void SetPosition(const Vec3 &value);
		Vec3 GetPosition() const;
		void SetRotation(const Quaternion &rot);
		Quaternion GetRotation();

		//IPhysicsBodyComponent
		void SetVelocity(const Vec3 &vel, bool relative = false) override;
		Vec3 GetVelocity(bool relative = false) const override;
		void SetAngularVelocity(const Vec3 &vel, bool relative = false);
		Vec3 GetAngularVelocity(bool relative = false) const;
		void AddForce(const Vec3 &force_vec, bool relative = false) override;
		void AddForceAtPos(const Vec3 &force_vec, const Vec3 &pos_vec, bool rel_force = false, bool rel_pos = false) override;
		void AddTorque(const Vec3 &torque_vec, bool relative = false) override;
		float GetMass() const override { return m_Mass; }
		void SetMass(float mass) override;
		void SetActive(bool value) override;
		bool GetActive() const override;

		//IPhysXBody
		physx::PxRigidDynamic* GetPxRigidDynamic() const override {return m_Actor;}
		
		void WakeUp();
	protected:
		void OnLocationLoaded(LocationLoadedEventPtr message);
		void OnTransformationChanged(TransformationChangedEventPtr event);

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
		MassRepresentationType GetMassRepresentation() const { return m_MassRepresentation; }
	protected:
		float m_Mass{1};
		float m_Density;
		bool m_Debug;
		float m_BodyMass;
		Vec3 m_CGPosition;
		Vec3 m_SymmetricInertia;
		Vec3 m_AssymetricInertia;
		bool m_Initialized;
		bool m_EffectJoints{false};
		bool m_Kinematic{false};
		bool m_TrackTransformation{true};
		bool m_Active{true};
		bool m_DisableGravity{false};
		bool m_ForceReport{false};
		int m_PositionIterCount{4};
		int m_VelocityIterCount{4};
	
		MassRepresentationType m_MassRepresentation{MR_GEOMETRY};
		PhysXPhysicsSceneManagerWeakPtr m_SceneManager;
		physx::PxRigidDynamic* m_Actor{nullptr};
		ILocationComponent* m_LocationComponent{nullptr};
		Vec3 m_LastPos;
	};
}

