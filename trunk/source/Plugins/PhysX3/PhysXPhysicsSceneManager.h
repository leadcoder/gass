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
#include "PhysXCommon.h"

#define MAX_NUM_WHEELS 256
namespace physx
{
	class PxScene;
}

namespace GASS
{
	class VehicleSceneQueryData;

	struct PhysXConvexMesh
	{
		physx::PxConvexMesh* m_ConvexMesh;
	};

	struct PhysXTriangleMesh
	{
		physx::PxTriangleMesh* m_TriangleMesh;	
	};

	class PhysXBodyComponent;
	typedef SPTR<PhysXBodyComponent> PhysXBodyComponentPtr;

	class PhysXPhysicsSceneManager  : public Reflection<PhysXPhysicsSceneManager, BaseSceneManager>
	{
	public:
		typedef std::map<std::string,PhysXConvexMesh> ConvexMeshMap;
		typedef std::map<std::string,PhysXTriangleMesh> TriangleMeshMap;
		PhysXPhysicsSceneManager();
		virtual ~PhysXPhysicsSceneManager();
		static void RegisterReflection();
		virtual void OnCreate();
		virtual void OnInit();
		virtual void OnShutdown();
		virtual bool GetSerialize() const {return false;}
		physx::PxScene* GetPxScene() {return m_PxScene;}
		PhysXConvexMesh CreateConvexMesh(const std::string &col_mesh_id, MeshComponentPtr mesh);
		PhysXTriangleMesh CreateTriangleMesh(const std::string &col_mesh_id, MeshComponentPtr mesh);
	
		void RegisterVehicle(physx::PxVehicleWheels* vehicle);
		void UnregisterVehicle(physx::PxVehicleWheels* vehicle);
	protected:
		void SystemTick(double delta);
		void OnSceneObjectLoaded(PostComponentsInitializedEventPtr message);
		void SetGravity(float gravity);
		float GetGravity() const;
		bool HasConvexMesh(const std::string &name) const;
		bool HasTriangleMesh(const std::string &name) const;
		physx::PxConvexMesh* CreateConvexMesh(const physx::PxVec3* verts, const physx::PxU32 numVerts, physx::PxPhysics& physics, physx::PxCooking& cooking);
		physx::PxTriangleMesh* CreateTriangleMesh(const physx::PxVec3* verts, const physx::PxU32 numVerts, physx::PxPhysics& physics, physx::PxCooking& cooking);
	private:
		float m_Gravity;
		bool m_Paused;
		bool m_Init;
		physx::PxScene *m_PxScene;
		physx::PxDefaultCpuDispatcher* m_CpuDispatcher;
		ConvexMeshMap m_ConvexMeshMap;
		TriangleMeshMap m_TriangleMeshMap;
		std::vector<PhysXBodyComponentPtr> m_Bodies;
		
		//raycast vehicle data
		VehicleSceneQueryData* m_VehicleSceneQueryData;
		physx::PxBatchQuery* m_WheelRaycastBatchQuery;
		std::vector<physx::PxVehicleWheels*> m_Vehicles;
	};
	typedef SPTR<PhysXPhysicsSceneManager> PhysXPhysicsSceneManagerPtr;
}
