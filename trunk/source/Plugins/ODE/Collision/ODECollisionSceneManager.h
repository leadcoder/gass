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

#ifndef ODE_SCENE_MANAGER_H
#define ODE_SCENE_MANAGER_H

#include <ode/ode.h>
#include "Core/MessageSystem/GASSBaseMessage.h"
#include "Sim/GASSCommon.h"
#include "Sim/GASSBaseSceneManager.h"
#include "Sim/Interface/GASSICollisionSceneManager.h"
#include "Sim/Messages/GASSCoreSceneMessages.h"
#include "tbb/spin_mutex.h"

namespace GASS
{
	class IMeshComponent;
	typedef boost::shared_ptr<IMeshComponent> MeshComponentPtr;
	struct MeshData;
	struct ODECollisionMeshInfo
	{
		MeshData* Mesh;
		dTriMeshDataID ID;
	};

	/**
		Collision Scene Manager
	*/
	class ODECollisionSceneManager : public Reflection<ODECollisionSceneManager, BaseSceneManager> , public ICollisionSceneManager
	{
		friend class ODECollisionGeometryComponent;
	public:
		

		ODECollisionSceneManager();
		virtual ~ODECollisionSceneManager();
		static void RegisterReflection();
		virtual void OnCreate();
		virtual void OnInit();
		virtual void OnShutdown();
		virtual void SystemTick(double delta_time);
		virtual bool GetSerialize() const {return false;}
		//ICollisionSceneManager
		virtual CollisionHandle Request(const CollisionRequest &request);
		virtual bool Check(CollisionHandle handle, CollisionResult &result);
		virtual void Force(CollisionRequest &request, CollisionResult &result) const;
		virtual Float GetHeight(const Vec3 &pos, bool absolute=true) const;
	protected:
		void Process();
		//used by collision geometry
		ODECollisionMeshInfo CreateCollisionMesh(const std::string &col_mesh_id, MeshComponentPtr mesh);
		dSpaceID GetSpace() const;
		bool HasCollisionMesh(const std::string &name);
		void OnSceneObjectInitialize(PreSceneObjectInitializedEventPtr message);
	private:
		typedef std::map<CollisionHandle,CollisionRequest> RequestMap;
		typedef std::map<CollisionHandle,CollisionResult> ResultMap;
		typedef std::map<std::string,ODECollisionMeshInfo> CollisionMeshMap;

		RequestMap m_RequestMap;
		ResultMap m_ResultMap;
		unsigned int m_HandleCount;
		tbb::spin_mutex m_RequestMutex;
		tbb::spin_mutex m_ResultMutex;
		float m_MaxRaySegment;
		SceneWeakPtr m_Scene;
		CollisionMeshMap m_ColMeshMap;
		dSpaceID m_Space;
	};
	typedef boost::shared_ptr<ODECollisionSceneManager> ODECollisionSceneManagerPtr; 
	
	
}
#endif