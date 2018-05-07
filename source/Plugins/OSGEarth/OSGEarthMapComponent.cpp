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
#include <osgEarth/NodeUtils>
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

	class OSGEarthMapEnumerationMetaData : public EnumerationPropertyMetaData
	{
	public:
		OSGEarthMapEnumerationMetaData(const std::string &annotation, PropertyFlags flags, std::string res_group = "") : EnumerationPropertyMetaData(annotation, flags, false),
			m_ResourceGroup(res_group)
		{

		}
		virtual std::vector<std::string> GetEnumeration(BaseReflectionObjectPtr object) const
		{
			std::vector<std::string> content;
			ResourceManagerPtr rm = GASS::SimEngine::Get().GetResourceManager();
			ResourceGroupVector groups = rm->GetResourceGroups();
			std::vector<std::string> values;
			for (size_t i = 0; i < groups.size(); i++)
			{
				ResourceGroupPtr group = groups[i];
				if (m_ResourceGroup == "" || group->GetName() == m_ResourceGroup)
				{
					ResourceVector res_vec;
					group->GetResourcesByType(res_vec, "MAP");
					for (size_t j = 0; j < res_vec.size(); j++)
					{
						content.push_back(res_vec[j]->Name());
					}
				}
			}
			return content;
		}
	private:
		std::string m_ResourceGroup;
	};

	OSGEarthMapComponent::OSGEarthMapComponent() : m_Initlized(false)
	{

	}

	OSGEarthMapComponent::~OSGEarthMapComponent()
	{

	}

	void OSGEarthMapComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("OSGEarthMapComponent", new Creator<OSGEarthMapComponent, Component>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("Component used to OSGEarth map", OF_VISIBLE)));
		RegisterProperty<ResourceHandle>("EarthFile", &OSGEarthMapComponent::GetEarthFile, &OSGEarthMapComponent::SetEarthFile,
			EnumerationPropertyMetaDataPtr(new OSGEarthMapEnumerationMetaData("OSGEarth map file", PF_VISIBLE)));
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

	void OSGEarthMapComponent::OnDelete()
	{
		Shutdown();
	}

	void OSGEarthMapComponent::Shutdown()
	{
		if (m_Initlized && m_MapNode)
		{
			OSGEarthSceneManagerPtr osgearth_sm = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OSGEarthSceneManager>();
			osgEarth::Util::EarthManipulator* manip = osgearth_sm->GetManipulator().get();
			manip->setNode(NULL);

			// disconnect extensions
			osgViewer::ViewerBase::Views views;
			IOSGGraphicsSystemPtr osg_sys = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<IOSGGraphicsSystem>();
			osg_sys->GetViewer()->getViews(views);

			for (std::vector<osg::ref_ptr<osgEarth::Extension> >::const_iterator eiter = m_MapNode->getExtensions().begin();
				eiter != m_MapNode->getExtensions().end();
				++eiter)
			{
				osgEarth::Extension* e = eiter->get();

				// Check for a View interface:
				osgEarth::ExtensionInterface<osg::View>* viewIF = osgEarth::ExtensionInterface<osg::View>::get(e);
				if (viewIF)
					viewIF->disconnect(views[0]);

				// Check for a Control interface:
				osgEarth::ExtensionInterface<osgEarth::Util::Control>* controlIF = osgEarth::ExtensionInterface<osgEarth::Util::Control>::get(e);
				if (controlIF)
					controlIF->disconnect(osgearth_sm->GetGUI());
			}

			if (m_FogEffect)
				m_FogEffect->detach(m_MapNode->getStateSet());
			//remove map from scene

			osg::Group* parent = m_TopNode->getParent(0);
			if (parent)
				parent->removeChild(m_TopNode);

			m_MapNode.release();
			m_TopNode.release();

			//release
			m_Lighting.release();
			m_FogEffect.release();

			osgearth_sm->SetMapNode(NULL);
		}
	}

	void OSGEarthMapComponent::SetEarthFile(const ResourceHandle &earth_file)
	{
		//Always store filename
		m_EarthFile = earth_file;

		//Stop here if we are uninitialized!
		if (!m_Initlized)
			return;

		if (earth_file.Valid())
		{
			IOSGGraphicsSceneManagerPtr osg_sm = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<IOSGGraphicsSceneManager>();
			OSGEarthSceneManagerPtr osgearth_sm = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OSGEarthSceneManager>();
			osg::ref_ptr<osg::Group> root = osg_sm->GetOSGShadowRootNode();

			std::string full_path = m_EarthFile.GetResource()->Path().GetFullPath();
			osg::Node* top_node = osgDB::readNodeFile(full_path);

			//If successfully loaded, unload current map (if present)
			if (top_node)
				Shutdown();
			else //failed to load earth file
				return;

			m_TopNode = top_node;
			m_MapNode = osgEarth::MapNode::findMapNode(top_node);
			root->addChild(top_node);

			//Set EarthManipulator to only work on map node, otherwise entire scene is used for calculating home-position etc.
			osgEarth::Util::EarthManipulator* manip = osgearth_sm->GetManipulator().get();
			if (manip)
				manip->setNode(m_MapNode);

			//if no sky is present (projected mode) but we still want get terrain lightning  
			//m_Lighting = new osgEarth::PhongLightingEffect();
			//m_Lighting->setCreateLightingUniform(false);
			//m_Lighting->attach(m_MapNode->getOrCreateStateSet());

			osgEarth::Util::SkyNode* skyNode = osgEarth::findFirstParentOfType<osgEarth::Util::SkyNode>(m_MapNode);
			if (skyNode)
			{
				//set default year/month and day to get good lighting
				osgEarth::DateTime date = skyNode->getDateTime();
				skyNode->setDateTime(osgEarth::DateTime(2017, 6, 6, 10/*date.hours()*/));
			}
			


			//Connect component with osg by adding user data, this is needed if we want to used the intersection implementated by the OSGCollisionSystem
			osg::ref_ptr<OSGNodeData> data = new OSGNodeData(shared_from_this());
			m_MapNode->setUserData(data);

			//inform OSGEarth scene manager that we have new map node
			osgearth_sm->SetMapNode(m_MapNode);

			IOSGGraphicsSystemPtr osg_sys = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<IOSGGraphicsSystem>();
			osgViewer::ViewerBase::Views views;
			osg_sys->GetViewer()->getViews(views);

			const osgEarth::Config& externals = m_MapNode->externalConfig();
			//const osgEarth::Config& skyConf = externals.child("sky");
			osgEarth::Config viewpointsConf = externals.child("viewpoints");

			// backwards-compatibility: read viewpoints at the top level:
			const osgEarth::ConfigSet& old_viewpoints = externals.children("viewpoint");
			for (osgEarth::ConfigSet::const_iterator i = old_viewpoints.begin(); i != old_viewpoints.end(); ++i)
				viewpointsConf.add(*i);

			// Hook up the extensions!
			if (true)
			{
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
			}

			if (!skyNode)
			{
				if (false)
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
			}

			if (false) //use fog
			{
				osg::ref_ptr<osg::Group> fog_root = osg_sm->GetOSGRootNode();
				osg::StateSet* state = fog_root->getOrCreateStateSet();
				osg::Fog* fog = (osg::Fog *) state->getAttribute(osg::StateAttribute::FOG);
				if (fog)
				{
					if (!fog->getUpdateCallback())
						fog->setUpdateCallback(new osgEarth::Util::FogCallback());
					m_FogEffect = new osgEarth::Util::FogEffect;
					m_FogEffect->attach(m_MapNode->getOrCreateStateSet());
				}
			}

			
		

			/*if (true)
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
			}*/

			//attach fog if present possible
			
			//if (m_UseOcean)
			//	{
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
			//}
			//m_MapNode->addCullCallback( new osgEarth::Util::AutoClipPlaneCullCallback(m_MapNode) );
		}
	}


	void OSGEarthMapComponent::SceneManagerTick(double delta_time)
	{

	}

}

