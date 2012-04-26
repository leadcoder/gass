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
#include "Sim/Scene/GASSScene.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"
#include "Core/System/GASSSystemFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"

namespace GASS
{

	ODECollisionSystem::ODECollisionSystem() : m_MaxRaySegment(300)
	{
		m_HandleCount = 5;
	}

	ODECollisionSystem::~ODECollisionSystem()
	{

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


	void ODECollisionSystem::Process()
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
				ODEPhysicsSceneManagerPtr ode_scene = boost::shared_static_cast<ODEPhysicsSceneManager>(scene->GetSceneManager("PhysicsSceneManager"));
				if(request.Type == COL_LINE)
				{
					CollisionResult result;
					ODELineCollision raycast(&request,&result,ode_scene,m_MaxRaySegment);
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
			ODEPhysicsSceneManagerPtr ode_scene = boost::shared_static_cast<ODEPhysicsSceneManager>(scene->GetSceneManager("PhysicsSceneManager"));
			if(request.Type == COL_LINE)
			{
				ODELineCollision raycast(&request,&result,ode_scene,m_MaxRaySegment);
				raycast.Process();
			}
		}
	}

	void ODECollisionSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("ODECollisionSystem",new GASS::Creator<ODECollisionSystem, ISystem>);
	}

	void ODECollisionSystem::OnCreate()
	{
		int address = (int) this;
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(ODECollisionSystem::OnUnloadScene,SceneUnloadNotifyMessage,0));
		//m_Owner->GetMessageManager()->RegisterForMessage(SystemManager::SYSTEM_RM_UPDATE, address,  boost::bind( &ODECollisionSystem::OnUpdate, this, _1 ),0);
	}

	void ODECollisionSystem::OnUnloadScene(SceneUnloadNotifyMessagePtr message)
	{
		m_RequestMap.clear();
		m_ResultMap.clear();
	}

	Float ODECollisionSystem::GetHeight(ScenePtr scene, const Vec3 &pos, bool absolute) const
	{
		ODEPhysicsSceneManagerPtr ode_scene = boost::shared_static_cast<ODEPhysicsSceneManager>(scene->GetSceneManager("PhysicsSceneManager"));
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
		ODELineCollision raycast(&request,&result,ode_scene);
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




	
}
