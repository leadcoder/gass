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
#include "PhysXCommon.h"
#include "Sim/Interface/GASSICollisionSceneManager.h"
#include "Sim/Interface/GASSIPhysicsSceneManager.h"

#define MAX_NUM_WHEELS 256

namespace physx
{
	class PxScene;
}

namespace GASS
{
	class VehicleSceneQueryData;
	class VehicleWheelQueryResults;

	struct PhysXConvexMesh
	{
		physx::PxConvexMesh* m_ConvexMesh;
	};

	struct PhysXTriangleMesh
	{
		physx::PxTriangleMesh* m_TriangleMesh;	
	};

	class PhysXBodyComponent;
	typedef GASS_SHARED_PTR<PhysXBodyComponent> PhysXBodyComponentPtr;

	class PhysXPhysicsSceneManager  : public Reflection<PhysXPhysicsSceneManager, BaseSceneManager>, public IPhysicsSceneManager
	{
	public:
		typedef std::map<std::string,PhysXConvexMesh> ConvexMeshMap;
		typedef std::map<std::string,PhysXTriangleMesh> TriangleMeshMap;
		PhysXPhysicsSceneManager(SceneWeakPtr scene);
		~PhysXPhysicsSceneManager() override;
		static void RegisterReflection();
		void OnCreate() override;
		void OnInit() override;
		void OnShutdown() override;
		void OnUpdate(double delta_time) override;
	
		//IPhysicsSceneManager
		void SetActive(bool value) override { m_Active = value; }
		bool GetActive() const override { return m_Active; }

		bool GetSerialize() const override {return true;}
		physx::PxScene* GetPxScene() const {return m_PxScene;}
		PhysXConvexMesh CreateConvexMesh(const std::string &col_mesh_id, MeshComponentPtr mesh);
		PhysXTriangleMesh CreateTriangleMesh(const std::string &col_mesh_id, MeshComponentPtr mesh);
		void Raycast(const Vec3 &ray_start, const Vec3 &ray_dir, GeometryFlags flags, CollisionResult &result, bool return_at_first_hit = false) const;
		void RegisterVehicle(physx::PxVehicleWheels* vehicle);
		void UnregisterVehicle(physx::PxVehicleWheels* vehicle);
		void SetOffset(const Vec3 &offset) {m_Offset = offset;}
		Vec3 GetOffset() const {return m_Offset;}
		Vec3 LocalToWorld(const physx::PxVec3 & local) const; 
		physx::PxVec3 WorldToLocal(const Vec3 & world) const;
		physx::PxControllerManager* GetControllerManager() const {return m_ControllerManager;}
	protected:
		void OnSceneObjectLoaded(PostComponentsInitializedEventPtr message);
		void SetGravity(float gravity);
		float GetGravity() const;
		bool HasConvexMesh(const std::string &name) const;
		bool HasTriangleMesh(const std::string &name) const;
		physx::PxConvexMesh* CreateConvexMesh(const physx::PxVec3* verts, const physx::PxU32 numVerts, physx::PxPhysics& physics, physx::PxCooking& cooking);
		physx::PxTriangleMesh* _CreateTriangleMesh(physx::PxPhysics& physics, physx::PxCooking& cooking, const physx::PxVec3* verts, const physx::PxU32 numVerts,  const physx::PxU32* indices32, physx::PxU32 triCount);
	private:
		float m_Gravity;
		Vec3 m_Offset;
		bool m_Active;
		bool m_Init;
		physx::PxScene *m_PxScene;
		physx::PxDefaultCpuDispatcher* m_CpuDispatcher;
		ConvexMeshMap m_ConvexMeshMap;
		TriangleMeshMap m_TriangleMeshMap;
		std::vector<PhysXBodyComponentPtr> m_Bodies;
		
		//raycast vehicle data
		VehicleSceneQueryData* m_VehicleSceneQueryData;
		VehicleWheelQueryResults* m_WheelQueryResults;
		physx::PxVehicleWheelQueryResult m_VehicleWheelQueryResults[MAX_NUM_WHEELS];
		physx::PxBatchQuery* m_WheelRaycastBatchQuery;
		std::vector<physx::PxVehicleWheels*> m_Vehicles;
		physx::PxControllerManager* m_ControllerManager;
	};
	typedef GASS_SHARED_PTR<PhysXPhysicsSceneManager> PhysXPhysicsSceneManagerPtr;
}
