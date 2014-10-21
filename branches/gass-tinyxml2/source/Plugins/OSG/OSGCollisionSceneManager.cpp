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

#include "Plugins/OSG/OSGCollisionSceneManager.h"
#include "Plugins/OSG/OSGConvert.h"
//#include "Plugins/OSG/Components/OSGMeshComponent.h"
#include "Plugins/OSG/OSGGraphicsSceneManager.h"
#include "Plugins/OSG/OSGNodeMasks.h"
#include "Plugins/OSG/OSGGraphicsSystem.h"
#include "Plugins/OSG/OSGNodeData.h"


#include <osgUtil/IntersectionVisitor>
#include <osgUtil/LineSegmentIntersector>

#include <osgSim/LineOfSight>
#include <osgSim/HeightAboveTerrain>
#include <osgSim/ElevationSlice>
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>



namespace GASS
{
	class CustomIntersectionVisitor : public osgUtil::IntersectionVisitor
	{
	public:

		CustomIntersectionVisitor(osgUtil::Intersector* intersector, osgUtil::IntersectionVisitor::ReadCallback* readCallback) : IntersectionVisitor(intersector, readCallback)
		{
		}

		virtual ~CustomIntersectionVisitor()
		{

		}
		void apply(osg::Billboard& billboard)
		{
			if (!enter(billboard)) return;

#if 1
			// IntersectVisitor doesn't have getEyeLocal(), can we use NodeVisitor::getEyePoint()?
			osg::Vec3 eye_local = getEyePoint();

			for(unsigned int i = 0; i < billboard.getNumDrawables(); i++ )
			{
				const osg::Vec3& pos = billboard.getPosition(i);
				osg::ref_ptr<osg::RefMatrix> billboard_matrix = new osg::RefMatrix;
				if (getViewMatrix())
				{
					if (getModelMatrix()) billboard_matrix->mult( *getModelMatrix(), *getViewMatrix() );
					else billboard_matrix->set( *getViewMatrix() );
				}
				else if (getModelMatrix()) billboard_matrix->set( *getModelMatrix() );

				billboard.computeMatrix(*billboard_matrix,eye_local,pos);

				if (getViewMatrix()) billboard_matrix->postMult( osg::Matrix::inverse(*getViewMatrix()) );
				pushModelMatrix(billboard_matrix.get());

				// now push an new intersector clone transform to the new local coordinates
				push_clone();

				intersect( billboard.getDrawable(i) );

				// now push an new intersector clone transform to the new local coordinates
				pop_clone();

				popModelMatrix();

			}
#else

			for(unsigned int i=0; i<billboard.getNumDrawables(); ++i)
			{
				intersect( billboard.getDrawable(i) );
			}
#endif

			leave();
		}
	};

	OSGCollisionSceneManager::OSGCollisionSceneManager()
	{
	}

	OSGCollisionSceneManager::~OSGCollisionSceneManager()
	{

	}

	void OSGCollisionSceneManager::RegisterReflection()
	{
		//SystemFactory::GetPtr()->Register("OSGCollisionSceneManager",new GASS::Creator<OSGCollisionSceneManager, ISystem>);
	}

	void OSGCollisionSceneManager::OnCreate()
	{

	}

	void OSGCollisionSceneManager::OnInit()
	{
		
		OSGGraphicsSystemPtr system =  SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<OSGGraphicsSystem>();
		SystemListenerPtr listener = shared_from_this();
		system->Register(listener);
	}

	void OSGCollisionSceneManager::OnShutdown()
	{
	}

	void OSGCollisionSceneManager::SystemTick(double delta_time)
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
	
	/*Float OSGCollisionSceneManager::GetHeight(const Vec3 &pos, bool absolute) const
	{
		CollisionRequest request;
		CollisionResult result;

		Vec3 up = Vec3(0,1,0);

		Vec3 ray_start = pos;
		Vec3 ray_direction = -up;
		//max raycast 2000000 units down
		ray_direction = ray_direction*2000000;

		request.LineStart = ray_start;
		request.LineEnd = ray_start + ray_direction;
		request.Type = COL_LINE;
		request.ReturnFirstCollisionPoint = false;
		request.CollisionBits = GEOMETRY_FLAG_SCENE_OBJECTS;

		OSGGraphicsSceneManagerPtr gfx_sm = GetScene()->GetFirstSceneManagerByClass<OSGGraphicsSceneManager>();
		if(gfx_sm)
		{
			ProcessRaycast(&request,&result,gfx_sm->GetOSGRootNode());
		}

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
	}*/

	void OSGCollisionSceneManager::_ProcessRaycast(const Vec3 &ray_start, const Vec3 &ray_dir, GeometryFlags flags, CollisionResult *result, osg::Node *node) const
	{
		osg::Vec3d start = OSGConvert::Get().ToOSG(ray_start);
		osg::Vec3d end = OSGConvert::Get().ToOSG(ray_start + ray_dir);

		result->Coll = false;

		osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::MODEL, start, end);
		//osgUtil::IntersectionVisitor intersectVisitor( intersector.get(), NULL);//new MyReadCallback );
		CustomIntersectionVisitor intersectVisitor( intersector.get(), NULL);//new MyReadCallback );

		int mask = OSGConvert::Get().ToOSGNodeMask(flags);
		intersectVisitor.setTraversalMask(mask);

		node->accept(intersectVisitor);


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
						//for(std::size_t i = 0; i < intersection.nodePath.size() ;i ++)
					{
						if(intersection.nodePath[i]->getUserData())
						{
							OSGNodeData* data = dynamic_cast<OSGNodeData*>(intersection.nodePath[i]->getUserData());
							//OSGNodeData* data = (OSGNodeData*)intersection.nodePath[i]->getUserData();
							if(data)
							{
								BaseSceneComponentPtr bo(data->m_Component,NO_THROW);
								if(bo)
								{
									GeometryComponentPtr geom  = DYNAMIC_PTR_CAST<IGeometryComponent>(bo);
									if(geom)
									{
									
										if(flags && geom->GetGeometryFlags())
										{
											Vec3 col_pos = OSGConvert::Get().ToGASS(intersection.getWorldIntersectPoint());
											Float col_dist = (col_pos - ray_start).FastLength(); 

											result->CollDist = col_dist;
											result->Coll = true;
											result->CollPosition = col_pos;
											result->CollNormal =  OSGConvert::Get().ToGASS(intersection.getWorldIntersectNormal());
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
