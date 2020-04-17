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

#pragma once


#include <map>

#include "Sim/GASS.h"
#include "Sim/GASSThreading.h"
#include "Plugins/OSG/OSGCommon.h"
namespace osgSim {class DatabaseCacheReadCallback;}

namespace GASS
{
	/**
		Extended IntersectionVisitor to support billboard intersection and plod with empty string
	*/
	class CustomIntersectionVisitor : public osgUtil::IntersectionVisitor
	{
	public:

		CustomIntersectionVisitor() : IntersectionVisitor()
		{

		}

		CustomIntersectionVisitor(osgUtil::Intersector* intersector, osgUtil::IntersectionVisitor::ReadCallback* readCallback) : IntersectionVisitor(intersector, readCallback)
		{
		}

		~CustomIntersectionVisitor() override
		{

		}

        void apply(osg::PagedLOD& plod) override
        {
            if (!enter(plod)) return;

            if (plod.getNumFileNames() > 0)
            {
                // Identify the range value for the highest res child
                float targetRangeValue;
                if (plod.getRangeMode() == osg::LOD::DISTANCE_FROM_EYE_POINT)
                    targetRangeValue = 1e6; // Init high to find min value
                else
                    targetRangeValue = 0; // Init low to find max value

                const osg::LOD::RangeList rl = plod.getRangeList();
                osg::LOD::RangeList::const_iterator rit;
                for (rit = rl.begin();
                    rit != rl.end();
                    rit++)
                {
                    if (plod.getRangeMode() == osg::LOD::DISTANCE_FROM_EYE_POINT)
                    {
                        if (rit->first < targetRangeValue)
                            targetRangeValue = rit->first;
                    }
                    else
                    {
                        if (rit->first > targetRangeValue)
                            targetRangeValue = rit->first;
                    }
                }

                // Perform an intersection test only on children that display
                // at the maximum resolution.
                unsigned int childIndex;
                for (rit = rl.begin(), childIndex = 0;
                    rit != rl.end();
                    rit++, childIndex++)
                {
                    if (rit->first != targetRangeValue)
                        // This is not one of the highest res children
                        continue;

                    osg::ref_ptr<osg::Node> child(NULL);
                    if (plod.getNumChildren() > childIndex)
                        child = plod.getChild(childIndex);

                    if ((!child.valid()) && (_readCallback.valid()))
                    {
                        // Child is NULL; attempt to load it, if we have a readCallback...
                        unsigned int validIndex(childIndex);
                        if (plod.getNumFileNames() <= childIndex)
                            validIndex = plod.getNumFileNames() - 1;

                        //Patched: Check for empty string
                        if(plod.getFileName(validIndex) != "") 
                            child = _readCallback->readNodeFile(plod.getDatabasePath() + plod.getFileName(validIndex));
                    }

                    if (!child.valid() && plod.getNumChildren() > 0)
                    {
                        // Child is still NULL, so just use the one at the end of the list.
                        child = plod.getChild(plod.getNumChildren() - 1);
                    }

                    if (child.valid())
                    {
                        child->accept(*this);
                    }
                }
            }
            leave();
        }

		void apply(osg::Billboard& billboard) override
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

	class OSGCollisionSystem;
	class OSGGraphicsSystem;

	class OSGCollisionSceneManager : public Reflection<OSGCollisionSceneManager, BaseSceneManager> , public ICollisionSceneManager
	{
	public:
		OSGCollisionSceneManager(SceneWeakPtr scene);
		~OSGCollisionSceneManager() override;
		static void RegisterReflection();
		void OnPostConstruction() override;
		void OnSceneCreated() override;
		void OnSceneShutdown() override;
		bool GetSerialize() const override {return false;}

		//ICollisionSceneManager
		void Raycast(const Vec3 &ray_start, const Vec3 &ray_dir, GeometryFlags flags, CollisionResult &result, bool return_at_first_hit) const override;
	private:
		void _ProcessRaycast(const Vec3 &ray_start, const Vec3 &ray_dir, GeometryFlags flags, CollisionResult *result, osg::Node *node) const;
		mutable GASS_MUTEX m_Mutex;
		osgUtil::IntersectionVisitor* m_IntersectVisitor;
		osg::ref_ptr<osgSim::DatabaseCacheReadCallback> m_DatabaseCache;
		OSGCollisionSystem* m_ColSystem = nullptr;
		OSGGraphicsSystem* m_GFXSystem = nullptr;
	};
	typedef GASS_SHARED_PTR<OSGCollisionSceneManager> OSGCollisionSceneManagerPtr;
}