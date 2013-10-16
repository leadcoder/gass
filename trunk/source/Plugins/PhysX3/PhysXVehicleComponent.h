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
#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/GASSSceneObjectRef.h"

namespace GASS
{
	class PhysXPhysicsSceneManager;
	typedef WPTR<PhysXPhysicsSceneManager> PhysXPhysicsSceneManagerWeakPtr;

	class PhysXVehicleComponent : public Reflection<PhysXVehicleComponent,BaseSceneComponent>
	{
	public:
		PhysXVehicleComponent();
		virtual ~PhysXVehicleComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		physx::PxRigidDynamic* GetPxActor() {return m_Actor;}
		float GetMass() const {return m_ChassisData.mMass;}
		void SetMass(float mass);
		void SceneManagerTick(double delta);
	protected:
		void OnPostSceneObjectInitializedEvent(PostSceneObjectInitializedEventPtr message);
		void OnLocationLoaded(LocationLoadedMessagePtr message);
		void OnPositionChanged(PositionMessagePtr message);
		void OnWorldPositionChanged(WorldPositionMessagePtr message);
		void OnRotationChanged(RotationMessagePtr message);
		void OnMassMessage(PhysicsBodyMassRequestPtr message);
		void OnInput(InputControllerMessagePtr message);
		
		//reflection functions
		void SetPosition(const Vec3 &value);
		Vec3 GetPosition() const;
		void SetRotation(const Quaternion &rot);
		Quaternion GetRotation();
		
		//helpers
		void Reset();
		physx::PxVec3 ComputeDim(const physx::PxConvexMesh* cm);
		void ProcessAutoReverse(const physx::PxVehicleWheels& focusVehicle, 
					const physx::PxVehicleDriveDynData& driveDynData, 
					const physx::PxVehicleDrive4WRawInputData& carRawInputs,
		bool& toggleAutoReverse, 
		bool& newIsMovingForwardSlowly) const;
	protected:
		ADD_PROPERTY(SceneObjectRef,FrontLeftWheel);
		ADD_PROPERTY(SceneObjectRef,FrontRightWheel);
		ADD_PROPERTY(SceneObjectRef,RearLeftWheel);
		ADD_PROPERTY(SceneObjectRef,RearRightWheel);
		ADD_PROPERTY(std::vector<SceneObjectRef>,ExtraWheels)
		ADD_PROPERTY(bool,UseAutoReverse);


		std::vector<SceneObjectWeakPtr> m_AllWheels;
		bool m_Initialized;
		PhysXPhysicsSceneManagerWeakPtr m_SceneManager;
		physx::PxRigidDynamic* m_Actor;
		physx::PxVehicleDrive4W* m_Vehicle;
		float m_ThrottleInput;
		float m_SteerInput;
		physx::PxVehicleChassisData m_ChassisData;
		bool m_DigBrakeInput;
		bool m_DigAccelInput;
		bool m_IsMovingForwardSlowly;
		bool m_InReverseMode;
		bool m_UseDigitalInputs;
	};
	typedef SPTR<PhysXVehicleComponent> PhysXVehicleComponentPtr;
}
