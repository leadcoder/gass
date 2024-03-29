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
#include "Plugins/ODE/ODEHingeComponent.h"
#include "Plugins/ODE/ODESuspensionComponent.h"
#include "Sim/GASSComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Core/Math/GASSVector.h"
#include "Sim/Interface/GASSIPhysicsBodyComponent.h"

#include <ode/ode.h>
namespace GASS
{
	class ODEBodyComponent : public Reflection<ODEBodyComponent,Component> , 
		public IPhysicsBodyComponent
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
		~ODEBodyComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		dBodyID GetODEBodyComponent() const {return m_ODEBodyID;}
		dSpaceID GetSpace();
		MassRepresentationType GetMassRepresentation() const { return m_MassRepresentation; }
		void SetODEMass(dMass mass);
		Vec3 GetCGPosition() const {return m_CGPosition;}

		//IPhysicsBodyComponent
		void SetVelocity(const Vec3 &vel, bool relative = false) override;
		Vec3 GetVelocity(bool relative = false) const override;
		void AddForce(const Vec3 &force_vec, bool relative = false) override;
		void AddForceAtPos(const Vec3 &force_vec, const Vec3 &pos_vec, bool rel_force = false, bool rel_pos = false) override;
		void AddTorque(const Vec3 &torque_vec, bool relative = false) override;
		float GetMass() const override { return m_Mass; }
		void SetMass(float mass) override;
		
		void SetTorque(const Vec3 &torque);
		Vec3 GetTorque(bool rel = false) const;
		void SetAngularVelocity(const Vec3 &vel, bool relative = false);
		Vec3 GetAngularVelocity(bool relative = false) const;
	protected:
		void SetActive(bool value) override;
		bool GetActive() const override;
		void SetForce(const Vec3 &force);
		Vec3 GetForce(bool rel = false) const;
		
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
		Quaternion GetRotation() const;
		void BodyMoved();
		void static BodyMovedCallback(dBodyID id);
		void OnLocationLoaded(LocationLoadedEventPtr message);
		void OnTransformationChanged(TransformationChangedEventPtr event);
		void DampenBody( dBodyID body, float vScale, float aScale );
		dSpaceID GetSecondarySpace();
	protected:
		void Wake();
		dBodyID m_ODEBodyID{nullptr};
		dSpaceID m_ODESpaceID{nullptr};
		
		dSpaceID m_ODESecondarySpaceID{nullptr};
		dMass m_ODEMass;
		float m_Mass{1};
		bool m_Debug{false};
		bool m_AutoDisable{true};
		bool m_FastRotation{true};
		bool m_Active{true};
		bool m_TrackTransformation{true};
		
		Vec3 m_CGPosition;
		Vec3 m_SymmetricInertia;
		Vec3 m_AssymetricInertia;
		bool m_EffectJoints{true};
		Mat4 m_LastTrans;
		MassRepresentationType m_MassRepresentation{MR_GEOMETRY};
		ODEPhysicsSceneManagerWeakPtr m_SceneManager;
	};
	using ODEBodyComponentPtr = std::shared_ptr<ODEBodyComponent>;
}

