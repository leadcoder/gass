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


#include <map>

#include "Sim/GASS.h"
#include "Plugins/OSG/Components/OSGCameraComponent.h"
#include <tbb/spin_mutex.h>
#include <osg/Geode>
#include <osg/Billboard>
#include <osgUtil/IntersectionVisitor>
#include "Sim/GASSThreading.h"
//namespace osgUtil {class IntersectionVisitor;}
namespace osgSim {class DatabaseCacheReadCallback;}

namespace GASS
{
	class CustomIntersectionVisitor : public osgUtil::IntersectionVisitor
	{
	public:

		CustomIntersectionVisitor() : IntersectionVisitor()
		{

		}

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



	class OSGCollisionSceneManager : public Reflection<OSGCollisionSceneManager, BaseSceneManager> , public ICollisionSceneManager
	{
	public:
	public:
		OSGCollisionSceneManager();
		virtual ~OSGCollisionSceneManager();
		static void RegisterReflection();
		virtual void OnCreate();
		virtual void OnInit();
		virtual void OnShutdown();
		virtual void SystemTick(double delta_time);
		virtual bool GetSerialize() const {return false;}

		//ICollisionSceneManager
		void Raycast(const Vec3 &ray_start, const Vec3 &ray_dir, GeometryFlags flags, CollisionResult &result, bool return_at_first_hit) const;
	private:
		void _ProcessRaycast(const Vec3 &ray_start, const Vec3 &ray_dir, GeometryFlags flags, CollisionResult *result, osg::Node *node) const;
		mutable GASS_MUTEX m_Mutex;
		osgUtil::IntersectionVisitor* m_IntersectVisitor;
		osgSim::DatabaseCacheReadCallback* m_DatabaseCache;
	};
	typedef SPTR<OSGCollisionSceneManager> OSGCollisionSceneManagerPtr;


}

