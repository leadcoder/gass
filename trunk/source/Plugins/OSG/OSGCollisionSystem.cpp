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

#include "Plugins/OSG/OSGCollisionSystem.h"
#include "Plugins/OSG/OSGConvert.h"
#include "Plugins/OSG/Components/OSGMeshComponent.h"

#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/SimEngine.h"
#include "Core/System/SystemFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"

#include <osgUtil/IntersectionVisitor>
#include <osgUtil/LineSegmentIntersector>

#include <osgSim/LineOfSight>
#include <osgSim/HeightAboveTerrain>
#include <osgSim/ElevationSlice>


namespace GASS
{

	OSGCollisionSystem::OSGCollisionSystem()
	{
		m_HandleCount = 5;
	}

	OSGCollisionSystem::~OSGCollisionSystem()
	{

	}

	CollisionHandle OSGCollisionSystem::Request(const CollisionRequest &request)
	{
		tbb::spin_mutex::scoped_lock lock(m_RequestMutex);
		//assert(request.Scene);
		m_HandleCount = ( m_HandleCount + 1 ) % 0xFFFFFFFE;
		CollisionHandle handle = m_HandleCount;
		m_RequestMap[handle] = request;
		return handle;
	}

	void OSGCollisionSystem::Process()
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

		for(iter = requestMap.begin(); iter != requestMap.end(); iter++)
		{
			CollisionRequest request =  iter->second;
			CollisionHandle handle = iter->first;
			ScenarioScenePtr scene = ScenarioScenePtr(request.Scene);

			if(scene)
			{
				//ODEPhysicsSceneManagerPtr ode_scene = boost::shared_static_cast<ODEPhysicsSceneManager>(scene->GetSceneManager("PhysicsSceneManager"));
				if(request.Type == COL_LINE)
				{
					CollisionResult result;
					//ODELineCollision raycast(&request,&result,ode_scene);
					//raycast.Process();
					resultMap[handle] = result;
				}
			}

		}
		{
			tbb::spin_mutex::scoped_lock lock(m_ResultMutex);
			//transfer results
			for(res_iter = resultMap.begin(); res_iter != resultMap.end(); res_iter++)
			{
				CollisionHandle handle = res_iter->first;
				m_ResultMap[handle] = res_iter->second;
			}
		}
	}

	bool OSGCollisionSystem::Check(CollisionHandle handle, CollisionResult &result)
	{
		tbb::spin_mutex::scoped_lock lock(m_ResultMutex);
		ResultMap::iterator iter = m_ResultMap.find(handle);
		if(iter != m_ResultMap.end())
		{
			result = m_ResultMap[handle];
			m_ResultMap.erase(iter);
			return true;
		}
		return false;
	}

	void OSGCollisionSystem::Force(CollisionRequest &request, CollisionResult &result)
	{
		ScenarioScenePtr scene(request.Scene);
		if(scene)
		{
			//ODEPhysicsSceneManagerPtr ode_scene = boost::shared_static_cast<ODEPhysicsSceneManager>(scene->GetSceneManager("PhysicsSceneManager"));
			if(request.Type == COL_LINE)
			{
				//ODELineCollision raycast(&request,&result,ode_scene);
				//raycast.Process();
			}
		}
	}

	void OSGCollisionSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("OSGCollisionSystem",new GASS::Creator<OSGCollisionSystem, ISystem>);
	}

	void OSGCollisionSystem::OnCreate()
	{
		int address = (int) this;
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(OSGCollisionSystem::OnUnloadScene,ScenarioSceneUnloadNotifyMessage,0));
	}

	void OSGCollisionSystem::OnUnloadScene(ScenarioSceneUnloadNotifyMessagePtr message)
	{
		m_RequestMap.clear();
		m_ResultMap.clear();
	}

	Float OSGCollisionSystem::GetHeight(ScenarioScenePtr scene, const Vec3 &pos, bool absolute) const
	{
		//ODEPhysicsSceneManagerPtr ode_scene = boost::shared_static_cast<ODEPhysicsSceneManager>(scene->GetSceneManager("PhysicsSceneManager"));
		CollisionRequest request;
		CollisionResult result;

		Vec3 up = scene->GetSceneUp();

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
		//OSGLineCollision raycast(&request,&result,ode_scene);
		//raycast.Process();

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

	void OSGCollisionSystem::ProcessRaycast(CollisionRequest *request,CollisionResult *result, osg::Node *node)
	{
		osg::Vec3d start = OSGConvert::Get().ToOSG(request->LineStart);
		osg::Vec3d end = OSGConvert::Get().ToOSG(request->LineEnd);
		/*osgSim::LineOfSight los;
		los.addLOS(start,end);
		los.computeIntersections(node);

		result->Coll = false;
		result->CollDist = 0;

		for(unsigned int i=0; i<los.getNumLOS(); i++)
		{
			const osgSim::LineOfSight::Intersections& intersections = los.getIntersections(i);
			for(osgSim::LineOfSight::Intersections::const_iterator itr = intersections.begin();
				itr != intersections.end();
				++itr)
			{
				//std::cout<<"  point "<<*itr<<std::endl;
				//return (*itr).z();
				result->Coll = true;
				//result->CollSceneObject = scene_object;	
				result->CollPosition = OSGConvert::Get().ToGASS(*itr);
			}
		}*/


		osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(start, end);
        osgUtil::IntersectionVisitor intersectVisitor( intersector.get(), NULL);//new MyReadCallback );
        node->accept(intersectVisitor);

        if ( intersector->containsIntersections() )
        {
            osgUtil::LineSegmentIntersector::Intersections& intersections = intersector->getIntersections();
            for(osgUtil::LineSegmentIntersector::Intersections::iterator itr = intersections.begin();
                itr != intersections.end();
                ++itr)
            {
                const osgUtil::LineSegmentIntersector::Intersection& intersection = *itr;

				result->Coll = true;
				//result->CollSceneObject = scene_object;	
				result->CollPosition = OSGConvert::Get().ToGASS(intersection.localIntersectionPoint);
				result->CollNormal =  OSGConvert::Get().ToGASS(intersection.localIntersectionNormal);

				//get first user data
				for(std::size_t i = 0; i < intersection.nodePath.size() ;i ++)
				{
					if(intersection.nodePath[i]->getUserData())
					{
						OSGMeshComponent* mesh = (OSGMeshComponent*) intersection.nodePath[i]->getUserData();
						result->CollSceneObject = mesh->GetSceneObject();
						break;		
					}
				}
				break;
				
               /* std::cout << "  ratio "<<intersection.ratio<<std::endl;
                std::cout << "  point "<<intersection.localIntersectionPoint<<std::endl;
                std::cout << "  normal "<<intersection.localIntersectionNormal<<std::endl;
                std::cout << "  indices "<<intersection.indexList.size()<<std::endl;
                std::cout << "  primitiveIndex "<<intersection.primitiveIndex<<std::endl;
                std::cout<<std::endl;*/
            }
        }
	}
}
