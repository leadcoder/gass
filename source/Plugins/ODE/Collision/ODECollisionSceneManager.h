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

#ifndef ODE_SCENE_MANAGER_H
#define ODE_SCENE_MANAGER_H

#include "Sim/GASSCommon.h"
#include <ode/ode.h>
#include "Core/MessageSystem/GASSBaseMessage.h"
#include "Sim/GASSBaseSceneManager.h"
#include "Sim/Interface/GASSICollisionSceneManager.h"
#include "Sim/Messages/GASSCoreSceneMessages.h"
#include "Sim/GASSThreading.h"
#include "ODECollisionInfo.h"

namespace GASS
{
	class IMeshComponent;
	typedef GASS_SHARED_PTR<IMeshComponent> MeshComponentPtr;
	class PhysicsMesh;
	typedef GASS_SHARED_PTR<PhysicsMesh> PhysicsMeshPtr;

	/**
		Collision Scene Manager
	*/
	class ODECollisionSceneManager : public Reflection<ODECollisionSceneManager, BaseSceneManager> , public ICollisionSceneManager
	{
		friend class ODECollisionGeometryComponent;
	public:
		ODECollisionSceneManager(SceneWeakPtr scene);
		~ODECollisionSceneManager() override;
		static void RegisterReflection();
		void OnPostConstruction() override;
		void OnSceneCreated() override;
		void OnSceneShutdown() override;
		bool GetSerialize() const override {return false;}
		void Raycast(const Vec3 &ray_start, const Vec3 &ray_dir, GeometryFlags flags, CollisionResult &result, bool return_first_hit = false) const override;
		bool GetTerrainHeight(const Vec3& location, double& height, GeometryFlags flags) const override;
		bool GetHeightAboveTerrain(const Vec3& location, double& height, GeometryFlags flags) const override;
		bool GetHeightAboveSeaLevel(const Vec3& location, double& height) const override;
		bool GetUpVector(const Vec3& location, GASS::Vec3& up_vec) const override;
		bool GetOrientation(const Vec3& location, Quaternion& rot) const override;
		bool GetLocationOnTerrain(const Vec3& location, GeometryFlags flags, Vec3& terrain_location) const override;
	protected:
		//used by collision geometry
		ODECollisionMeshInfo CreateCollisionMeshAndCache(const std::string &cache_id, PhysicsMeshPtr mesh);
		dSpaceID GetSpace() const;
		ODECollisionMeshInfo GetCollisionMesh(const std::string &name);
		bool HasCollisionMesh(const std::string &name);
		void OnSceneObjectInitialize(PreSceneObjectInitializedEventPtr message);
		GASS_MUTEX& GetMutex() const {return m_Mutex;}
	private:
		ODECollisionMeshInfo _CreateCollisionMesh(PhysicsMeshPtr mesh) const;

		typedef std::map<std::string,ODECollisionMeshInfo> CollisionMeshMap;
		mutable GASS_MUTEX m_Mutex;
		float m_MaxRaySegment;
		SceneWeakPtr m_Scene;
		CollisionMeshMap m_ColMeshMap;
		dSpaceID m_Space;
		
	};
	typedef GASS_SHARED_PTR<ODECollisionSceneManager> ODECollisionSceneManagerPtr;
	
	
}
#endif