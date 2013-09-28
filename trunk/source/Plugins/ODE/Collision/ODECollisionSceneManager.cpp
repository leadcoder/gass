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

#include "Plugins/ODE/Collision/ODECollisionSceneManager.h"
#include "Plugins/ODE/Collision/ODECollisionSystem.h"
#include "Plugins/ODE/Collision/ODECollisionGeometryComponent.h"
#include "Plugins/ODE/Collision/ODELineCollision.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSException.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSSceneManagerFactory.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Interface/GASSIMeshComponent.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Interface/GASSIMeshComponent.h"
#include "Sim/Interface/GASSITerrainComponent.h"

namespace GASS
{
	ODECollisionSceneManager::ODECollisionSceneManager() : m_Space(0) , m_MaxRaySegment(30)
	{

	}

	ODECollisionSceneManager::~ODECollisionSceneManager()
	{

	}

	void ODECollisionSceneManager::RegisterReflection()
	{
		
	}

	void ODECollisionSceneManager::OnCreate()
	{
		ODECollisionSystemPtr system =  SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<ODECollisionSystem>();
		SystemListenerPtr listener = shared_from_this();
		system->Register(listener);
		GetScene()->RegisterForMessage(REG_TMESS(ODECollisionSceneManager::OnSceneObjectInitialize,PreSceneObjectInitializedEvent,0));
	}

	void ODECollisionSceneManager::OnInit()
	{
		m_Space = dHashSpaceCreate(m_Space);
 	}

	void ODECollisionSceneManager::OnShutdown()
	{
		dSpaceDestroy(m_Space);
		m_Space = 0;
		m_RequestMap.clear();
		m_ResultMap.clear();
 	}

	void ODECollisionSceneManager::OnSceneObjectInitialize(PreSceneObjectInitializedEventPtr message)
	{
		//auto create collision component
		SceneObjectPtr object = message->GetSceneObject();
		ODECollisionGeometryComponentPtr comp = object->GetFirstComponentByClass<ODECollisionGeometryComponent>();
		if(!comp) //only add if not already present
		{
			if(object->GetFirstComponentByClass<IHeightmapTerrainComponent>())
			{
				ODECollisionGeometryComponentPtr comp = ODECollisionGeometryComponentPtr(new ODECollisionGeometryComponent());
				comp->SetType(ODECollisionGeometryComponent::CGT_TERRAIN);
				object->AddComponent(comp);
			}
			else if(object->GetFirstComponentByClass("OgreBillboardComponent",false) || 
				object->GetFirstComponentByClass("OSGBillboardComponent",false) || 
				object->GetFirstComponentByClass("GizmoComponent",false))
			{
				ODECollisionGeometryComponentPtr comp = ODECollisionGeometryComponentPtr(new ODECollisionGeometryComponent());
				comp->SetType(ODECollisionGeometryComponent::CGT_BOX);
				object->AddComponent(comp);
			}
			else if(object->GetFirstComponentByClass<IMeshComponent>())// && !object->GetFirstComponentByClass("OgreManualMeshComponent",false))
			{
				ODECollisionGeometryComponentPtr comp = ODECollisionGeometryComponentPtr(new ODECollisionGeometryComponent());
				comp->SetType(ODECollisionGeometryComponent::CGT_MESH);
				object->AddComponent(comp);
			}
		}
	}

	dSpaceID ODECollisionSceneManager::GetSpace() const 
	{
		return m_Space;
	}

	CollisionHandle ODECollisionSceneManager::Request(const CollisionRequest &request)
	{
		tbb::spin_mutex::scoped_lock lock(m_RequestMutex);
		//assert(request.Scene);
		m_HandleCount = ( m_HandleCount + 1 ) % 0xFFFFFFFE;
		CollisionHandle handle = m_HandleCount;
		m_RequestMap[handle] = request;
		return handle;
	}

	void ODECollisionSceneManager::SystemTick(double delta_time)
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
			ScenePtr scene = GetScene();
			
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
		BaseSceneManager::SystemTick(delta_time);
	}

	bool ODECollisionSceneManager::Check(CollisionHandle handle, CollisionResult &result)
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

	void ODECollisionSceneManager::Force(CollisionRequest &request, CollisionResult &result) const
	{
		ScenePtr scene = GetScene();
		if(scene)
		{
			if(request.Type == COL_LINE)
			{
				ODELineCollision raycast(&request,&result,(dGeomID)GetSpace(),m_MaxRaySegment);
				raycast.Process();
			}
		}
	}

	Float ODECollisionSceneManager::GetHeight(const Vec3 &pos, bool absolute) const
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
		request.ReturnFirstCollisionPoint = false;
		request.CollisionBits = GEOMETRY_FLAG_SCENE_OBJECTS;
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

	//use cache
	ODECollisionMeshInfo ODECollisionSceneManager::CreateCollisionMeshAndCache(const std::string &cache_id, PhysicsMeshPtr physics_mesh)
	{
		if(HasCollisionMesh(cache_id)) //check cache
		{
			return m_ColMeshMap[cache_id];
		}
		ODECollisionMeshInfo id = CreateCollisionMesh(physics_mesh);
		//save to cache
		m_ColMeshMap[cache_id] = id;
		return id;
	}

	ODECollisionMeshInfo ODECollisionSceneManager::CreateCollisionMesh(PhysicsMeshPtr physics_mesh)
	{
		if(physics_mesh->PositionVector.size() < 1 || physics_mesh->IndexVector.size() < 1)
		{
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"No vertex or face data for mesh", "ODECollisionSystem::CreateCollisionMesh");
		}
		// This should equal above code, but without Opcode dependency and no duplicating data
		dTriMeshDataID id = dGeomTriMeshDataCreate();

		
		int float_size = sizeof(Float);
		if(float_size == 8) //double precision
		{
			
			dGeomTriMeshDataBuildDouble(id,
			&(physics_mesh->PositionVector[0]),
			sizeof(Float)*3,
			physics_mesh->PositionVector.size(),
			(unsigned int*)&physics_mesh->IndexVector[0],
			physics_mesh->IndexVector.size(),
			3 * sizeof(unsigned int));
		}
		else
		{
			dGeomTriMeshDataBuildSingle(id,
			&(physics_mesh->PositionVector[0]),
			sizeof(Float)*3,
			physics_mesh->PositionVector.size(),
			(unsigned int*)&physics_mesh->IndexVector[0],
			physics_mesh->IndexVector.size(),
			3 * sizeof(unsigned int));
		}
		//Save id for this collision mesh

		ODECollisionMeshInfo col_mesh;
		col_mesh.ID = id;
		col_mesh.Mesh = physics_mesh;
		return col_mesh;
	}

	bool ODECollisionSceneManager::HasCollisionMesh(const std::string &name)
	{
		CollisionMeshMap::iterator iter;
		iter = m_ColMeshMap.find(name);
		if (iter!= m_ColMeshMap.end()) //in map.
		{
			return true;
		}
		return false;
	}


	ODECollisionMeshInfo ODECollisionSceneManager::GetCollisionMesh(const std::string &name)
	{
		return m_ColMeshMap[name];
	}
}
