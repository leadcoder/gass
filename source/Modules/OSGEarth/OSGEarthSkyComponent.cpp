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

#include "OSGEarthCommonIncludes.h"
#include "OSGEarthSkyComponent.h"

#include "Modules/Graphics/OSGNodeMasks.h"
#include "Modules/Graphics/OSGConvert.h"
#include "Modules/Graphics/OSGGraphicsSceneManager.h"
#include "Modules/Graphics/OSGGraphicsSystem.h"

namespace GASS
{
	OSGEarthSkyComponent::OSGEarthSkyComponent() 
	{

	}

	OSGEarthSkyComponent::~OSGEarthSkyComponent()
	{

	}

	void OSGEarthSkyComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<OSGEarthSkyComponent>();
	}
	
	void OSGEarthSkyComponent::OnDelete()
	{

	}

	void OSGEarthSkyComponent::OnInitialize()
	{
		OSGGraphicsSceneManagerPtr osg_sm  = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OSGGraphicsSceneManager>();

		osg::ref_ptr<osg::Group> root = osg_sm->GetOSGRootNode();
		//osg::Group* root = static_cast<osg::Group*> (message->GetUserData());
		osgEarth::MapNode* map_node = osgEarth::MapNode::findMapNode(root);
		if(map_node)
		{
			osgEarth::SkyOptions sky_options;
			sky_options.coordinateSystem() = osgEarth::SkyOptions::COORDSYS_ECEF;
			osgEarth::SkyNode* sky = osgEarth::SkyNode::create(sky_options);
			sky->setDateTime(osgEarth::DateTime(2013, 1, 6, 17.0));
			root->addChild( sky );

			//hack
			OSGGraphicsSystemPtr osg_sys = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<OSGGraphicsSystem>();
			osgViewer::ViewerBase::Views views;
			osg_sys->GetViewer()->getViews(views);
			sky->attach( views[0] );
		}
		//else
		//	Log::Warning("No MapNode found for OSGEarthSkyComponent");
	}
}

