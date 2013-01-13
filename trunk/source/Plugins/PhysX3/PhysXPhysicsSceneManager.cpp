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

#include "Plugins/PhysX3/PhysXPhysicsSystem.h"
#include "Plugins/PhysX3/PhysXPhysicsSceneManager.h"
#include "Plugins/PhysX3/PhysXBodyComponent.h"
#include "Plugins/PhysX3/PhysXVehicleSceneQuery.h"
#include "Plugins/PhysX3/PhysXStream.h"
#include <PxPhysicsAPI.h>



namespace GASS
{

	enum Word3
	{
		SWEPT_INTEGRATION_LINEAR = 1,
	};

	physx::PxFilterFlags SampleVehicleFilterShader(	
		physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0, 
		physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
		physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
	{
		PX_FORCE_PARAMETER_REFERENCE(constantBlock);
		PX_FORCE_PARAMETER_REFERENCE(constantBlockSize);

		// let triggers through
		if(physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
		{
			pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
			return physx::PxFilterFlags();
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
			return physx::PxFilterFlag::eSUPPRESS;

		pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;

		//enable CCD stuff -- for now just for everything or nothing.
		if((filterData0.word3|filterData1.word3) & SWEPT_INTEGRATION_LINEAR)
			pairFlags |= physx::PxPairFlag::eSWEPT_INTEGRATION_LINEAR;

		// The pairFlags for each object are stored in word2 of the filter data. Combine them.
		pairFlags |= physx::PxPairFlags(physx::PxU16(filterData0.word2 | filterData1.word2));
		return physx::PxFilterFlags();
	}


	PhysXPhysicsSceneManager::PhysXPhysicsSceneManager() : m_Paused(false),
		m_Init(false),
		m_Gravity(-9.81f),
		m_CpuDispatcher(NULL),
		m_VehicleSceneQueryData(NULL),
		m_WheelRaycastBatchQuery(NULL)

	{

	}

	PhysXPhysicsSceneManager::~PhysXPhysicsSceneManager()
	{
	}

	void PhysXPhysicsSceneManager::RegisterReflection()
	{
		SceneManagerFactory::GetPtr()->Register("PhysicsSceneManager",new GASS::Creator<PhysXPhysicsSceneManager, ISceneManager>);
		RegisterProperty<float>("Gravity", &GASS::PhysXPhysicsSceneManager::GetGravity, &GASS::PhysXPhysicsSceneManager::SetGravity);
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
		scene->RegisterForMessage(REG_TMESS(PhysXPhysicsSceneManager::OnSceneObjectLoaded, PostComponentsInitializedEvent,0));
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
		m_PxScene = system->GetPxSDK()->createScene(sceneDesc);
		if (!m_PxScene)
			GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"createScene failed!", "PhysXPhysicsSystem::OnLoad");

		//Vehicle related setup
		m_VehicleSceneQueryData = VehicleSceneQueryData::Allocate(MAX_NUM_WHEELS);
		m_WheelRaycastBatchQuery = m_VehicleSceneQueryData->SetUpBatchedSceneQuery(GetPxScene());

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

	void PhysXPhysicsSceneManager::OnSceneObjectLoaded(PostComponentsInitializedEventPtr message)
	{
		SceneObjectPtr obj = message->GetSceneObject();
		assert(obj);

		//Save all bodies
		PhysXBodyComponentPtr body = obj->GetFirstComponentByClass<PhysXBodyComponent>();
		if(body)
			m_Bodies.push_back(body);
	}

	void PhysXPhysicsSceneManager::RegisterVehicle(physx::PxVehicleWheels* vehicle)
	{
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
			iter++;
		}
	}

	void PhysXPhysicsSceneManager::SystemTick(double delta_time)
	{
		if(m_Paused)
			return;

		if(delta_time > 0.1)
			m_PxScene->simulate(0.1);
		else
			m_PxScene->simulate(delta_time);

		while(!m_PxScene->fetchResults())
		{

		}

		//Process vehicles
		if(m_Vehicles.size()  > 0)
		{
			PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<PhysXPhysicsSystem>();
			PxVehicleSuspensionRaycasts(m_WheelRaycastBatchQuery,(int)m_Vehicles.size(),&m_Vehicles[0],m_VehicleSceneQueryData->GetRaycastQueryResultBufferSize(),m_VehicleSceneQueryData->GetRaycastQueryResultBuffer());
			PxVehicleUpdates(delta_time,physx::PxVec3(0, m_Gravity, 0),*system->GetSurfaceTirePairs(),1,&m_Vehicles[0]);
		}

		for(int i = 0 ; i < m_Bodies.size();i++)
		{
			m_Bodies[i]->SendTransformation();
		}


		BaseSceneManager::SystemTick(delta_time);
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

	physx::PxTriangleMesh* PhysXPhysicsSceneManager::CreateTriangleMesh(const physx::PxVec3* verts, const physx::PxU32 numVerts, physx::PxPhysics& physics, physx::PxCooking& cooking)
	{
		PxTriangleMeshDesc desc;
		desc.setToDefault();
		desc.points.count			= numVerts;
		desc.points.stride		= sizeof(physx::PxVec3);
		desc.points.data			= verts;
		
		physx::PxTriangleMesh* triMesh = NULL;
		MemoryOutputStream buf;
		if(cooking.cookTriangleMesh(desc, buf))
		{
			MemoryInputData mid(buf.getData(), buf.getSize());
			triMesh = physics.createTriangleMesh(mid);
		}
		return triMesh;
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
		//not loaded, load it!
		MeshDataPtr mesh_data = new MeshData;
		mesh->GetMeshData(mesh_data);
		int float_size = sizeof(Float);
		if(float_size == 8) //double precision
		{
			std::vector<physx::PxVec3> verts;
			for(int i =0 ;i < mesh_data->NumVertex;i++)
			{
				physx::PxVec3 pos(mesh_data->VertexVector[i].x,mesh_data->VertexVector[i].y,mesh_data->VertexVector[i].z);
				verts.push_back(pos);
			}
			PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<PhysXPhysicsSystem>();
			GASSAssert(system,"PhysXPhysicsSceneManager::CreateConvexMesh");
			m_ConvexMeshMap[col_mesh_id].m_ConvexMesh = CreateConvexMesh(&verts[0], mesh_data->NumVertex, *system->GetPxSDK(), *system->GetPxCooking());
			return m_ConvexMeshMap[col_mesh_id];
		}
	}


	PhysXTriangleMesh PhysXPhysicsSceneManager::CreateTriangleMesh(const std::string &col_mesh_id, MeshComponentPtr mesh)
	{
		if(HasTriangleMesh(col_mesh_id))
		{
			return m_TriangleMeshMap[col_mesh_id];
		}
		//not loaded, load it!
		MeshDataPtr mesh_data = new MeshData;
		mesh->GetMeshData(mesh_data);
		int float_size = sizeof(Float);
		if(float_size == 8) //double precision
		{
			std::vector<physx::PxVec3> verts;
			for(int i =0 ;i < mesh_data->NumVertex;i++)
			{
				physx::PxVec3 pos(mesh_data->VertexVector[i].x,mesh_data->VertexVector[i].y,mesh_data->VertexVector[i].z);
				verts.push_back(pos);
			}
			PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<PhysXPhysicsSystem>();
			GASSAssert(system,"PhysXPhysicsSceneManager::CreateTriangleMesh");
			m_TriangleMeshMap[col_mesh_id].m_TriangleMesh = CreateTriangleMesh(&verts[0], mesh_data->NumVertex, *system->GetPxSDK(), *system->GetPxCooking());
			return m_TriangleMeshMap[col_mesh_id];
		}
	}

	
}
