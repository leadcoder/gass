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

#include "Plugins/PhysX/PhysXPhysicsSystem.h"
#include "Plugins/PhysX/PhysXPhysicsSceneManager.h"
#include "Plugins/PhysX/PhysXVehicleSceneQuery.h"
#include "Plugins/PhysX/PhysXVehicleWheelQueryResults.h"
#include "Plugins/PhysX/PhysXStream.h"
#include <PxPhysicsAPI.h>

#include <memory>

namespace GASS
{

	enum Word3
	{
		SWEPT_INTEGRATION_LINEAR = 1,
	};

	PxFilterFlags SampleVehicleFilterShader(	
		PxFilterObjectAttributes attributes0, PxFilterData filterData0, 
		PxFilterObjectAttributes attributes1, PxFilterData filterData1,
		PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
	{
		PX_UNUSED(constantBlock);
		PX_UNUSED(constantBlockSize);

		// let triggers through
		if(physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
		{
			pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
			return physx::PxFilterFlags();
		}

		// let triggers through
		if(PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
		{
			pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
			return PxFilterFlags();
		}


		// use a group-based mechanism for all other pairs:
		// - Objects within the default group (mask 0) always collide
		// - By default, objects of the default group do not collide
		//   with any other group. If they should collide with another
		//   group then this can only be specified through the filter
		//   data of the default group objects (objects of a different
		//   group can not choose to do so)
		// - For objects that are not in the default group, a bitmask
		//   is used to define the groups they should collide with
		if ((filterData0.word0 != 0 || filterData1.word0 != 0) &&
			!(filterData0.word0&filterData1.word1 || filterData1.word0&filterData0.word1))
			return PxFilterFlag::eSUPPRESS;

		pairFlags = PxPairFlag::eCONTACT_DEFAULT;

		// The pairFlags for each object are stored in word2 of the filter data. Combine them.
		pairFlags |= PxPairFlags(PxU16(filterData0.word2 | filterData1.word2));
		return PxFilterFlags();
	}

	void PhysXPhysicsSceneManager::RegisterReflection()
	{
		SceneManagerFactory::GetPtr()->Register<PhysXPhysicsSceneManager>("PhysXPhysicsSceneManager");
		GetClassRTTI()->SetMetaData(std::make_shared<ClassMetaData>("PhysX Scene Manager", OF_VISIBLE));
		RegisterGetSet("Gravity", &PhysXPhysicsSceneManager::GetGravity, &PhysXPhysicsSceneManager::SetGravity, PF_VISIBLE | PF_EDITABLE, "Gravity");
		RegisterGetSet("Origin", &PhysXPhysicsSceneManager::GetOrigin, &PhysXPhysicsSceneManager::SetOrigin, PF_VISIBLE | PF_EDITABLE, "Local simulation origin");
		RegisterMember("Active", &PhysXPhysicsSceneManager::m_Active, PF_VISIBLE | PF_EDITABLE, "Enable/disable simulation");
	}

	PhysXPhysicsSceneManager::PhysXPhysicsSceneManager(SceneWeakPtr scene) : Reflection(scene),
		m_Active(true),
		m_Init(false),
		m_Gravity(-9.81f),
		m_CpuDispatcher(nullptr),
		m_VehicleSceneQueryData(nullptr),
		m_WheelRaycastBatchQuery(nullptr),
		m_Origin(0,0,0),
		m_UpVector(0,1,0)
	{

	}

	void PhysXPhysicsSceneManager::OnPostConstruction()
	{
		PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<PhysXPhysicsSystem>();
		if (system == nullptr)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to find PhysXPhysicsSystem", "PhysXPhysicsSystem::OnLoad");

		RegisterForPreUpdate<PhysXPhysicsSystem>();

		
		physx::PxSceneDesc scene_desc(system->GetPxSDK()->getTolerancesScale());
	
		scene_desc.gravity = physx::PxVec3(0, m_Gravity, 0);
		
		if (!scene_desc.cpuDispatcher)
		{
			m_CpuDispatcher = physx::PxDefaultCpuDispatcherCreate(3);
			if (!m_CpuDispatcher)
				GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR, "PxDefaultCpuDispatcherCreate failed", "PhysXPhysicsSystem::OnLoad");
			scene_desc.cpuDispatcher = m_CpuDispatcher;
		}
		if (!scene_desc.filterShader)
			scene_desc.filterShader = SampleVehicleFilterShader;//physx::PxDefaultSimulationFilterShader;

															   //sceneDesc.flags	|= PxSceneFlag::eENABLE_SWEPT_INTEGRATION;
		//sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVETRANSFORMS;

		m_PxScene = system->GetPxSDK()->createScene(scene_desc);
		
		if (!m_PxScene)
			GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR, "createScene failed!", "PhysXPhysicsSystem::OnLoad");

		//Vehicle related setup
		m_VehicleSceneQueryData = VehicleSceneQueryData::Allocate(MAX_NUM_WHEELS);
		m_WheelRaycastBatchQuery = m_VehicleSceneQueryData->SetUpBatchedSceneQuery(GetPxScene());

		//Data to store reports for each wheel.
		m_WheelQueryResults = VehicleWheelQueryResults::Allocate(MAX_NUM_WHEELS);

		GASS_LOG(LINFO) << "Create  PxCreateControllerManager...";
		m_ControllerManager = PxCreateControllerManager(*m_PxScene);

		/*physx::PxVisualDebuggerConnectionFlags theConnectionFlags( physx::PxVisualDebuggerExt::getAllConnectionFlags() );
		PVD::PvdConnection* theConnection = physx::PxVisualDebuggerExt::createConnection(system->GetPxSDK()->getPvdConnectionManager(), "127.0.0.1", 5425, 10, theConnectionFlags);
		if (theConnection)
		{
		theConnection->release();
		}*/
		//system->GetPxSDK()->getVisualDebugger()->setVisualizeConstraints(true);
		//system->GetPxSDK()->getVisualDebugger()->setVisualDebuggerFlag(physx::PxVisualDebuggerFlags::eTRANSMIT_CONTACTS, true);


		//m_PxScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE,				 1.0);
		//m_PxScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES,	1.0f);
		//PxMaterial* mMaterial = system->GetPxSDK()->createMaterial(0.5,0.5,0.5);


		/*physx::PxReal d = 0.0f;
		physx::PxTransform pose = physx::PxTransform(physx::PxVec3(0.0f, 0, 0.0f),physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0.0f, 0.0f, 1.0f)));

		physx::PxRigidStatic* plane = system->GetPxSDK()->createRigidStatic(pose);
		if (!plane)
		std::cerr<<"create plane failed!"<<std::endl;


		physx::PxShape* shape = plane->createShape(physx::PxPlaneGeometry(), *system->GetDefaultMaterial());
		if (!shape)
		std::cerr<<"create shape failed!"<<std::endl;
		m_PxScene->addActor(*plane);
		*/
		m_Init = true;
	}

	void PhysXPhysicsSceneManager::OnSceneShutdown()
	{

	}

	PhysXPhysicsSceneManager::~PhysXPhysicsSceneManager()
	{
	}

	void PhysXPhysicsSceneManager::SetGravity(float gravity)
	{
		m_Gravity = gravity;
	}

	float PhysXPhysicsSceneManager::GetGravity() const
	{
		return m_Gravity;
	}

	void PhysXPhysicsSceneManager::SetOrigin(const Vec3& origin)
	{
		m_Origin = origin;
		if (m_PxScene)
		{
			if (GetScene()->GetGeocentric())
			{
				m_UpVector = m_Origin.NormalizedCopy();
				const physx::PxVec3 gravity = PxConvert::ToPx(m_UpVector * m_Gravity);
				m_PxScene->setGravity(gravity);
			}
		}
	}

	Vec3 PhysXPhysicsSceneManager::GetUpVector() const
	{
		return m_UpVector;
	}

	void PhysXPhysicsSceneManager::OnSceneCreated()
	{
		
		
	}

	void PhysXPhysicsSceneManager::OnUpdate(double delta_time)
	{
		if (m_Active)
		{
			//Lock to 60 hz
			//if (delta_time > 1.0 / 60.0)
			//	delta_time = 1.0 / 60.0;

			//Process vehicles
			if (m_Vehicles.size()  > 0)
			{
				PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<PhysXPhysicsSystem>();

				physx::PxVehicleDrivableSurfaceToTireFrictionPairs* surface_tire_pairs = system->GetSurfaceTirePairs();

				
				const physx::PxVec3 gravity = GetScene()->GetGeocentric() ? PxConvert::ToPx(m_Origin.NormalizedCopy() * m_Gravity) : physx::PxVec3(0, static_cast<float>(m_Gravity), 0);

				PxVehicleSuspensionRaycasts(m_WheelRaycastBatchQuery, (int)m_Vehicles.size(), &m_Vehicles[0], m_VehicleSceneQueryData->GetRaycastQueryResultBufferSize(), m_VehicleSceneQueryData->GetRaycastQueryResultBuffer());
				PxVehicleUpdates(static_cast<float>(delta_time), gravity, *surface_tire_pairs, (int)m_Vehicles.size(), &m_Vehicles[0], m_VehicleWheelQueryResults);
			}

			m_PxScene->simulate(static_cast<float>(delta_time));

			while (!m_PxScene->fetchResults())
			{

			}
			GetScene()->PostMessage(std::make_shared<PostPhysicsSceneUpdateEvent>(delta_time));
		}
	}

	Vec3 PhysXPhysicsSceneManager::LocalToWorld(const physx::PxVec3 & local) const
	{
		return PxConvert::ToGASS(local) + m_Origin;
	}

	physx::PxVec3 PhysXPhysicsSceneManager::WorldToLocal(const Vec3 & world) const
	{
		return  PxConvert::ToPx(world - m_Origin);
	}

	void PhysXPhysicsSceneManager::RegisterVehicle(physx::PxVehicleWheels* vehicle)
	{
		size_t index = m_Vehicles.size();
		const PxU32 num_wheels = vehicle->mWheelsSimData.getNbWheels();
		m_VehicleWheelQueryResults[index].nbWheelQueryResults = num_wheels;
		m_VehicleWheelQueryResults[index].wheelQueryResults = m_WheelQueryResults->AddVehicle(num_wheels);
		m_Vehicles.push_back(vehicle);
	}

	void PhysXPhysicsSceneManager::UnregisterVehicle(physx::PxVehicleWheels* vehicle)
	{
		auto iter  = m_Vehicles.begin();
		while(iter != m_Vehicles.end())
		{
			if(*iter == vehicle)
			{
				m_Vehicles.erase(iter);
				return;
			}
			++iter;
		}
	}

	physx::PxConvexMesh* PhysXPhysicsSceneManager::CreateConvexMesh(const physx::PxVec3* verts, const physx::PxU32 numVerts, physx::PxPhysics& physics, physx::PxCooking& cooking)
	{
		// Create descriptor for convex mesh
		physx::PxConvexMeshDesc convex_desc;
		convex_desc.points.count			= numVerts;
		convex_desc.points.stride		= sizeof(physx::PxVec3);
		convex_desc.points.data			= verts;
		convex_desc.flags				= physx::PxConvexFlag::eCOMPUTE_CONVEX;

		physx::PxConvexMesh* convex_mesh = nullptr;
		MemoryOutputStream buf;
		if(cooking.cookConvexMesh(convex_desc, buf))
		{
			MemoryInputData id(buf.GetData(), buf.GetSize());
			convex_mesh = physics.createConvexMesh(id);
		}
		return convex_mesh;
	}

	physx::PxTriangleMesh* PhysXPhysicsSceneManager::CreateTriangleMesh(physx::PxPhysics& physics, physx::PxCooking& cooking, const physx::PxVec3* verts, const physx::PxU32 numVerts,  const physx::PxU32* indices32, physx::PxU32 triCount)
	{
		PxTriangleMeshDesc desc;
		desc.setToDefault();
		desc.points.count			= numVerts;
		desc.points.stride		= sizeof(physx::PxVec3);
		desc.points.data			= verts;

		desc.triangles.count		= triCount;
		desc.triangles.stride		= 3*sizeof(PxU32);
		desc.triangles.data			= indices32;

		physx::PxTriangleMesh* tri_mesh = nullptr;
		MemoryOutputStream buf;
		if(cooking.cookTriangleMesh(desc, buf))
		{
			MemoryInputData mid(buf.GetData(), buf.GetSize());
			tri_mesh = physics.createTriangleMesh(mid);
			return tri_mesh;
		}
		GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"Failed to cook triangles", "PhysXPhysicsSceneManager::CreateTriangleMesh");
	}

	bool PhysXPhysicsSceneManager::HasConvexMesh(const std::string &name) const
	{
		auto iter = m_ConvexMeshMap.find(name);
		return (iter!= m_ConvexMeshMap.end());
	}

	bool PhysXPhysicsSceneManager::HasTriangleMesh(const std::string &name) const
	{
		auto iter = m_TriangleMeshMap.find(name);
		return (iter!= m_TriangleMeshMap.end()); 
	}

	PhysXConvexMesh PhysXPhysicsSceneManager::CreateConvexMesh(const std::string &col_mesh_id, MeshComponentPtr mesh)
	{
		if(HasConvexMesh(col_mesh_id))
		{
			return m_ConvexMeshMap[col_mesh_id];
		}

		GraphicsMesh gfx_mesh_data = mesh->GetMeshData();
		PhysicsMeshPtr physics_mesh(new PhysicsMesh(gfx_mesh_data));

		//int float_size = sizeof(Float);
		//if(float_size == 8) //double precision
		{
			std::vector<physx::PxVec3> verts;
			for(size_t i =0 ;i < physics_mesh->PositionVector.size();i++)
			{
				physx::PxVec3 pos = PxConvert::ToPx(physics_mesh->PositionVector[i]);
				verts.push_back(pos);
			}
			PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<PhysXPhysicsSystem>();
			GASSAssert(system,"PhysXPhysicsSceneManager::CreateConvexMesh");
			m_ConvexMeshMap[col_mesh_id].m_ConvexMesh = CreateConvexMesh(&verts[0], static_cast<physx::PxU32>(physics_mesh->PositionVector.size()), *system->GetPxSDK(), *system->GetPxCooking());
			return m_ConvexMeshMap[col_mesh_id];
		}
		//GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"Size of Float != 8", "PhysXPhysicsSystem::CreateConvexMesh");
	}

	void PhysXPhysicsSceneManager::Raycast(const Vec3 &ray_start, const Vec3 &ray_dir, GeometryFlags /*flags*/, CollisionResult& result, bool /*return_at_first_hit*/) const
	{
		Float ray_length = ray_dir.Length();
		Vec3 norm_ray_dir = ray_dir*(1.0/ray_length);
		/*PxRaycastHit ray_hit;	
		result.Coll = m_PxScene->raycastSingle(PxConvert::ToPx(ray_start + GetOffset()), 
			PxConvert::ToPx(norm_ray_dir), 
			static_cast<float>(ray_length),
			PxSceneQueryFlag::eIMPACT | PxSceneQueryFlag::eNORMAL, 
			ray_hit,
			PxSceneQueryFilterData());

		*/

		PxRaycastBuffer ray_hit;
		result.Coll = m_PxScene->raycast(PxConvert::ToPx(ray_start - GetOrigin()),
			PxConvert::ToPx(norm_ray_dir),
			static_cast<PxReal>(ray_length),
			ray_hit,
			PxSceneQueryFlag::ePOSITION | PxSceneQueryFlag::eNORMAL,
			PxSceneQueryFilterData());

		if(result.Coll)
		{
			result.CollPosition = PxConvert::ToGASS(ray_hit.block.position) + GetOrigin();
			result.CollNormal = PxConvert::ToGASS(ray_hit.block.normal);
			result.CollDist = (ray_start - result.CollPosition).Length();
			if(ray_hit.block.shape && ray_hit.block.shape->userData)
				result.CollSceneObject = ((Component*) ray_hit.block.shape->userData)->GetSceneObject();
		}
	}

	PhysXTriangleMesh PhysXPhysicsSceneManager::CreateTriangleMesh(const std::string &col_mesh_id, MeshComponentPtr mesh)
	{
		if(HasTriangleMesh(col_mesh_id))
		{
			return m_TriangleMeshMap[col_mesh_id];
		}
		//not loaded, load it!
		GraphicsMesh gfx_mesh_data = mesh->GetMeshData();
		PhysicsMeshPtr physics_mesh(new PhysicsMesh(gfx_mesh_data));

		//int float_size = sizeof(Float);
		//if(float_size == 8) //double precision
		{
			std::vector<physx::PxVec3> verts;
			for(size_t i =0 ;i < physics_mesh->PositionVector.size();i++)
			{
				physx::PxVec3 pos= PxConvert::ToPx(physics_mesh->PositionVector[i]);
				verts.push_back(pos);
			}
			PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<PhysXPhysicsSystem>();
			GASSAssert(system,"PhysXPhysicsSceneManager::CreateTriangleMesh");
			m_TriangleMeshMap[col_mesh_id].m_TriangleMesh = CreateTriangleMesh( *system->GetPxSDK(), 
				*system->GetPxCooking(), 
				&verts[0], 
				static_cast<physx::PxU32>(physics_mesh->PositionVector.size()),
				&physics_mesh->IndexVector[0], 
				static_cast<physx::PxU32>(physics_mesh->IndexVector.size()/3));
			return m_TriangleMeshMap[col_mesh_id];
		}
//		GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"Size of Float != 8", "PhysXPhysicsSystem::CreateConvexMesh");
	}
}
