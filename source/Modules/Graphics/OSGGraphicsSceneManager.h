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
#include "Sim/GASS.h"
#include "Sim/Interface/GASSIGraphicsSceneManager.h"
#include "Modules/Graphics/OSGCommon.h"
#include "OSGDebugDraw.h"

namespace osgShadow
{
	class ShadowedScene;
}

namespace GASS
{
	class OSGGraphicsSystem;
	using OSGGraphicsSystemWeakPtr = std::weak_ptr<OSGGraphicsSystem>;

	class ExportOSG OSGGraphicsSceneManager : public Reflection<OSGGraphicsSceneManager, BaseSceneManager>, public IGraphicsSceneManager
	{
	public:
		OSGGraphicsSceneManager(SceneWeakPtr scene);
		virtual ~OSGGraphicsSceneManager() {};
		virtual osg::Group* GetObjectGroup() = 0;
		virtual osg::ref_ptr<osg::Group> GetOSGRootNode() = 0;
		virtual void PostProcess(osg::Node*) {};
	private:
	};
	using OSGGraphicsSceneManagerPtr = std::shared_ptr<OSGGraphicsSceneManager>;
	using OSGGraphicsSceneManagerWeakPtr = std::weak_ptr<OSGGraphicsSceneManager>;
}