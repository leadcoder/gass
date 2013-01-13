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

#pragma once


#include <ode/ode.h>
#include <map>
#include "Sim/Interface/GASSICollisionSystem.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/Messages/GASSCoreSceneMessages.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/GASSSimSystem.h"
#include "Sim/Messages/GASSCoreSystemMessages.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"
#include "Core/System/GASSBaseSystem.h"
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

	class ODECollisionSystem : public Reflection<ODECollisionSystem , SimSystem> , public ICollisionSystem
	{
	public:
		typedef std::map<CollisionHandle,CollisionRequest> RequestMap;
		typedef std::map<CollisionHandle,CollisionResult> ResultMap;
		typedef std::map<std::string,ODECollisionMeshInfo> CollisionMeshMap;
	public:
		ODECollisionSystem();
		virtual ~ODECollisionSystem();
		static void RegisterReflection();
		virtual void Init();
		virtual void Update(double delta_time);
		virtual std::string GetSystemName() const {return "ODECollisionSystem";}

		CollisionHandle Request(const CollisionRequest &request);
		bool Check(CollisionHandle handle, CollisionResult &result);
		void Force(CollisionRequest &request, CollisionResult &result) const;
		void Process();
		Float GetHeight(ScenePtr scene, const Vec3 &pos, bool absolute=true) const;
	
		//used by collision geometry
		ODECollisionMeshInfo CreateCollisionMesh(const std::string &col_mesh_id, MeshComponentPtr mesh);
		dSpaceID GetSpace() const;
	private:
		bool HasCollisionMesh(const std::string &name);
		void OnPreSceneCreate(PreSceneCreateEventPtr message);
		void OnSceneUnloaded(SceneUnloadedEventPtr message);
		void OnSceneObjectInitialize(PreSceneObjectInitializedEventPtr message);
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
	typedef boost::shared_ptr<ODECollisionSystem> ODECollisionSystemPtr;


}

