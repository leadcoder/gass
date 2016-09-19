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

#include "Plugins/OSG/OSGCollisionSceneManager.h"

#include "Plugins/OSG/OSGConvert.h"
#include "Plugins/OSG/OSGGraphicsSceneManager.h"
#include "Plugins/OSG/OSGGraphicsSystem.h"
#include "Plugins/OSG/OSGNodeData.h"

namespace GASS
{

	OSGCollisionSceneManager::OSGCollisionSceneManager() : m_IntersectVisitor(NULL),
		m_DatabaseCache(NULL)
	{

	}

	OSGCollisionSceneManager::~OSGCollisionSceneManager()
	{

	}

	void OSGCollisionSceneManager::RegisterReflection()
	{
	}

	void OSGCollisionSceneManager::OnCreate()
	{

	}

	void OSGCollisionSceneManager::OnInit()
	{
		
		OSGGraphicsSystemPtr system =  SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<OSGGraphicsSystem>();
		SystemListenerPtr listener = shared_from_this();
		system->Register(listener);

		m_IntersectVisitor = new CustomIntersectionVisitor();
		m_IntersectVisitor->setLODSelectionMode(osgUtil::IntersectionVisitor::USE_HIGHEST_LEVEL_OF_DETAIL);
		m_DatabaseCache  = new osgSim::DatabaseCacheReadCallback();
		//m_IntersectVisitor->setReadCallback(m_DatabaseCache);
	}

	void OSGCollisionSceneManager::OnShutdown()
	{
	}

	void OSGCollisionSceneManager::SystemTick(double /*delta_time*/)
	{

	}

	void OSGCollisionSceneManager::Raycast(const Vec3 &ray_start, const Vec3 &ray_dir, GeometryFlags flags, CollisionResult &result, bool return_at_first_hit) const
	{
		ScenePtr scene = GetScene();
		if(scene)
		{
			OSGGraphicsSceneManagerPtr gfx_sm = scene->GetFirstSceneManagerByClass<OSGGraphicsSceneManager>();

			OSGGraphicsSystemPtr gfx_sys = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<OSGGraphicsSystem>();
			osgViewer::ViewerBase::Views views;
			gfx_sys->GetViewer()->getViews(views);

			if(gfx_sm)
			{
				_ProcessRaycast(ray_start, ray_dir, flags,&result,views[0]->getCamera());
			}
		}
	}

	void OSGCollisionSceneManager::_ProcessRaycast(const Vec3 &ray_start, const Vec3 &ray_dir, GeometryFlags flags, CollisionResult *result, osg::Node *node) const
	{
		GASS_MUTEX_LOCK(m_Mutex)

		osg::Vec3d start = OSGConvert::ToOSG(ray_start);
		osg::Vec3d end = OSGConvert::ToOSG(ray_start + ray_dir);

		result->Coll = false;
		
		
		m_IntersectVisitor->reset();
		m_IntersectVisitor->setLODSelectionMode(osgUtil::IntersectionVisitor::USE_HIGHEST_LEVEL_OF_DETAIL);
		if(flags & GEOMETRY_FLAG_PAGED_LOD)
			m_IntersectVisitor->setReadCallback(m_DatabaseCache);
		else
			m_IntersectVisitor->setReadCallback(NULL);

		osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::MODEL, start, end);
		m_IntersectVisitor->setIntersector(intersector.get());

		int mask = OSGConvert::ToOSGNodeMask(flags);
		m_IntersectVisitor->setTraversalMask(mask);

		node->accept(*m_IntersectVisitor);

		if ( intersector->containsIntersections() )
		{
			osgUtil::LineSegmentIntersector::Intersections& intersections = intersector->getIntersections();
			for(osgUtil::LineSegmentIntersector::Intersections::iterator itr = intersections.begin();
				itr != intersections.end();
				++itr)
			{
				const osgUtil::LineSegmentIntersector::Intersection& intersection = *itr;

				//get first user data

				//reverse
				if(intersection.nodePath.size() > 0)
				{
					for(int i = intersection.nodePath.size()-1; i >= 0  ;i--)
					{
						if(intersection.nodePath[i]->getUserData())
						{
							OSGNodeData* data = dynamic_cast<OSGNodeData*>(intersection.nodePath[i]->getUserData());
							if(data)
							{
								BaseSceneComponentPtr bo = data->m_Component.lock();
								if(bo)
								{
									GeometryComponentPtr geom  = GASS_DYNAMIC_PTR_CAST<IGeometryComponent>(bo);
									if(geom)
									{
									
										if(flags & geom->GetGeometryFlags())
										{
											Vec3 col_pos = OSGConvert::ToGASS(intersection.getWorldIntersectPoint());
											Float col_dist = (col_pos - ray_start).FastLength(); 

											result->CollDist = col_dist;
											result->Coll = true;
											result->CollPosition = col_pos;
											result->CollNormal =  OSGConvert::ToGASS(intersection.getWorldIntersectNormal());
											result->CollSceneObject = bo->GetSceneObject();
											return;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}
