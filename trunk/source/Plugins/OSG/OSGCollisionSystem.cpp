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
	class MyVisitor : public osgUtil::IntersectionVisitor
	{
	public:

		MyVisitor(osgUtil::Intersector* intersector, osgUtil::IntersectionVisitor::ReadCallback* readCallback) : IntersectionVisitor(intersector, readCallback)
		{
		}

		virtual ~MyVisitor()
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

	OSGCollisionSystem::OSGCollisionSystem()
	{
		m_HandleCount = 5;
	}

	OSGCollisionSystem::~OSGCollisionSystem()
	{

	}

	void OSGCollisionSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("OSGCollisionSystem",new GASS::Creator<OSGCollisionSystem, ISystem>);
	}

	void OSGCollisionSystem::Init()
	{
		int address = (int) this;
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(OSGCollisionSystem::OnUnloadScene,SceneUnloadNotifyMessage,0));
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(OSGCollisionSystem::OnLoadScene,SceneAboutToLoadNotifyMessage,0));
	}

	void OSGCollisionSystem::OnUnloadScene(SceneUnloadNotifyMessagePtr message)
	{
		m_RequestMap.clear();
		m_ResultMap.clear();
		message->GetScene()->UnregisterForMessage(UNREG_TMESS(OSGCollisionSystem::OnChangeCamera,ChangeCameraMessage));	
	}


	void OSGCollisionSystem::OnLoadScene(SceneAboutToLoadNotifyMessagePtr message)
	{
		message->GetScene()->RegisterForMessage(REG_TMESS(OSGCollisionSystem::OnChangeCamera,ChangeCameraMessage,0));	
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

	void OSGCollisionSystem::Force(CollisionRequest &request, CollisionResult &result) const
	{
		ScenePtr scene(request.Scene);
		if(scene)
		{
			OSGGraphicsSceneManagerPtr gfx_sm = scene->GetFirstSceneManagerByClass<OSGGraphicsSceneManager>();

			OSGGraphicsSystemPtr gfx_sys = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<OSGGraphicsSystem>();
			osgViewer::ViewerBase::Views views;
			gfx_sys->GetViewer()->getViews(views);

			if(gfx_sm)
			{
				if(request.Type == COL_LINE)
				{
					ProcessRaycast(&request,&result,views[0]->getCamera());//gfx_sm->GetOSGRootNode());
				}
			}
		}
	}

	

	void OSGCollisionSystem::OnChangeCamera(ChangeCameraMessagePtr message)
	{

	}

	Float OSGCollisionSystem::GetHeight(ScenePtr scene, const Vec3 &pos, bool absolute) const
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
		request.Scene = scene;
		request.ReturnFirstCollisionPoint = false;
		request.CollisionBits = 2;

		OSGGraphicsSceneManagerPtr gfx_sm = scene->GetFirstSceneManagerByClass<OSGGraphicsSceneManager>();
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
	}

	void OSGCollisionSystem::ProcessRaycast(CollisionRequest *request,CollisionResult *result, osg::Node *node) const
	{
		osg::Vec3d start = OSGConvert::Get().ToOSG(request->LineStart);
		osg::Vec3d end = OSGConvert::Get().ToOSG(request->LineEnd);

		result->Coll = false;

		osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::MODEL, start, end);
		//osgUtil::IntersectionVisitor intersectVisitor( intersector.get(), NULL);//new MyReadCallback );
		MyVisitor intersectVisitor( intersector.get(), NULL);//new MyReadCallback );

		//OSGCameraComponentPtr camera (m_CurrentCamera);
		//LocationComponentPtr location = camera->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		//Vec3 cam_pos = location->GetWorldPosition();
		//intersectVisitor.setReferenceEyePoint(OSGConvert::Get().ToOSG(cam_pos)); 
		//intersectVisitor.setReferenceEyePointCoordinateFrame(osgUtil::Intersector::MODEL); 
		//node = camera->GetOSGCamera();
	//	GeometryFlags cat(GT_REGULAR);
		if(request->CollisionBits == 1)
		{
			intersectVisitor.setTraversalMask(NM_REGULAR_GEOMETRY | NM_TERRAIN_GEOMETRY);
		}
		else if(request->CollisionBits == 2)
		{
			intersectVisitor.setTraversalMask(NM_GIZMO_GEOMETRY);
	//		cat.Set(GT_GIZMO);
		}
		else if(request->CollisionBits == 4)
		{
			intersectVisitor.setTraversalMask(NM_TERRAIN_GEOMETRY);
		//	cat.Set(GT_TERRAIN);
		}

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
								BaseSceneComponentPtr bo(data->m_Component,boost::detail::sp_nothrow_tag());
								if(bo)
								{
									GeometryComponentPtr geom  = boost::shared_dynamic_cast<IGeometryComponent>(bo);
									if(geom)
									{
										/*bool found = false;
										if(request->CollisionBits == 1 && (geom->GetGeometryFlags() == GT_REGULAR || geom->GetGeometryFlags() == GT_TERRAIN))
										{
											found = true;
										}
										else if(request->CollisionBits == 2 && geom->GetGeometryFlags() == GT_GIZMO )
										{
											found = true;
										}
										else if(request->CollisionBits == 4 && geom->GetGeometryFlags() == GT_TERRAIN)
										{
											found = true;
										}

										if(found)*/
										{
											Vec3 col_pos = OSGConvert::Get().ToGASS(intersection.getWorldIntersectPoint());
											Float col_dist = (col_pos - request->LineStart).FastLength(); 

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


	void OSGCollisionSystem::Update(double delta_time)
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

		for(iter = requestMap.begin(); iter != requestMap.end(); ++iter)
		{
			CollisionRequest request =  iter->second;
			CollisionHandle handle = iter->first;
			ScenePtr scene = ScenePtr(request.Scene);

			if(scene)
			{
				OSGGraphicsSceneManagerPtr gfx_sm = scene->GetFirstSceneManagerByClass<OSGGraphicsSceneManager>();
				OSGGraphicsSystemPtr gfx_sys = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<OSGGraphicsSystem>();

				osgViewer::ViewerBase::Views views;
				gfx_sys->GetViewer()->getViews(views);
				//set same scene in all viewports for the moment 
				if(views.size() > 0)
				{

					//if(gfx_sm)
					//{
					if(request.Type == COL_LINE)
					{
						CollisionResult result;
						ProcessRaycast(&request,&result,views[0]->getCamera());//gfx_sm->GetOSGRootNode());
						resultMap[handle] = result;
					}
					//}
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
	}

	TaskGroup OSGCollisionSystem::GetTaskGroup() const
	{
		return MAIN_TASK_GROUP;
	}
}
