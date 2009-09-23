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

#include "Plugins/PhysX/PhysXPhysicsSystem.h"
#include "Plugins/PhysX/PhysXPhysicsSceneManager.h"
//#include "Plugins/PhysX/PhysXBody.h"
//#include "Plugins/PhysX/PhysXCollisionSystem.h"

#include <boost/bind.hpp>


#include "Core/Utils/Log.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/Message.h"
#include "Sim/Scenario/Scene/SceneManagerFactory.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"

#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/SimEngine.h"
#include "Sim/Scheduling/IRuntimeController.h"

#include "Sim/Components/Graphics/Geometry/IMeshComponent.h"
#include "Sim/Systems/SimSystemManager.h"


namespace GASS
{

	PhysXPhysicsSceneManager::PhysXPhysicsSceneManager() :
		m_Paused(false),
		m_TaskGroup(PHYSICS_TASK_GROUP),
		m_Gravity(-9.81f),
		m_SimulationUpdateInterval(1.0/60.0), //Locked to 60hz, if this value is changed the behavior of simulation is effected and values for bodies and joints must be retweeked
		m_TimeToProcess(0),
		m_MaxSimSteps(4)
	{

	}

	PhysXPhysicsSceneManager::~PhysXPhysicsSceneManager()
	{
	}

	void PhysXPhysicsSceneManager::RegisterReflection()
	{
		SceneManagerFactory::GetPtr()->Register("PhysXPhysicsSceneManager",new GASS::Creator<PhysXPhysicsSceneManager, ISceneManager>);
		RegisterProperty<float>("Gravity", &GASS::PhysXPhysicsSceneManager::GetGravity, &GASS::PhysXPhysicsSceneManager::SetGravity);
		RegisterProperty<TaskGroup>("TaskGroup", &GASS::PhysXPhysicsSceneManager::GetTaskGroup, &GASS::PhysXPhysicsSceneManager::SetTaskGroup);
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
		SimEngine::GetPtr()->GetRuntimeController()->Register(this);
		m_Scene->RegisterForMessage(ScenarioScene::SCENARIO_RM_LOAD_SCENE_MANAGERS, MESSAGE_FUNC( PhysXPhysicsSceneManager::OnLoad ));
		m_Scene->RegisterForMessage(ScenarioScene::SCENARIO_RM_UNLOAD_SCENE_MANAGERS, MESSAGE_FUNC( PhysXPhysicsSceneManager::OnUnload ));
		m_Scene->RegisterForMessage(ScenarioScene::SCENARIO_NM_SCENE_OBJECT_CREATED, MESSAGE_FUNC( PhysXPhysicsSceneManager::OnLoadSceneObject),ScenarioScene::PHYSICS_COMPONENT_LOAD_PRIORITY);
	}

	void PhysXPhysicsSceneManager::OnLoadSceneObject(MessagePtr message)
	{
		//Initlize all physics components and send scene mananger as argument
		SceneObjectPtr obj = boost::any_cast<SceneObjectPtr>(message->GetData("SceneObject"));
		assert(obj);
		MessagePtr phy_msg(new Message(SceneObject::OBJECT_RM_LOAD_PHYSICS_COMPONENTS,(int) this));
		phy_msg->SetData("PhysicsSceneManager",boost::any(this));
		obj->SendImmediate(phy_msg);
	}


	void PhysXPhysicsSceneManager::Update(double delta_time)
	{
		
		if (m_Paused)
			return;

		//do some time slicing
		m_TimeToProcess += delta_time;
		int num_steps = (int) (m_TimeToProcess / m_SimulationUpdateInterval);
		int clamp_num_steps = num_steps;

		//Take max 4 simulation step each frame
		if(num_steps > m_MaxSimSteps) clamp_num_steps = m_MaxSimSteps;

		for (int i = 0; i < clamp_num_steps; ++i)
		{
		
		}
		//std::cout << "Steps:" <<  clamp_num_steps << std::endl;
		m_TimeToProcess -= m_SimulationUpdateInterval * num_steps;

		//Temp: move this to ODEPhysicsSystem
		
		/*ODECollisionSystem* col_sys = dynamic_cast<ODECollisionSystem*>(SimEngine::GetPtr()->GetSystemManager()->GetFirstSystem<GASS::ICollisionSystem>().get());
		if(col_sys)
		{
			col_sys->Process();

		}*/
	}


	void PhysXPhysicsSceneManager::OnLoad(MessagePtr message)
	{
		ScenarioScene* scene = boost::any_cast<ScenarioScene*>(message->GetData("ScenarioScene"));
		Vec3 gravity_vec = scene->GetSceneUp()*m_Gravity;

		PhysXPhysicsSystemPtr system = SimEngine::Get().GetSystemManager()->GetFirstSystem<PhysXPhysicsSystem>();
		NxSceneDesc sceneDesc;
		sceneDesc.gravity = NxVec3(gravity_vec.x, gravity_vec.y, gravity_vec.z);
		m_NxScene = system->GetNxSDK()->createScene(sceneDesc);
		if(m_NxScene == NULL) 
		{
			Log::Error("\nError: Unable to create a PhysX scene, exiting the sample.\n\n");
			return ;
		}
		//Set default material
		NxMaterial* defaultMaterial = m_NxScene->getMaterialFromIndex(0);
		defaultMaterial->setRestitution(0.0f);
		defaultMaterial->setStaticFriction(0.5f);
		defaultMaterial->setDynamicFriction(0.5f);
		m_Init = true;
	}

	void PhysXPhysicsSceneManager::OnUnload(MessagePtr message)
	{
		SimEngine::GetPtr()->GetRuntimeController()->Unregister(this);
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


	void PhysXPhysicsSceneManager::SetTaskGroup(TaskGroup value)
	{
		m_TaskGroup = value;
	}

	TaskGroup PhysXPhysicsSceneManager::GetTaskGroup() const
	{
		return m_TaskGroup;
	}
}
