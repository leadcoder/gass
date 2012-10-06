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

#include "Plugins/ODE/ODECollisionSystem.h"
#include "Plugins/ODE/ODEPhysicsSceneManager.h"
#include "Plugins/ODE/ODELineCollision.h"
#include "Plugins/ODE/ODECollisionGeometryComponent.h"
#include "Core/System/GASSSystemFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSException.h"
#include "Sim/Scene/GASSScene.h"

#include "Sim/Systems/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Components/Graphics/Geometry/GASSIMeshComponent.h"
#include "Sim/Components/Graphics/Geometry/GASSIGeometryComponent.h"
#include "Sim/Components/Graphics/Geometry/GASSIMeshComponent.h"
#include "Sim/Components/Graphics/Geometry/GASSITerrainComponent.h"


namespace GASS
{
	ODECollisionSystem::ODECollisionSystem() : m_MaxRaySegment(300),m_Space(0)
	{
		m_HandleCount = 5;
	}

	ODECollisionSystem::~ODECollisionSystem()
	{

	}


	void ODECollisionSystem::OnCreate()
	{
		int address = (int) this;
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(ODECollisionSystem::OnSceneUnloaded,SceneUnloadNotifyMessage,0));
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(ODECollisionSystem::OnSceneLoaded,SceneLoadedNotifyMessage,0));
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(ODECollisionSystem::OnSceneAboutToLoad,SceneAboutToLoadNotifyMessage,0));
		//m_Owner->GetMessageManager()->RegisterForMessage(SystemManager::SYSTEM_RM_UPDATE, address,  boost::bind( &ODECollisionSystem::OnUpdate, this, _1 ),0);
	}

	void ODECollisionSystem::OnSceneAboutToLoad(SceneAboutToLoadNotifyMessagePtr message)
	{
		m_Space = dHashSpaceCreate(m_Space);
		m_Scene = message->GetScene();
		message->GetScene()->RegisterForMessage(REG_TMESS(ODECollisionSystem::OnSceneObjectInitialize,PreSceneObjectInitialized,0));
	}

	void ODECollisionSystem::OnSceneLoaded(SceneLoadedNotifyMessagePtr message)
	{
		//ODEPhysicsSceneManagerPtr ode_scene = message->GetScene()->GetFirstSceneManagerByClass<ODEPhysicsSceneManager>();
		//if(ode_scene)
		//	m_Space = ode_scene->GetPhysicsSpace();
	}

	void ODECollisionSystem::OnSceneObjectInitialize(PreSceneObjectInitializedPtr message)
	{
		//auto create collision component
		SceneObjectPtr object = message->GetSceneObject();
		ODECollisionGeometryComponentPtr comp = object->GetFirstComponentByClass<ODECollisionGeometryComponent>();
		if(!comp) //add if not present
		{
			if(object->GetFirstComponentByClass<ITerrainComponent>())
			{
				ODECollisionGeometryComponentPtr comp = ODECollisionGeometryComponentPtr(new ODECollisionGeometryComponent());
				comp->SetType(ODECollisionGeometryComponent::CGT_TERRAIN);
				//comp->SetMaterialFlag(MATERIAL_FLAG_UNKONWN_GROUND);
				object->AddComponent(comp);
			}

			else if(object->GetFirstComponentByClass<IMeshComponent>())
			{
				ODECollisionGeometryComponentPtr comp = ODECollisionGeometryComponentPtr(new ODECollisionGeometryComponent());
				comp->SetType(ODECollisionGeometryComponent::CGT_MESH);
				object->AddComponent(comp);
			}

			else if(object->GetFirstComponentByClass("OgreBillboardComponent",false) || object->GetFirstComponentByClass("GizmoComponent",false))
			{
				ODECollisionGeometryComponentPtr comp = ODECollisionGeometryComponentPtr(new ODECollisionGeometryComponent());
				comp->SetType(ODECollisionGeometryComponent::CGT_BOX);
				object->AddComponent(comp);
			}
		}
	}

	dSpaceID ODECollisionSystem::GetSpace() const 
	{
		return m_Space;
	}

	void ODECollisionSystem::OnSceneUnloaded(SceneUnloadNotifyMessagePtr message)
	{
		dSpaceDestroy(m_Space);
		m_RequestMap.clear();
		m_ResultMap.clear();
	}
	
	CollisionHandle ODECollisionSystem::Request(const CollisionRequest &request)
	{
		tbb::spin_mutex::scoped_lock lock(m_RequestMutex);
		//assert(request.Scene);
		m_HandleCount = ( m_HandleCount + 1 ) % 0xFFFFFFFE;
		CollisionHandle handle = m_HandleCount;
		m_RequestMap[handle] = request;
		return handle;
	}

	void ODECollisionSystem::Update(double delta_time)
	{
		RequestMap::iterator iter;
		RequestMap requestMap;
		ResultMap resultMap;
		ResultMap::iterator res_iter;
		{
			tbb::spin_mutex::scoped_lock lock(m_RequestMutex);
			requestMap = m_RequestMap;
			m_RequestMap.clear();
		}
		/*{
			tbb::spin_mutex::scoped_lock lock(m_ResultMutex);
			resultMap = m_ResultMap;
		}*/
		//std::cout << "ResultMap:" << m_ResultMap.size() << "\n";
		//std::cout << "RequestMap:" << requestMap.size() << "\n";

		for(iter = requestMap.begin(); iter != requestMap.end(); ++iter)
		{
			CollisionRequest request =  iter->second;
			CollisionHandle handle = iter->first;
			ScenePtr scene = ScenePtr(request.Scene);
			
			if(scene)
			{
			
				if(request.Type == COL_LINE)
				{
					CollisionResult result;
					ODELineCollision raycast(&request,&result,(dGeomID)GetSpace(),m_MaxRaySegment);
					raycast.Process();
					resultMap[handle] = result;
				}
			}
			/*else(request.Type == COL_SPHERE)
			{

			}*/
		}
		{
			tbb::spin_mutex::scoped_lock lock(m_ResultMutex);
			//transfer results
			for(res_iter = resultMap.begin(); res_iter != resultMap.end(); ++res_iter)
			{
				CollisionHandle handle = res_iter->first;
				m_ResultMap[handle] = res_iter->second;
			}
		}
		//update listeners
		SimSystem::Update(delta_time);
	}



	bool ODECollisionSystem::Check(CollisionHandle handle, CollisionResult &result)
	{
		tbb::spin_mutex::scoped_lock lock(m_ResultMutex);
		ResultMap::iterator iter = m_ResultMap.find(handle);
		if(iter != m_ResultMap.end())
		{
			result = (*iter).second;
			m_ResultMap.erase(iter);
			return true;
		}
		return false;
	}

	void ODECollisionSystem::Force(CollisionRequest &request, CollisionResult &result) const
	{
		ScenePtr scene(request.Scene);
		if(scene)
		{
			if(request.Type == COL_LINE)
			{
				ODELineCollision raycast(&request,&result,(dGeomID)GetSpace(),m_MaxRaySegment);
				raycast.Process();
			}
		}
	}

	void ODECollisionSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("ODECollisionSystem",new GASS::Creator<ODECollisionSystem, ISystem>);
	}

	

	Float ODECollisionSystem::GetHeight(ScenePtr scene, const Vec3 &pos, bool absolute) const
	{
		CollisionRequest request;
		CollisionResult result;

		Vec3 up(0,1,0);

		Vec3 ray_start = pos;
		Vec3 ray_direction = -up;
		//max raycast 2000000 units down
		ray_direction = ray_direction*2000000;

		request.LineStart = ray_start;
		request.LineEnd = ray_start + ray_direction;
		request.Type = COL_LINE;
		request.Scene = scene;
		request.ReturnFirstCollisionPoint = false;
		request.CollisionBits = 2;
		ODELineCollision raycast(&request,&result,(dGeomID)GetSpace());
		raycast.Process();

		if(result.Coll)
		{
			Vec3 col_pos;
			if(absolute)
				col_pos  = result.CollPosition;
			else
				col_pos = pos - result.CollPosition;

			col_pos = col_pos *up;
			return col_pos.Length();
		}
		return 0;
	}

	ODECollisionMeshInfo ODECollisionSystem::CreateCollisionMesh(MeshComponentPtr mesh)
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
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"No vertex or face data for mesh", "ODECollisionSystem::CreateCollisionMesh");
		}
		// This should equal above code, but without Opcode dependency and no duplicating data
		dTriMeshDataID id = dGeomTriMeshDataCreate();

		
		int float_size = sizeof(Float);
		if(float_size == 8) //double precision
		{
			
			dGeomTriMeshDataBuildDouble(id,
			&(mesh_data->VertexVector[0]),
			sizeof(Float)*3,
			mesh_data->NumVertex,
			(unsigned int*)&mesh_data->FaceVector[0],
			mesh_data->NumFaces*3,
			3 * sizeof(unsigned int));
		}
		else
		{
			dGeomTriMeshDataBuildSingle(id,
			&(mesh_data->VertexVector[0]),
			sizeof(Float)*3,
			mesh_data->NumVertex,
			(unsigned int*)&mesh_data->FaceVector[0],
			mesh_data->NumFaces*3,
			3 * sizeof(unsigned int));
		}
		//Save id for this collision mesh

		ODECollisionMeshInfo col_mesh;
		col_mesh.ID = id;
		col_mesh.Mesh = mesh_data;
		m_ColMeshMap[col_mesh_name] = col_mesh;
		return col_mesh;
	}

	bool ODECollisionSystem::HasCollisionMesh(const std::string &name)
	{
		CollisionMeshMap::iterator iter;
		iter = m_ColMeshMap.find(name);
		if (iter!= m_ColMeshMap.end()) //in map.
		{
			return true;
		}
		return false;
	}
}
