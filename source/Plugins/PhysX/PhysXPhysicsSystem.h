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

#ifndef PHYS_X_PHYSICS_SYSTEM
#define PHYS_X_PHYSICS_SYSTEM

#include "PhysXCommon.h"
#include "Sim/GASSPhysicsMaterialManager.h"


namespace physx
{
	class PxPhysics;
}
namespace GASS
{
	class IMeshComponent;
	class PhysicsMesh;

	struct TireData
	{
		std::string Name;
		std::map<std::string,double> FrictionMultipliers;
	};

	class PhysXPhysicsSystem : public Reflection<PhysXPhysicsSystem, SimSystem>
	{
	public:
		static void RegisterReflection();
		PhysXPhysicsSystem(SimSystemManagerWeakPtr manager);
		~PhysXPhysicsSystem() override;
		void OnSystemInit() override;
		physx::PxPhysics* GetPxSDK() const {return m_PhysicsSDK;}
		physx::PxMaterial* GetDefaultMaterial() const {return m_DefaultMaterial;}
		physx::PxMaterial* GetMaterial(const std::string &name) const;
		physx::PxDefaultAllocator* GetAllocator() {return &m_DefaultAllocator;}
		std::string GetSystemName() const override {return "PhysXPhysicsSystem";}
		physx::PxCooking* GetPxCooking() const {return m_Cooking;}
		physx::PxVehicleDrivableSurfaceToTireFrictionPairs* GetSurfaceTirePairs()const {return m_SurfaceTirePairs;}
		int GetTireIDFromName(const std::string &name) const;
		
	protected:
		void LoadTires(const std::string &file);
	private:
		
		// PhysX
		physx::PxPhysics* m_PhysicsSDK;
		physx::PxFoundation* m_Foundation;
		physx::PxMaterial* m_DefaultMaterial;
		physx::PxDefaultAllocator m_DefaultAllocator;
		physx::PxCooking* m_Cooking;

		//vehicle data
		std::vector<TireData> m_Tires;
		std::vector<std::string> m_DrivableMaterialNames;

		std::vector<physx::PxVehicleDrivableSurfaceType> m_VehicleDrivableSurfaceTypes;;
		std::vector<physx::PxMaterial*> m_DrivableMaterials;
		std::map<std::string,physx::PxMaterial*> m_Materials;
		physx::PxVehicleDrivableSurfaceToTireFrictionPairs* m_SurfaceTirePairs;
		int m_MaxNumThreads;
		PhysicsMaterialManager m_MaterialManager;
	};

	using PhysXPhysicsSystemPtr = std::shared_ptr<PhysXPhysicsSystem>;
}

#endif