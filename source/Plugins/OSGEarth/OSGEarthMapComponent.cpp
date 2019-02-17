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
#include "OSGEarthMapComponent.h"
#include "OSGEarthSceneManager.h"
#include "Plugins/OSG/IOSGGraphicsSceneManager.h"
#include "Plugins/OSG/IOSGGraphicsSystem.h"
#include "Plugins/OSG/OSGNodeMasks.h"
#include "Plugins/OSG/OSGConvert.h"
#include "Plugins/OSG/IOSGGraphicsSceneManager.h"
#include "Plugins/OSG/IOSGNode.h"
#include "Plugins/OSG/OSGNodeData.h"
#include "Sim/GASSBaseSceneManager.h"
#include "Sim/Interface/GASSIGraphicsSceneManager.h"
#include "Sim/Messages/GASSGraphicsSceneMessages.h"

namespace GASS
{

	struct OEMapListenerProxy : public osgEarth::MapCallback
	{
		OEMapListenerProxy(OSGEarthMapComponent* map_comp) : m_MapComponent(map_comp){}
		void onMapModelChanged(const osgEarth::MapModelChange& change)
		{
			m_MapComponent->onMapModelChanged(change);
		}
		OSGEarthMapComponent* m_MapComponent;
	};

	class OETerrainCallbackProxy : public osgEarth::TerrainCallback
	{
	public:
		OETerrainCallbackProxy(OSGEarthMapComponent* map_comp) : m_MapComponent(map_comp)
		{

		}

		void onTileAdded(
			const osgEarth::TileKey&          key,
			osg::Node*              graph,
			osgEarth::TerrainCallbackContext& context)
		{
			m_MapComponent->onTileAdded(key, graph, context);
		}

		OSGEarthMapComponent* m_MapComponent;
	};

	class OSGEarthMapLayer : public IMapLayer
	{
	public:
		OSGEarthMapLayer(osgEarth::VisibleLayer* layer) : m_Layer(layer)
		{
			
		}

		std::string GetName() const override
		{
			return m_Layer->getName();
		}
		void SetName(const std::string& name) override
		{
			m_Layer->setName(name);
		}

		bool GetVisible() const override
		{
			return m_Layer->getVisible();
		}

		void SetVisible(bool value) override
		{
			m_Layer->setVisible(value);
		}

		bool GetEnabled() const override
		{
			return m_Layer->getEnabled();
		}

		void SetEnabled(bool value) override
		{
			m_Layer->setEnabled(value);
		}

		MapLayerType GetType() const override
		{
			MapLayerType layer_type = MLT_IMAGE;
			if (dynamic_cast<osgEarth::ImageLayer*>(m_Layer))
				layer_type = MLT_IMAGE;
			else if (dynamic_cast<osgEarth::ElevationLayer*>(m_Layer))
				layer_type = MLT_ELEVATION;
			else if (dynamic_cast<osgEarth::ModelLayer*>(m_Layer))
				layer_type = MLT_MODEL;
			return layer_type;
		}

		int GetUID() const override
		{
			return m_Layer->getUID();
		}
	private:
		osgEarth::VisibleLayer* m_Layer;
	};


	class OSGEarthMapEnumerationMetaData : public EnumerationPropertyMetaData
	{
	public:
		OSGEarthMapEnumerationMetaData(const std::string &annotation, PropertyFlags flags, std::string res_group = "") : EnumerationPropertyMetaData(annotation, flags, false),
			m_ResourceGroup(res_group)
		{

		}
		std::vector<std::string> GetEnumeration(BaseReflectionObjectPtr object) const override
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

	class OSGEarthViewpointEnumerationMetaData : public EnumerationPropertyMetaData
	{
	public:
		OSGEarthViewpointEnumerationMetaData(const std::string &annotation, PropertyFlags flags) : EnumerationPropertyMetaData(annotation, flags, false)
		{

		}
		std::vector<std::string> GetEnumeration(BaseReflectionObjectPtr object) const override
		{
			OSGEarthMapComponentPtr map_comp = GASS_DYNAMIC_PTR_CAST<OSGEarthMapComponent>(object);
			return map_comp->GetViewpointNames();
		}
	private:
	};


	class OSGEarthLayerEnumerationMetaData : public EnumerationPropertyMetaData
	{
	public:
		OSGEarthLayerEnumerationMetaData(const std::string &annotation, PropertyFlags flags) : EnumerationPropertyMetaData(annotation, flags, true)
		{

		}
		std::vector<std::string> GetEnumeration(BaseReflectionObjectPtr object) const override
		{
			OSGEarthMapComponentPtr map_comp = GASS_DYNAMIC_PTR_CAST<OSGEarthMapComponent>(object);

			std::vector<std::string> layer_names;
			const MapLayers &layers = map_comp->GetMapLayers();
			for (size_t i = 0; i < layers.size(); i++)
			{
				layer_names.push_back(layers[i]->GetName());
			}
			return layer_names;
		}
	private:
	};


	OSGEarthMapComponent::OSGEarthMapComponent() : m_Initlized(false),
		m_Hour(10),
		m_SkyNode(NULL),
		m_UseAutoClipPlane(true),
		m_OESceneManager(nullptr),
		m_TerrainCallbackProxy(new OETerrainCallbackProxy(this)),
		m_TerrainChangedLastFrame(false)
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
		RegisterProperty<std::string>("Viewpoint", &OSGEarthMapComponent::GetViewpointName, &OSGEarthMapComponent::SetViewpointByName,
			EnumerationPropertyMetaDataPtr(new OSGEarthViewpointEnumerationMetaData("Set Viewpoint", PF_VISIBLE)));

		RegisterProperty<double>("TimeOfDay", &OSGEarthMapComponent::GetTimeOfDay, &OSGEarthMapComponent::SetTimeOfDay,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Time of day", PF_VISIBLE | PF_EDITABLE)));

		RegisterProperty<float>("MinimumAmbient", &OSGEarthMapComponent::GetMinimumAmbient, &OSGEarthMapComponent::SetMinimumAmbient,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Minimum ambient sky light", PF_VISIBLE | PF_EDITABLE)));

		RegisterProperty<bool>("SkyLighting", &OSGEarthMapComponent::GetSkyLighting, &OSGEarthMapComponent::SetSkyLighting,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Enable/disable sky light", PF_VISIBLE | PF_EDITABLE)));

		RegisterProperty<std::vector<std::string> >("VisibleMapLayers", &OSGEarthMapComponent::GetVisibleMapLayers, &OSGEarthMapComponent::SetVisibleMapLayers,
			EnumerationPropertyMetaDataPtr(new OSGEarthLayerEnumerationMetaData("Map Layers", PF_VISIBLE)));
	}

	void OSGEarthMapComponent::OnInitialize()
	{
		RegisterForPreUpdate<IGraphicsSceneManager>();
		if (OSGEarthSceneManagerPtr osgearth_sm = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OSGEarthSceneManager>())
			m_OESceneManager = osgearth_sm.get();
		else
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to find OSGEarthSceneManager", "OSGEarthMapComponent::OnInitialize");
		
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
			if(m_MapNode->getTerrain())
				m_MapNode->getTerrain()->removeTerrainCallback(m_TerrainCallbackProxy);
			// disconnect extensions
			osgViewer::ViewerBase::Views views;
			IOSGGraphicsSystemPtr osg_sys = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<IOSGGraphicsSystem>();
			GASSAssert(osg_sys, "Failed to find OSGGraphicsSystem in OSGEarthMapComponent::Shutdown");
			
			osg_sys->GetViewer()->getViews(views);


			//shutdown extensions
			for (std::vector<osg::ref_ptr<osgEarth::Extension> >::const_iterator eiter = m_MapNode->getExtensions().begin();
				eiter != m_MapNode->getExtensions().end();
				++eiter)
			{
				osgEarth::Extension* e = eiter->get();

				// Check for a View interface:
				osgEarth::ExtensionInterface<osg::View>* viewIF = osgEarth::ExtensionInterface<osg::View>::get(e);
				if (viewIF &&  views.size() > 0)
					viewIF->disconnect(views[0]);

				// Check for a Control interface:
				osgEarth::ExtensionInterface<osgEarth::Util::Control>* controlIF = osgEarth::ExtensionInterface<osgEarth::Util::Control>::get(e);
				if (controlIF)
					controlIF->disconnect(m_OESceneManager->GetGUI());
			}

			//remove fog effect
			if (m_FogEffect)
				m_FogEffect->detach(m_MapNode->getStateSet());

			if (m_AutoClipCB)
				views[0]->getCamera()->removeCullCallback(m_AutoClipCB);

			//remove map from scene
			osg::Group* parent = m_TopNode->getParent(0);
			if (parent)
				parent->removeChild(m_TopNode);


			m_MapNode.release();
			m_TopNode.release();

			//release
			m_Lighting.release();
			m_FogEffect.release();
			m_AutoClipCB.release();

			m_OESceneManager->SetMapNode(NULL);
		}
	}

	void OSGEarthMapComponent::onTileAdded(
		const osgEarth::TileKey&          key,
		osg::Node*              graph,
		osgEarth::TerrainCallbackContext& context)
	{
		m_TerrainChangedLastFrame = true;
	}

	void OSGEarthMapComponent::onMapModelChanged(const osgEarth::MapModelChange& change)
	{
		const osgEarth::ElevationLayer* elevationLayer = change.getElevationLayer();
		if (elevationLayer)
		{
			m_OESceneManager->OnElevationChanged();
			GetSceneObject()->GetScene()->PostMessage(GASS_MAKE_SHARED<TerrainChangedEvent>());
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
			GASSAssert(osg_sm, "Failed to find OSGGraphicsSceneManager in OSGEarthMapComponent::SetEarthFile");
			//OSGEarthSceneManagerPtr osgearth_sm = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OSGEarthSceneManager>();
			//GASSAssert(osgearth_sm, "Failed to find OSGEarthSceneManager in OSGEarthMapComponent::SetEarthFile");

			const std::string full_path = m_EarthFile.GetResource()->Path().GetFullPath();

			//read earth file
			osg::Node* top_node = osgDB::readNodeFile(full_path);
			if (!top_node)
			{
				//failed to load earth file!
				return;
			}

			//If successfully loaded, unload current map (if present)
			Shutdown();

			//Save top node, to be used during shutdown
			m_TopNode = top_node;
			m_MapNode = osgEarth::MapNode::findMapNode(top_node);
			GASSAssert(m_MapNode, "Failed to find mapnode in OSGEarthMapComponent::SetEarthFile");
		
			m_MapNode->getMap()->addMapCallback(new OEMapListenerProxy(this));

			_SetupNodeMasks();

			if (m_MapNode->getTerrain())
			{
				m_MapNode->getTerrain()->addTerrainCallback(m_TerrainCallbackProxy);
			}

			osg::ref_ptr<osg::Group> root = osg_sm->GetOSGShadowRootNode();
			root->addChild(top_node);

			//if no sky is present (projected mode) but we still want get terrain lightning  
			//m_Lighting = new osgEarth::PhongLightingEffect();
			//m_Lighting->setCreateLightingUniform(false);
			//m_Lighting->attach(m_MapNode->getOrCreateStateSet());

			m_SkyNode = osgEarth::findFirstParentOfType<osgEarth::Util::SkyNode>(m_MapNode);
			if (m_SkyNode)
			{
				//set default year/month and day to get good lighting
				m_SkyNode->setDateTime(osgEarth::DateTime(2017, 6, 6, m_Hour));
			}

			//Connect component with osg by adding user data, this is needed if we want to used the intersection implementated by the OSGCollisionSystem
			osg::ref_ptr<OSGNodeData> data = new OSGNodeData(shared_from_this());
			m_MapNode->setUserData(data);

			//inform OSGEarth scene manager that we have new map node
			m_OESceneManager->SetMapNode(m_MapNode);

			//read viewpoints from earth file and store them in m_Viewpoints vector 
			{
				m_Viewpoints.clear();
				const osgEarth::Config& externals = m_MapNode->externalConfig();

				osgEarth::Config viewpointsConf = externals.child("viewpoints");

				if (viewpointsConf.children("viewpoint").size() == 0)
					viewpointsConf = m_MapNode->getConfig().child("viewpoints");

				// backwards-compatibility: read viewpoints at the top level:
				const osgEarth::ConfigSet& old_viewpoints = externals.children("viewpoint");
				for (osgEarth::ConfigSet::const_iterator i = old_viewpoints.begin(); i != old_viewpoints.end(); ++i)
					viewpointsConf.add(*i);

				const osgEarth::ConfigSet& children = viewpointsConf.children("viewpoint");
				if (children.size() > 0)
				{
					for (osgEarth::ConfigSet::const_iterator i = children.begin(); i != children.end(); ++i)
					{
						m_Viewpoints.push_back(osgEarth::Viewpoint(*i));
					}
				}
			}

			IOSGGraphicsSystemPtr osg_sys = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<IOSGGraphicsSystem>();
			osgViewer::ViewerBase::Views all_views;
			osg_sys->GetViewer()->getViews(all_views);

			if (all_views.size() == 0)
			{
				return;
			}
			osgViewer::View* view = all_views[0];
		
			// Hook up the extensions!
			{
				for (std::vector<osg::ref_ptr<osgEarth::Extension> >::const_iterator eiter = m_MapNode->getExtensions().begin();
					eiter != m_MapNode->getExtensions().end();
					++eiter)
				{
					osgEarth::Extension* e = eiter->get();

					// Check for a View interface:
					osgEarth::ExtensionInterface<osg::View>* viewIF = osgEarth::ExtensionInterface<osg::View>::get(e);
					if (viewIF)
						viewIF->connect(view);

					// Check for a Control interface:
					osgEarth::ExtensionInterface<osgEarth::Util::Control>* controlIF = osgEarth::ExtensionInterface<osgEarth::Util::Control>::get(e);
					if (controlIF)
						controlIF->connect(m_OESceneManager->GetGUI());
				}
			}
			
			if (!m_SkyNode)
			{
				/*if (false)
				{
					m_SkyNode = osgEarth::Util::SkyNode::create(m_MapNode);
					//osgEarth::Util::SkyNode* sky = new osgEarth::Util::SkyNode::vre( m_MapNode->getMap());
					m_SkyNode->setDateTime(osgEarth::DateTime(2013, 1, 6, m_Time));
					m_SkyNode->attach(view);
					root->addChild(m_SkyNode);
					//if (m_ShowSkyControl)
					{
						osgEarth::Util::Controls::Control* c = osgEarth::Util::SkyControlFactory().create(m_SkyNode);
						if (c)
							osgearth_sm->GetGUI()->addControl(c);
					}
				}*/
			}
			
			//Restore setLightingMode to sky light to get osgEarth lighting to be reflected in rest of scene
			view->setLightingMode(osg::View::SKY_LIGHT);

			if (m_UseAutoClipPlane)
			{
				//Setup AutoClipPlaneCullCallback to handel near/far clipping issues
				m_AutoClipCB = new osgEarth::Util::AutoClipPlaneCullCallback(m_MapNode);
				m_AutoClipCB->setMinNearFarRatio(0.00000001); //NearFarRatio at zero altitude
				m_AutoClipCB->setMaxNearFarRatio(0.00005); //NearFarRatio at height threshold 
				m_AutoClipCB->setHeightThreshold(3000); // above this height we get MaxNearFarRatio
				view->getCamera()->addCullCallback(m_AutoClipCB);
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

			_UpdateMapLayers();

			GetSceneObject()->PostEvent(GeometryChangedEventPtr(new GeometryChangedEvent(GASS_DYNAMIC_PTR_CAST<IGeometryComponent>(shared_from_this()))));
			GetSceneObject()->GetScene()->PostMessage(GASS_MAKE_SHARED<TerrainChangedEvent>());
			
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
		}
	}

	void OSGEarthMapComponent::_SetupNodeMasks()
	{
		osgEarth::ModelLayerVector modelLayers;
		m_MapNode->getMap()->getLayers(modelLayers);
		OSGConvert::SetOSGNodeMask(GEOMETRY_FLAG_GROUND, m_MapNode->getTerrain()->getGraph());
		
		for (unsigned i = 0; i < modelLayers.size(); ++i)
		{
			double model_elevation = 0;
			if (modelLayers[i]->getNode())
			{
				const GeometryFlags flags = modelLayers[i]->options().terrainPatch() == true ? GEOMETRY_FLAG_GROUND : GEOMETRY_FLAG_STATIC_OBJECT;
				OSGConvert::SetOSGNodeMask(flags, modelLayers[i]->getNode());
			}
		}
	}

	std::vector<std::string> OSGEarthMapComponent::GetVisibleMapLayers() const
	{
		std::vector<std::string> layer_names;
		// the active map layers:
		if (m_Initlized)
		{
			
			for (size_t i = 0;  i < m_MapLayers.size(); i++)
			{
				if(m_MapLayers[i]->GetVisible())
					layer_names.push_back(m_MapLayers[i]->GetName());
			}
		}
		return layer_names;
	}

	void OSGEarthMapComponent::SetVisibleMapLayers(const std::vector<std::string> &layer_names)
	{
		if (m_Initlized)
		{
			
			for (size_t i = 0; i < m_MapLayers.size(); i++)
			{
				bool visible = false;
				for (size_t j = 0; j < layer_names.size(); j++)
				{
					if (m_MapLayers[i]->GetName() == layer_names[j])
						visible = true;
				}
				m_MapLayers[i]->SetVisible(visible);
			}
		}
	}

	void OSGEarthMapComponent::SceneManagerTick(double /*delta_time*/)
	{
		if (m_TerrainChangedLastFrame)
		{
			//GetSceneObject()->GetScene()->PostMessage(GASS_MAKE_SHARED<TerrainChangedEvent>());
			m_TerrainChangedLastFrame = false;
		}
	}

	void OSGEarthMapComponent::SetTimeOfDay(double hour)
	{
		m_Hour = hour;
		if (m_SkyNode)
		{
			m_SkyNode->setDateTime(osgEarth::DateTime(2017, 6, 6, m_Hour));
		}
	}

	void OSGEarthMapComponent::SetMinimumAmbient(float value)
	{
		if (m_SkyNode)
		{
			m_SkyNode->setMinimumAmbient(osg::Vec4f(value,value,value,1));
		}
	}

	float OSGEarthMapComponent::GetMinimumAmbient() const
	{
		float value = 0;
		if (m_SkyNode)
		{
			value = m_SkyNode->getMinimumAmbient().x();
		}
		return value;
	}

	void OSGEarthMapComponent::SetSkyLighting(bool value)
	{
		if (m_SkyNode)
		{
			m_SkyNode->setLighting(value);
		}
	}

	bool OSGEarthMapComponent::GetSkyLighting() const
	{
		bool value = false;
		if (m_SkyNode)
		{
			value = m_SkyNode->getLighting();
		}
		return value;
	}

	
	std::vector<std::string> OSGEarthMapComponent::GetViewpointNames() const
	{
		std::vector<std::string> names;
		for (size_t i = 0; i <  m_Viewpoints.size(); i++)
		{
			names.push_back(m_Viewpoints[i].name().value());
		}
		return names;
	}

	void OSGEarthMapComponent::SetViewpointByName(const std::string &name)
	{
		if (!m_Initlized)
			return;
		for (size_t i = 0; i < m_Viewpoints.size(); i++)
		{
			if( m_Viewpoints[i].name().value() == name)
			{
				osgEarth::Util::EarthManipulator* manip = m_OESceneManager->GetManipulator().get();
				if(manip)
					manip->setViewpoint(m_Viewpoints[i], 2.0);
				return; //done
			}
		}
	}

	const MapLayers& OSGEarthMapComponent::GetMapLayers() const
	{
		return m_MapLayers;
	}

	void OSGEarthMapComponent::_UpdateMapLayers()
	{
		m_MapLayers.clear();
		// the active map layers:
		if (m_Initlized)
		{
			osgEarth::LayerVector oe_layers;
			m_MapNode->getMap()->getLayers(oe_layers);
			for (size_t i = 0; i < oe_layers.size(); i++)
			{
				osgEarth::Layer* oe_layer = oe_layers[i].get();
				osgEarth::VisibleLayer* visibleLayer = dynamic_cast<osgEarth::VisibleLayer*>(oe_layer);
				// only return layers that derive from VisibleLayer
				if (visibleLayer)
				{
					m_MapLayers.emplace_back(std::make_unique<OSGEarthMapLayer>(OSGEarthMapLayer(visibleLayer)));
				}
			}
		}
	}
}

