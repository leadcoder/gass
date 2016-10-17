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



#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgGA/NodeTrackerManipulator>
#include <osgGA/TrackballManipulator>
#include <osg/MatrixTransform>
#include <osgShadow/ShadowTechnique>
#include <osgEarth/MapNode>
#include <osgEarthUtil/Sky>
#include <osgEarthUtil/ExampleResources>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/LatLongFormatter>
#include <osgEarthUtil/MGRSFormatter>
#include <osgEarthUtil/MouseCoordsTool>
#include <osgEarthUtil/AutoClipPlaneHandler>
#include <osgEarthUtil/Controls>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthAnnotation/AnnotationData>
#include <osgEarthAnnotation/AnnotationRegistry>
#include "Plugins/OSG/IOSGGraphicsSceneManager.h"
#include "Plugins/OSG/IOSGGraphicsSystem.h"

#include <osg/Camera>
#include "Plugins/OSG/OSGNodeMasks.h"
#include "Plugins/OSG/OSGConvert.h"
#include "Plugins/OSG/IOSGGraphicsSceneManager.h"
#include "Sim/GASSBaseSceneManager.h"
#include "Sim/Interface/GASSIGraphicsSceneManager.h"
#include "OSGEarthMapComponent.h"
#include "OSGEarthSceneManager.h"
#include "Plugins/OSG/IOSGNode.h"
#include "Plugins/OSG/OSGNodeData.h"

namespace GASS
{
	OSGEarthMapComponent::OSGEarthMapComponent() : m_Initlized(false)
	{

	}

	OSGEarthMapComponent::~OSGEarthMapComponent()
	{

	}

	void OSGEarthMapComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("OSGEarthMapComponent",new Creator<OSGEarthMapComponent, Component>);
		RegisterProperty<std::string>("EarthFile", &OSGEarthMapComponent::GetEarthFile, &OSGEarthMapComponent::SetEarthFile);
	}

	void OSGEarthMapComponent::OnInitialize()
	{
		SceneManagerListenerPtr listener = shared_from_this();
		GASS_SHARED_PTR<BaseSceneManager> osg_sm = GASS_DYNAMIC_PTR_CAST<BaseSceneManager>(GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<IGraphicsSceneManager>());
		osg_sm->Register(listener);

		GASS_SHARED_PTR<OSGEarthSceneManager> earth_sm = GASS_DYNAMIC_PTR_CAST<OSGEarthSceneManager>(GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OSGEarthSceneManager>());
		m_Initlized = true;
		SetEarthFile(m_EarthFile);
	}

	void OSGEarthMapComponent::SetEarthFile(const std::string &earth_file)
	{
			m_EarthFile = earth_file;
			if (!m_Initlized)
				return;
			if (earth_file != "")
			{
				//disable GLSL
				IOSGGraphicsSceneManagerPtr osg_sm = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<IOSGGraphicsSceneManager>();
				OSGEarthSceneManagerPtr osgearth_sm = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OSGEarthSceneManager>();
				osg::ref_ptr<osg::Group> root = osg_sm->GetOSGRootNode();
				ResourceHandle rh(earth_file);
				std::string full_path = rh.GetResource()->Path().GetFullPath();
				osg::Node* node = osgDB::readNodeFile(full_path);
				m_MapNode = osgEarth::MapNode::findMapNode(node);
				root->addChild(m_MapNode);

				OSGNodeData* data = new OSGNodeData(shared_from_this());
				m_MapNode->setUserData(data);
				IOSGGraphicsSystemPtr osg_sys = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<IOSGGraphicsSystem>();

				osgViewer::ViewerBase::Views views;
				osg_sys->GetViewer()->getViews(views);
				
				const osgEarth::Config& externals = m_MapNode->externalConfig();
				const osgEarth::Config& skyConf = externals.child("sky");
				const osgEarth::Config& oceanConf = externals.child("ocean");
				const osgEarth::Config& annoConf = externals.child("annotations");
				const osgEarth::Config& declutterConf = externals.child("decluttering");

				osgEarth::Config        viewpointsConf = externals.child("viewpoints");

				// backwards-compatibility: read viewpoints at the top level:
				const osgEarth::ConfigSet& old_viewpoints = externals.children("viewpoint");
				for (osgEarth::ConfigSet::const_iterator i = old_viewpoints.begin(); i != old_viewpoints.end(); ++i)
					viewpointsConf.add(*i);

				// Loading a viewpoint list from the earth file:
				/*if ( !viewpointsConf.empty() )
				{
				std::vector<osgEarth::Viewpoint> viewpoints;

				const osgEarth::ConfigSet& children = viewpointsConf.children();
				if ( children.size() > 0 )
				{
				for( osgEarth::ConfigSet::const_iterator i = children.begin(); i != children.end(); ++i )
				{
				viewpoints.push_back( osgEarth::Viewpoint(*i) );
				}
				}

				if ( viewpoints.size() > 0 )
				{
				osgEarth::Util::Controls::Control* c = osgEarth::Util::ViewpointControlFactory().create(viewpoints, views[0]);
				if ( c )
				mainContainer->addControl( c );
				}
				}*/

				//if (m_UseSky)
				{
					
					osgEarth::Util::SkyNode* sky = osgEarth::Util::SkyNode::create(m_MapNode);
					//osgEarth::Util::SkyNode* sky = new osgEarth::Util::SkyNode::vre( m_MapNode->getMap());
					sky->setDateTime(osgEarth::DateTime(2013, 1, 6, 17.0));
					sky->attach(views[0]);
					root->addChild(sky);
					//if (m_ShowSkyControl)
					{
						osgEarth::Util::Controls::Control* c = osgEarth::Util::SkyControlFactory().create(sky);
						if (c)
							osgearth_sm->GetGUI()->addControl(c);
					}
				}

				// Hook up the extensions!
				for (std::vector<osg::ref_ptr<osgEarth::Extension> >::const_iterator eiter = m_MapNode->getExtensions().begin();
				eiter != m_MapNode->getExtensions().end();
					++eiter)
				{
					osgEarth::Extension* e = eiter->get();

					// Check for a View interface:
					osgEarth::ExtensionInterface<osg::View>* viewIF = osgEarth::ExtensionInterface<osg::View>::get(e);
					if (viewIF)
						viewIF->connect(views[0]);

					// Check for a Control interface:
					osgEarth::ExtensionInterface<osgEarth::Util::Control>* controlIF = osgEarth::ExtensionInterface<osgEarth::Util::Control>::get(e);
					if (controlIF)
						controlIF->connect(osgearth_sm->GetGUI());
				}


				//if (m_UseOcean)
				{
					/*	osgEarth::Util::OceanNode* ocean = osgEarth::Util::OceanNode::create(osgEarth::Util::OceanOptions(oceanConf), m_MapNode);
					if ( ocean )
					{
					// if there's a sky, we want to ocean under it
					osg::Group* parent = osgEarth::findTopMostNodeOfType<osgEarth::Util::SkyNode>(root);
					if ( !parent ) parent = root;
					parent->addChild( ocean );

					osgEarth::Util::Controls::Control* c = osgEarth::Util::OceanControlFactory().create(ocean);
					if ( c )
					mainContainer->addControl(c);
					}
					*/

					/*osgEarth::Util::OceanSurfaceNode* ocean = new  osgEarth::Util::OceanSurfaceNode( m_MapNode, oceanConf );
					if ( ocean )
					{
					root->addChild( ocean );
					if(m_ShowOceanControl)
					{
					osgEarth::Util::Controls::Control* c = osgEarth::Util::OceanControlFactory().create(ocean, views[0]);
					if ( c )
					mainContainer->addControl(c);
					}
					}*/
				}
				//m_MapNode->addCullCallback( new osgEarth::Util::AutoClipPlaneCullCallback(m_MapNode) );
				//root->addChild(canvas);
			}
		}


	void OSGEarthMapComponent::SceneManagerTick(double delta_time)
	{
		
	}
	
}

