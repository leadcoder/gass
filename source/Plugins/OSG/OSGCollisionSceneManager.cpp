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
#include "Plugins/OSG/OSGCollisionSystem.h"
#include "Plugins/OSG/OSGGraphicsSystem.h"
#include "Plugins/OSG/OSGNodeData.h"
#include "Sim/Interface/GASSITerrainSceneManager.h"
#include <osg/CoordinateSystemNode>

namespace GASS
{

	void OSGCollisionSceneManager::RegisterReflection()
	{
	}

	OSGCollisionSceneManager::OSGCollisionSceneManager(SceneWeakPtr scene) : Reflection(scene), m_IntersectVisitor(NULL)
	{

	}

	OSGCollisionSceneManager::~OSGCollisionSceneManager()
	{

	}

	void OSGCollisionSceneManager::OnPostConstruction()
	{
		//register on system to get updates
		RegisterForPostUpdate<OSGGraphicsSystem>();

		m_ColSystem = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<OSGCollisionSystem>().get();
		m_GFXSystem = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<OSGGraphicsSystem>().get();
		m_IntersectVisitor = new CustomIntersectionVisitor();
		m_IntersectVisitor->setLODSelectionMode(osgUtil::IntersectionVisitor::USE_HIGHEST_LEVEL_OF_DETAIL);
		m_DatabaseCache = new osgSim::DatabaseCacheReadCallback();
	}

	void OSGCollisionSceneManager::OnSceneCreated()
	{
		m_Scene = GetScene().get();
		m_TerrainSM = GetScene()->GetFirstSceneManagerByClass<ITerrainSceneManager>(true).get();
	}

	void OSGCollisionSceneManager::OnSceneShutdown()
	{

	}

	void OSGCollisionSceneManager::Raycast(const Vec3 &ray_start, const Vec3 &ray_dir, GeometryFlags flags, CollisionResult &result, bool /*return_at_first_hit*/) const
	{
		if(m_GFXSystem)
		{
			osgViewer::ViewerBase::Views views;
			m_GFXSystem->GetViewer()->getViews(views);
			if(views.size() > 0)
				_ProcessRaycast(ray_start, ray_dir, flags,&result,views[0]->getCamera());
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
		if((flags & GEOMETRY_FLAG_PAGED_LOD) && m_ColSystem->GetReadPagedLOD())
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
					for(int i = static_cast<int>(intersection.nodePath.size()) - 1 ; i >= 0  ;i--)
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
											Float col_dist = (col_pos - ray_start).Length(); 

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

	bool OSGCollisionSceneManager::GetTerrainHeight(const Vec3& location, double& terrain_height, GeometryFlags flags) const
	{
		if (m_TerrainSM)
			return m_TerrainSM->GetTerrainHeight(location, terrain_height, flags);
		if (m_Scene->GetGeocentric())
		{
			const double r = osg::WGS_84_RADIUS_EQUATOR;

			const GASS::Vec3d up_vector = OSGConvert::ToGASS(m_EllipsoidModel.computeLocalUpVector(location.x, -location.z, location.y));
			double latitude, longitude, height_hae;
			m_EllipsoidModel.convertXYZToLatLongHeight(location.x, -location.z, location.y, latitude, longitude, height_hae);
			
			constexpr double min_height = -20000;
			const Vec3 dir = -up_vector * (height_hae - min_height);
			CollisionResult result;
			Raycast(location, dir, flags, result, true);
			if (result.Coll)
			{
				terrain_height = height_hae - (location - result.CollPosition).Length();
			}
			return result.Coll;
		}
		else
		{
			constexpr double max_terrain_height = 20000;
			CollisionResult result;
			const Vec3 ray_start(location.x, max_terrain_height, location.z);
			const Vec3 ray_direction = Vec3(0, -1, 0) * (max_terrain_height * 2.0);
			Raycast(ray_start, ray_direction, flags, result,true);
			if (result.Coll)
			{
				terrain_height = result.CollPosition.y;
			}
		}
		return false;
	}

	bool OSGCollisionSceneManager::GetHeightAboveSeaLevel(const Vec3& location, double& height) const
	{
		if (m_Scene->GetGeocentric())
		{
			double latitude, longitude;
			m_EllipsoidModel.convertXYZToLatLongHeight(location.x, -location.z, location.y, latitude, longitude, height);
		}
		else
		{
			return location.y;
		}
		return true;
	}

	bool OSGCollisionSceneManager::GetHeightAboveTerrain(const Vec3& location, double& height, GeometryFlags flags) const
	{
		if (m_TerrainSM)
			return m_TerrainSM->GetHeightAboveTerrain(location, height,flags);

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

	bool OSGCollisionSceneManager::GetUpVector(const Vec3& location, GASS::Vec3& up_vec) const
	{
		if (m_TerrainSM)
			return m_TerrainSM->GetUpVector(location, up_vec);

		if (m_Scene->GetGeocentric())
		{
			up_vec = OSGConvert::ToGASS(m_EllipsoidModel.computeLocalUpVector(location.x, -location.z, location.y));
		}
		else
		{
			up_vec.Set(0,1,0);
		}
		return true;
	}

	bool OSGCollisionSceneManager::GetOrientation(const Vec3& location, Quaternion& rot) const
	{
		if (m_TerrainSM)
			return m_TerrainSM->GetOrientation(location, rot);

		if (m_Scene->GetGeocentric())
		{
			double  latitude, longitude, height;
			m_EllipsoidModel.convertXYZToLatLongHeight(location.x, -location.z, location.y, latitude, longitude, height);
			osg::Matrixd localToWorld;
			m_EllipsoidModel.computeCoordinateFrame(latitude, longitude, localToWorld);
			rot = GASS::OSGConvert::ToGASS(localToWorld.getRotate());
		}
		else
		{
			rot = Quaternion::IDENTITY;
		}
		return true;
	}
}
