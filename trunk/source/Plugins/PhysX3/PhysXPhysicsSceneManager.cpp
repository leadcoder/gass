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

namespace GASS
{
	PhysXPhysicsSceneManager::PhysXPhysicsSceneManager() :
		m_Paused(false),
		m_Init(false),
		m_Gravity(-9.81f),
		m_CpuDispatcher(NULL)
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
		if(scene)
		{
			scene->RegisterForMessage(REG_TMESS(PhysXPhysicsSceneManager::OnLoad ,LoadSceneManagersMessage,0));
			scene->RegisterForMessage(REG_TMESS(PhysXPhysicsSceneManager::OnUnload ,UnloadSceneManagersMessage,0));
			scene->RegisterForMessage(REG_TMESS(PhysXPhysicsSceneManager::OnLoadSceneObject, SceneObjectCreatedNotifyMessage ,Scene::PHYSICS_COMPONENT_LOAD_PRIORITY));
		}
	}
	
	void PhysXPhysicsSceneManager::OnLoadSceneObject(SceneObjectCreatedNotifyMessagePtr message)
	{
		//Initlize all physics components and send scene mananger as argument
		SceneObjectPtr obj = message->GetSceneObject();
		assert(obj);
		
		//Save all bodies
		PhysXBodyComponentPtr body = obj->GetFirstComponentByClass<PhysXBodyComponent>();
		if(body)
			m_Bodies.push_back(body);
	}

	void PhysXPhysicsSceneManager::SystemTick(double delta_time)
	{
		if(m_Paused)
			return;

		
		m_PxScene->simulate(delta_time);
		while(!m_PxScene->fetchResults())
		{

		}
		
		for(int i = 0 ; i < m_Bodies.size();i++)
		{
			m_Bodies[i]->SendTransformation();
		}
	}

/*	static physx::PxFilterFlags filter(physx::PxFilterObjectAttributes attributes0,
							physx::PxFilterData filterData0, 
							physx::PxFilterObjectAttributes attributes1,
							physx::PxFilterData filterData1,
							physx::PxPairFlags& pairFlags,
							const void* constantBlock,
							physx::PxU32 constantBlockSize)
{
	pairFlags |= physx::PxPairFlag::eCONTACT_DEFAULT;
	return physx::PxFilterFlags();
}
*/


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
		//if((filterData0.word3|filterData1.word3) & SWEPT_INTEGRATION_LINEAR)
		//	pairFlags |= physx::PxPairFlag::eSWEPT_INTEGRATION_LINEAR;

		// The pairFlags for each object are stored in word2 of the filter data. Combine them.
		pairFlags |= physx::PxPairFlags(physx::PxU16(filterData0.word2 | filterData1.word2));
		return physx::PxFilterFlags();
	}

	void PhysXPhysicsSceneManager::OnLoad(LoadSceneManagersMessagePtr message)
	{
		PhysXPhysicsSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<PhysXPhysicsSystem>();
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

	void PhysXPhysicsSceneManager::OnUnload(UnloadSceneManagersMessagePtr message)
	{
	}

	//TODO: move this to physics system instead of scenatio manager, mesh data should be same between scenes and therefore shareable
/*	ODECollisionMesh PhysXPhysicsSceneManager::CreateCollisionMesh(IMeshComponent* mesh)
	{
		std::string col_mesh_name = mesh->GetFilename();
		if(HasCollisionMesh(col_mesh_name))
		{
			return m_ColMeshMap[col_mesh_name];
		}
		//not loaded, load it!

		MeshDataPtr mesh_data = new MeshData;
		mesh->GetMeshData(mesh_data);

		if(mesh_data->NumVertex < 1 || mesh_data->NumFaces < 1)
		{
			//Log::Error("No verticies found for this mesh")
		}
		// This should equal above code, but without Opcode dependency and no duplicating data
		dTriMeshDataID id = dGeomTriMeshDataCreate();
		dGeomTriMeshDataBuildSingle(id,
			&(mesh_data->VertexVector[0]),
			sizeof(float)*3,
			mesh_data->NumVertex,
			(unsigned int*)&mesh_data->FaceVector[0],
			mesh_data->NumFaces*3,
			3 * sizeof(unsigned int));
		//Save id for this collision mesh
		ODECollisionMesh col_mesh;
		col_mesh.ID = id;
		col_mesh.Mesh = mesh_data;
		m_ColMeshMap[col_mesh_name] = col_mesh;
		return col_mesh;
	}

	PhysXCollisionMesh PhysXPhysicsSceneManager::CreateCollisionMesh(IMeshComponent* mesh)
	{
		std::string col_mesh_name = mesh->GetFilename();
		if(HasCollisionMesh(col_mesh_name))
		{
			return m_ColMeshMap[col_mesh_name];
		}
		//not loaded, load it!

		MeshDataPtr mesh_data = new MeshData;
		mesh->GetMeshData(mesh_data);

		unsigned int mVertexCount = mesh_data->NumVertex, mIndexCount  = mesh_data->NumFaces;
		NxVec3* mMeshVertices = new NxVec3[mVertexCount];
		NxU32* mMeshFaces = new NxU32[mIndexCount];
		NxMaterialIndex* mMaterials = new NxMaterialIndex[mIndexCount];

		NxMaterialIndex currentMaterialIndex = 0;
//		bool use32bitindexes;

		for(unsigned int  i = 0;  i < mesh->NumVertex;i++)
		{
			Vec3 pos = mesh->VertexVector[i];
			mMeshVertices[i] = NxVec3(pos.x,pos.y,pos.z); 
		}

		for(unsigned int  i = 0;  i < mesh->NumFaces;i++)
		{
			mMaterials[i] = mesh->MatIDVector[i];
			mMeshFaces[i] = mesh->FaceVector[i]; 
		}

		NxTriangleMeshDesc mTriangleMeshDescription;

		// Vertices
		mTriangleMeshDescription.numVertices				= mVertexCount;
		mTriangleMeshDescription.points						= mMeshVertices;							
		mTriangleMeshDescription.pointStrideBytes			= sizeof(NxVec3);
		// Triangles
		mTriangleMeshDescription.numTriangles				= mIndexCount / 3;
		mTriangleMeshDescription.triangles					= mMeshFaces;
		mTriangleMeshDescription.triangleStrideBytes		= 3 * sizeof(NxU32);
		// Materials
		//#if 0
		mTriangleMeshDescription.materialIndexStride		= sizeof(NxMaterialIndex);

		mTriangleMeshDescription.materialIndices			= mMaterials;
		//#endif
		//mTriangleMeshDescription.flags					= NX_MF_HARDWARE_MESH;

		NxTriangleMesh* trimesh;

#ifndef NX_DEBUG

		MemoryWriteBuffer buf;
		if (!NxCookTriangleMesh(mTriangleMeshDescription, buf)) {
			std::stringstream s;
			s	<< "Mesh '" << "' failed to cook"
				<< "V(" << mMeshVertices << ") F(" << mMeshFaces << ")";

			Log::Error("%s",s.str());//NxThrow_Error(s.str());
		}
		trimesh = m_Scene->getPhysicsSDK().createTriangleMesh(MemoryReadBuffer(buf.data));

#else

		NxString filename;
		if (Ogre::StringUtil::endsWith(meshName, ".mesh")) {
			filename = meshName.substr(0, meshName.length() - 5) + ".TriangleMeshShape.nxs";	
		}
		else {
			filename = meshName + ".TriangleMeshShape.nxs";
		}

		UserStream buf(filename.c_str(),false);

		if (!NxCookTriangleMesh(mTriangleMeshDescription, buf)) {
			std::stringstream s;
			s	<< "Mesh '" << meshName << "' failed to cook"
				<< "V(" << mMeshVertices << ") F(" << mMeshFaces << ")";

			NxThrow_Error(s.str());
		}

		fclose(buf.fp);

		UserStream rbuf(filename.c_str(), true);

		trimesh = scene->getPhysicsSDK().createTriangleMesh(rbuf);

		fclose(rbuf.fp);



#endif

	

//		delete []vertices;
//		delete []indices;

		delete []mMeshVertices;
		delete []mMeshFaces;
		delete []mMaterials;

		PhysXCollisionMesh col_mesh;
		col_mesh.NxMesh = trimesh;
		col_mesh.Mesh = mesh_data;
		m_ColMeshMap[col_mesh_name] = col_mesh;
		return col_mesh;

//		return trimesh;

	}

	bool PhysXPhysicsSceneManager::HasCollisionMesh(const std::string &name)
	{
		CollisionMeshMap::iterator iter;
		iter = m_ColMeshMap.find(name);
		if (iter!= m_ColMeshMap.end()) //in map.
		{
			return true;
		}
		return false;
	}*/

}
