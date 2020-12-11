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

#include "Plugins/ODE/Collision/ODECollisionSceneManager.h"
#include "Plugins/ODE/Collision/ODECollisionSystem.h"
#include "Plugins/ODE/Collision/ODECollisionGeometryComponent.h"
#include "Plugins/ODE/Collision/ODELineCollision.h"
#include "Core/Utils/GASSException.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSSceneManagerFactory.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSPhysicsMesh.h"
#include "Sim/Interface/GASSIMeshComponent.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Interface/GASSITerrainComponent.h"

namespace GASS
{

	void ODECollisionSceneManager::RegisterReflection()
	{

	}

	ODECollisionSceneManager::ODECollisionSceneManager(SceneWeakPtr scene) : Reflection(scene), 
		m_Space(0) ,
		m_MaxRaySegment(50)
	{

	}

	void ODECollisionSceneManager::OnPostConstruction()
	{
		RegisterForPreUpdate<ODECollisionSystem>();
		GetScene()->RegisterForMessage(REG_TMESS(ODECollisionSceneManager::OnSceneObjectInitialize, PreSceneObjectInitializedEvent, 0));
		m_Space = dHashSpaceCreate(m_Space);
	}

	void ODECollisionSceneManager::OnSceneShutdown()
	{
		GASS_MUTEX_LOCK(m_Mutex)
			dSpaceDestroy(m_Space);
		m_Space = 0;
		//m_RequestMap.clear();
		//m_ResultMap.clear();
	}

	ODECollisionSceneManager::~ODECollisionSceneManager()
	{
		//Fee all meshes in cache
		CollisionMeshMap::iterator iter = m_ColMeshMap.begin();
		while (iter!= m_ColMeshMap.end()) //in map.
		{
			if(iter->second.ID)
				dGeomTriMeshDataDestroy(iter->second.ID);
			++iter;
		}
	}

	void ODECollisionSceneManager::OnSceneCreated()
	{
		
 	}

	void ODECollisionSceneManager::OnSceneObjectInitialize(PreSceneObjectInitializedEventPtr message)
	{
		//auto create collision component
		SceneObjectPtr object = message->GetSceneObject();
		ODECollisionGeometryComponentPtr test_comp = object->GetFirstComponentByClass<ODECollisionGeometryComponent>();
		if(!test_comp) //only add if not already present
		{
			//don't auto add height fields?
			if(object->GetFirstComponentByClass<IHeightmapTerrainComponent>())
			{
				ODECollisionGeometryComponentPtr comp = ODECollisionGeometryComponentPtr(new ODECollisionGeometryComponent());
				comp->SetType(ODECollisionGeometryComponent::CGT_TERRAIN);
				object->AddComponent(comp);
			}
			else if(object->GetFirstComponentByClassName("OgreBillboardComponent",false) || 
				object->GetFirstComponentByClassName("OSGBillboardComponent",false) || 
				object->GetFirstComponentByClassName("GizmoComponent",false))
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

	void ODECollisionSceneManager::Raycast(const Vec3 &ray_start, const Vec3 &ray_dir, GeometryFlags flags, CollisionResult &result, bool return_at_first_hit) const
	{
		GASS_MUTEX_LOCK(m_Mutex)
		ScenePtr scene = GetScene();
		if(scene)
		{
			float seg_l = m_MaxRaySegment;
			if(fabs(ray_dir.x) + fabs(ray_dir.z) < 0.001f) //check if vertical
				seg_l = 0; // skip ray splitting
			ODELineCollision raycast( ray_start, ray_dir, flags, return_at_first_hit, &result,(dGeomID)GetSpace(), seg_l);
			raycast.Process();
		}
	}

	//use cache
	ODECollisionMeshInfo ODECollisionSceneManager::CreateCollisionMeshAndCache(const std::string &cache_id, PhysicsMeshPtr physics_mesh)
	{
		//GASS_MUTEX_LOCK(m_Mutex)
		if(HasCollisionMesh(cache_id)) //check cache
		{
			return m_ColMeshMap[cache_id];
		}
		ODECollisionMeshInfo id = _CreateCollisionMesh(physics_mesh);
		//save to cache
		m_ColMeshMap[cache_id] = id;
		return id;
	}

	ODECollisionMeshInfo ODECollisionSceneManager::_CreateCollisionMesh(PhysicsMeshPtr physics_mesh) const 
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
			static_cast<int>(physics_mesh->PositionVector.size()),
			(unsigned int*)&physics_mesh->IndexVector[0],
			static_cast<int>(physics_mesh->IndexVector.size()),
			3 * sizeof(unsigned int));
		}
		else
		{
			dGeomTriMeshDataBuildSingle(id,
			&(physics_mesh->PositionVector[0]),
			sizeof(Float)*3,
			static_cast<int>(physics_mesh->PositionVector.size()),
			(unsigned int*)&physics_mesh->IndexVector[0],
			static_cast<int>(physics_mesh->IndexVector.size()),
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

	bool ODECollisionSceneManager::GetTerrainHeight(const Vec3& location, double& height, GeometryFlags flags) const
	{
		constexpr double max_terrain_height = 20000;
		CollisionResult result;
		const Vec3 ray_start(location.x, max_terrain_height, location.z);
		const Vec3 ray_direction = Vec3(0, -1, 0) * (max_terrain_height * 2.0);
		Raycast(ray_start, ray_direction, flags, result, true);
		if (result.Coll)
		{
			height = result.CollPosition.y;
		}
		return result.Coll;
	}

	bool ODECollisionSceneManager::GetHeightAboveTerrain(const Vec3& location, double& height, GeometryFlags flags) const
	{
		double terrain_height = 0;
		if (GetTerrainHeight(location, terrain_height, flags))
		{
			double height_above_msl = 0;
			GetHeightAboveSeaLevel(location, height_above_msl);
			height = height_above_msl - terrain_height;
			return true;
		}
		return false;
	}

	bool ODECollisionSceneManager::GetHeightAboveSeaLevel(const Vec3& location, double& height) const
	{
		return location.y;
		return true;
	}

	bool ODECollisionSceneManager::GetUpVector(const Vec3& location, GASS::Vec3& up_vec) const
	{
		up_vec.Set(0, 1, 0);
		return true;
	}

	bool ODECollisionSceneManager::GetOrientation(const Vec3& location, Quaternion& rot) const
	{
		rot = Quaternion::IDENTITY;
		return true;
	}

	bool ODECollisionSceneManager::GetLocationOnTerrain(const Vec3& location, GeometryFlags flags, Vec3& terrain_location) const
	{
		constexpr double max_terrain_height = 20000;
		CollisionResult result;
		const Vec3 ray_start(location.x, max_terrain_height, location.z);
		const Vec3 ray_direction = Vec3(0, -1, 0) * (max_terrain_height * 2.0);
		Raycast(ray_start, ray_direction, flags, result, true);
		if (result.Coll)
		{
			terrain_location = result.CollPosition;
		}
		return result.Coll;
	}


}
