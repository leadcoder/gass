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

#include "Plugins/PhysX3/PhysXPhysicsSystem.h"
#include "Plugins/PhysX3/PhysXPhysicsSceneManager.h"
#include "Plugins/PhysX3/PhysXVehicleSceneQuery.h"
#include "Plugins/PhysX3/PhysXVehicleWheelQueryResults.h"
#include "Plugins/PhysX3/PhysXStream.h"
#include <PxPhysicsAPI.h>

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


	PhysXPhysicsSceneManager::PhysXPhysicsSceneManager() : m_Paused(false),
		m_Init(false),
		m_Gravity(-9.81f),
		m_CpuDispatcher(NULL),
		m_VehicleSceneQueryData(NULL),
		m_WheelRaycastBatchQuery(NULL),
		m_Offset(0,0,0)
	{

	}

	PhysXPhysicsSceneManager::~PhysXPhysicsSceneManager()
	{
	}

	void PhysXPhysicsSceneManager::RegisterReflection()
	{
		SceneManagerFactory::GetPtr()->Register("PhysXPhysicsSceneManager",new GASS::Creator<PhysXPhysicsSceneManager, ISceneManager>);
		REG_PROPERTY(float,Gravity,PhysXPhysicsSceneManager);
		REG_PROPERTY(Vec3,Offset,PhysXPhysicsSceneManager);
	}

	void PhysXPhysicsSceneManager::SetGravity(float gravity)
	{
		m_Gravity = gravity;
	}

	float PhysXPhysicsSceneManager::GetGravity() const
	{
		return m_Gravity;
	}

	void PhysXPhysicsSceneManager::OnCreate()
	{
		ScenePtr scene = GetScene();
		GetScene()->RegisterForMessage(REG_TMESS(PhysXPhysicsSceneManager::OnActivateMessage,ActivatePhysicsRequest,0));
	}

	void PhysXPhysicsSceneManager::OnActivateMessage(ActivatePhysicsRequestPtr message)
	{
		m_Paused = !message->GetActivate();
	}

	void PhysXPhysicsSceneManager::OnInit()
	{
		PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<PhysXPhysicsSystem>();
		if(system == NULL)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to find PhysXPhysicsSystem", "PhysXPhysicsSystem::OnLoad");
		SystemListenerPtr listener = shared_from_this();
		system->Register(listener);

		physx::PxSceneDesc sceneDesc(system->GetPxSDK()->getTolerancesScale());
		sceneDesc.gravity = physx::PxVec3(0, m_Gravity, 0);
		if(!sceneDesc.cpuDispatcher) 
		{
			m_CpuDispatcher = physx::PxDefaultCpuDispatcherCreate(3);
			if(!m_CpuDispatcher)
				GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"PxDefaultCpuDispatcherCreate failed", "PhysXPhysicsSystem::OnLoad");
			sceneDesc.cpuDispatcher = m_CpuDispatcher;
		} 
		if(!sceneDesc.filterShader)
			sceneDesc.filterShader  = SampleVehicleFilterShader;//physx::PxDefaultSimulationFilterShader;

		//sceneDesc.flags	|= PxSceneFlag::eENABLE_SWEPT_INTEGRATION;
		sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVETRANSFORMS;

		m_PxScene = system->GetPxSDK()->createScene(sceneDesc);
		if (!m_PxScene)
			GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"createScene failed!", "PhysXPhysicsSystem::OnLoad");

		//Vehicle related setup
		m_VehicleSceneQueryData = VehicleSceneQueryData::allocate(MAX_NUM_WHEELS);
		m_WheelRaycastBatchQuery = m_VehicleSceneQueryData->setUpBatchedSceneQuery(GetPxScene());

		//Data to store reports for each wheel.
		m_WheelQueryResults = VehicleWheelQueryResults::allocate(MAX_NUM_WHEELS);
	
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

	void PhysXPhysicsSceneManager::OnShutdown()
	{

	}

	void PhysXPhysicsSceneManager::OnPreSystemUpdate(double delta_time)
	{
		
	}

	void PhysXPhysicsSceneManager::OnPostSystemUpdate(double delta_time)
	{
		if (!m_Paused)
		{
			//Lock to 60 hz
			if (delta_time > 1.0 / 60.0)
				delta_time = 1.0 / 60.0;

			//Process vehicles
			if (m_Vehicles.size()  > 0)
			{


				PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<PhysXPhysicsSystem>();

				physx::PxVehicleDrivableSurfaceToTireFrictionPairs* surfaceTirePairs = system->GetSurfaceTirePairs();

				PxVehicleSuspensionRaycasts(m_WheelRaycastBatchQuery, (int)m_Vehicles.size(), &m_Vehicles[0], m_VehicleSceneQueryData->getRaycastQueryResultBufferSize(), m_VehicleSceneQueryData->getRaycastQueryResultBuffer());
				PxVehicleUpdates(static_cast<float>(delta_time), physx::PxVec3(0, static_cast<float>(m_Gravity), 0), *surfaceTirePairs, (int)m_Vehicles.size(), &m_Vehicles[0], m_VehicleWheelQueryResults);
			}

			m_PxScene->simulate(static_cast<float>(delta_time));

			while (!m_PxScene->fetchResults())
			{

			}
			GetScene()->PostMessage(PostPhysicsSceneUpdateEventPtr(new PostPhysicsSceneUpdateEvent(delta_time)));
		}
		//update tick subscribers
		BaseSceneManager::_UpdateListeners(delta_time);
	}

	void PhysXPhysicsSceneManager::OnSceneObjectLoaded(PostComponentsInitializedEventPtr message)
	{

	}

	void PhysXPhysicsSceneManager::RegisterVehicle(physx::PxVehicleWheels* vehicle)
	{
		size_t index = m_Vehicles.size();
		const PxU32 numWheels = vehicle->mWheelsSimData.getNbWheels();
		m_VehicleWheelQueryResults[index].nbWheelQueryResults = numWheels;
		m_VehicleWheelQueryResults[index].wheelQueryResults = m_WheelQueryResults->addVehicle(numWheels);
		m_Vehicles.push_back(vehicle);
	}

	void PhysXPhysicsSceneManager::UnregisterVehicle(physx::PxVehicleWheels* vehicle)
	{
		std::vector<physx::PxVehicleWheels*>::iterator iter  = m_Vehicles.begin();
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
		physx::PxConvexMeshDesc convexDesc;
		convexDesc.points.count			= numVerts;
		convexDesc.points.stride		= sizeof(physx::PxVec3);
		convexDesc.points.data			= verts;
		convexDesc.flags				= physx::PxConvexFlag::eCOMPUTE_CONVEX | physx::PxConvexFlag::eINFLATE_CONVEX;

		physx::PxConvexMesh* convexMesh = NULL;
		MemoryOutputStream buf;
		if(cooking.cookConvexMesh(convexDesc, buf))
		{
			MemoryInputData id(buf.getData(), buf.getSize());
			convexMesh = physics.createConvexMesh(id);
		}
		return convexMesh;
	}

	physx::PxTriangleMesh* PhysXPhysicsSceneManager::_CreateTriangleMesh(physx::PxPhysics& physics, physx::PxCooking& cooking, const physx::PxVec3* verts, const physx::PxU32 numVerts,  const physx::PxU32* indices32, physx::PxU32 triCount)
	{
		PxTriangleMeshDesc desc;
		desc.setToDefault();
		desc.points.count			= numVerts;
		desc.points.stride		= sizeof(physx::PxVec3);
		desc.points.data			= verts;

		desc.triangles.count		= triCount;
		desc.triangles.stride		= 3*sizeof(PxU32);
		desc.triangles.data			= indices32;

		physx::PxTriangleMesh* triMesh = NULL;
		MemoryOutputStream buf;
		if(cooking.cookTriangleMesh(desc, buf))
		{
			MemoryInputData mid(buf.getData(), buf.getSize());
			triMesh = physics.createTriangleMesh(mid);
			return triMesh;
		}
		GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"Failed to cook triangles", "PhysXPhysicsSceneManager::CreateTriangleMesh");
	}

	bool PhysXPhysicsSceneManager::HasConvexMesh(const std::string &name) const
	{
		ConvexMeshMap::const_iterator iter = m_ConvexMeshMap.find(name);
		return (iter!= m_ConvexMeshMap.end());
	}

	bool PhysXPhysicsSceneManager::HasTriangleMesh(const std::string &name) const
	{
		TriangleMeshMap::const_iterator iter = m_TriangleMeshMap.find(name);
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
			for(int i =0 ;i < physics_mesh->PositionVector.size();i++)
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

	void PhysXPhysicsSceneManager::Raycast(const Vec3 &ray_start, const Vec3 &ray_dir, GeometryFlags flags, CollisionResult &result, bool return_at_first_hit) const
	{
		Float ray_length = ray_dir.Length();
		Vec3 norm_ray_dir = ray_dir*(1.0/ray_length);
		PxRaycastHit ray_hit;	
		result.Coll = m_PxScene->raycastSingle(PxConvert::ToPx(ray_start + GetOffset()), 
			PxConvert::ToPx(norm_ray_dir), 
			static_cast<float>(ray_length),
			PxSceneQueryFlag::eIMPACT | PxSceneQueryFlag::eNORMAL, 
			ray_hit,
			PxSceneQueryFilterData());
		if(result.Coll)
		{
			result.CollPosition = PxConvert::ToGASS(ray_hit.position) - GetOffset();
			result.CollNormal = PxConvert::ToGASS(ray_hit.normal);
			result.CollDist = (ray_start - result.CollPosition).Length();
			if(ray_hit.shape && ray_hit.shape->userData)
				result.CollSceneObject = ((BaseSceneComponent*) ray_hit.shape->userData)->GetSceneObject();
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
			for(int i =0 ;i < physics_mesh->PositionVector.size();i++)
			{
				physx::PxVec3 pos= PxConvert::ToPx(physics_mesh->PositionVector[i]);
				verts.push_back(pos);
			}
			PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<PhysXPhysicsSystem>();
			GASSAssert(system,"PhysXPhysicsSceneManager::CreateTriangleMesh");
			m_TriangleMeshMap[col_mesh_id].m_TriangleMesh = _CreateTriangleMesh( *system->GetPxSDK(), 
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
