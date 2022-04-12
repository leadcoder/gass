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
#include "Core/Utils/GASSSystem.h"
#include <memory>
#include <osgDB/FileUtils>

namespace GASS
{

	struct OEMapListenerProxy : public osgEarth::MapCallback
	{
		OEMapListenerProxy(OSGEarthMapComponent* map_comp) : m_MapComponent(map_comp) {}
		void onMapModelChanged(const osgEarth::MapModelChange& change) override
		{
			m_MapComponent->OnMapModelChanged(change);
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
			const osgEarth::TileKey& key,
			osg::Node* graph,
			osgEarth::TerrainCallbackContext& context) override
		{
			m_MapComponent->OnTileAdded(key, graph, context);
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
			return m_Layer->isOpen();
			//return m_Layer->getEnabled();
		}

		void SetEnabled(bool value) override
		{
			m_Layer->setEnabled(value);
			if (value)
			{
				if (!m_Layer->isOpen())
					m_Layer->open();
			}
			else
			{
				if (m_Layer->isOpen())
					m_Layer->close();
			}
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
			else if (dynamic_cast<osgEarth::FeatureModelLayer*>(m_Layer))
				layer_type = MLT_FEATURE_MODEL;
			return layer_type;
		}

		int GetUID() const override
		{
			return m_Layer->getUID();
		}
	private:
		osgEarth::VisibleLayer* m_Layer;
	};

	std::vector<ResourceHandle> GetAllEarthFiles()
	{
		std::vector<ResourceHandle> content;
		ResourceManagerPtr rm = GASS::SimEngine::Get().GetResourceManager();
		ResourceGroupVector groups = rm->GetResourceGroups();
		std::vector<std::string> values;
		for (size_t i = 0; i < groups.size(); i++)
		{
			ResourceGroupPtr group = groups[i];
			ResourceVector res_vec;
			group->GetResourcesByType(res_vec, "MAP");
			for (size_t j = 0; j < res_vec.size(); j++)
			{
				content.emplace_back(res_vec[j]->Name());
			}
		}
		return content;
	}

	OSGEarthMapComponent::OSGEarthMapComponent() : 
		m_TerrainCallbackProxy(new OETerrainCallbackProxy(this))
		
	{
		m_ShadowRanges = { 0.0f, 100.0f, 250.0f };
	}

	OSGEarthMapComponent::~OSGEarthMapComponent()
	{

	}

	void OSGEarthMapComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<OSGEarthMapComponent>();
		GetClassRTTI()->SetMetaData(std::make_shared<ClassMetaData>("Component used to OSGEarth map", OF_VISIBLE));
		auto earth_file_prop = RegisterGetSet("EarthFile", &OSGEarthMapComponent::GetEarthFile, &OSGEarthMapComponent::SetEarthFile, PF_VISIBLE, "OSGEarth map file");
		earth_file_prop->SetOptionsFunction(&GetAllEarthFiles);
		auto vp_prop = RegisterGetSet("Viewpoint", &OSGEarthMapComponent::GetViewpointName, &OSGEarthMapComponent::SetViewpointByName, PF_VISIBLE | PF_EDITABLE, "Set Viewpoint");
		vp_prop->SetObjectOptionsFunction(&OSGEarthMapComponent::GetViewpointNames);

		RegisterGetSet("TimeOfDay", &OSGEarthMapComponent::GetTimeOfDay, &OSGEarthMapComponent::SetTimeOfDay, PF_VISIBLE | PF_EDITABLE, "Time of day");
		RegisterGetSet("MinimumAmbient", &OSGEarthMapComponent::GetMinimumAmbient, &OSGEarthMapComponent::SetMinimumAmbient, PF_VISIBLE | PF_EDITABLE, "Minimum ambient sky light");
		RegisterGetSet("SkyExposure", &OSGEarthMapComponent::GetSkyExposure, &OSGEarthMapComponent::SetSkyExposure, PF_VISIBLE | PF_EDITABLE, "Sky light exposure");
		RegisterGetSet("SkyContrast", &OSGEarthMapComponent::GetSkyContrast, &OSGEarthMapComponent::SetSkyContrast, PF_VISIBLE | PF_EDITABLE, "Sky light contrast");
		RegisterGetSet("SkyAmbientBoost", &OSGEarthMapComponent::GetSkyAmbientBoost, &OSGEarthMapComponent::SetSkyAmbientBoost, PF_VISIBLE | PF_EDITABLE, "Sky light ambient boost fasctor, (ONeal only)");
		RegisterGetSet("SkyLighting", &OSGEarthMapComponent::GetSkyLighting, &OSGEarthMapComponent::SetSkyLighting, PF_VISIBLE | PF_EDITABLE, "Enable/disable sky light");
		RegisterGetSet("SkyHazeCutoff", &OSGEarthMapComponent::GetSkyHazeCutoff, &OSGEarthMapComponent::SetSkyHazeCutoff, PF_VISIBLE | PF_EDITABLE, "SkyHazeCutoff");
		RegisterGetSet("SkyHazeStrength", &OSGEarthMapComponent::GetSkyHazeStrength, &OSGEarthMapComponent::SetSkyHazeStrength, PF_VISIBLE | PF_EDITABLE, "SkyHazeStrength");

		RegisterGetSet("ShadowEnabled", &OSGEarthMapComponent::GetShadowEnabled, &OSGEarthMapComponent::SetShadowEnabled, PF_VISIBLE | PF_EDITABLE, "Enable/disable shadows");
		RegisterGetSet("ShadowBlur", &OSGEarthMapComponent::GetShadowBlur, &OSGEarthMapComponent::SetShadowBlur, PF_VISIBLE | PF_EDITABLE, "Shadow blur factor");
		RegisterGetSet("ShadowRanges", &OSGEarthMapComponent::GetShadowRanges, &OSGEarthMapComponent::SetShadowRanges, PF_VISIBLE | PF_EDITABLE, "Shadow ranges");
		RegisterGetSet("ShadowColor", &OSGEarthMapComponent::GetShadowColor, &OSGEarthMapComponent::SetShadowColor, PF_VISIBLE | PF_EDITABLE, "Shadow Color");

		auto layers_prop = RegisterGetSet("VisibleMapLayers", &OSGEarthMapComponent::GetVisibleMapLayers, &OSGEarthMapComponent::SetVisibleMapLayers, PF_VISIBLE, "Map Layers");
		layers_prop->SetObjectOptionsFunction(&OSGEarthMapComponent::GetMapLayerNames);

		RegisterMember("AddSky", &OSGEarthMapComponent::m_AddSky, PF_VISIBLE , "Add sky light");
		RegisterMember("IsRoot", &OSGEarthMapComponent::m_IsRoot, PF_VISIBLE | PF_EDITABLE, "Objects should be placed under map node");
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
			if (m_MapNode->getTerrain())
				m_MapNode->getTerrain()->removeTerrainCallback(m_TerrainCallbackProxy);
			// disconnect extensions
			osgViewer::ViewerBase::Views views;
			IOSGGraphicsSystemPtr osg_sys = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<IOSGGraphicsSystem>();
			GASSAssert(osg_sys, "Failed to find OSGGraphicsSystem in OSGEarthMapComponent::Shutdown");

			osg_sys->GetViewer()->getViews(views);


			//shutdown extensions
			for (auto eiter = m_MapNode->getExtensions().begin();
				eiter != m_MapNode->getExtensions().end();
				++eiter)
			{
				osgEarth::Extension* e = eiter->get();

				// Check for a View interface:
				osgEarth::ExtensionInterface<osg::View>* view_if = osgEarth::ExtensionInterface<osg::View>::get(e);
				if (view_if && views.size() > 0)
					view_if->disconnect(views[0]);

				// Check for a Control interface:
				osgEarth::ExtensionInterface<osgEarth::Util::Control>* control_if = osgEarth::ExtensionInterface<osgEarth::Util::Control>::get(e);
				if (control_if)
					control_if->disconnect(m_OESceneManager->GetGUI());
			}

#ifdef HAS_FOG
			//remove fog effect
			if (m_FogEffect)
				m_FogEffect->detach(m_MapNode->getStateSet());
#endif
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
#ifdef HAS_FOG
			m_FogEffect.release();
#endif
			m_AutoClipCB.release();

			m_OESceneManager->SetMapNode(nullptr);
		}
	}

	void OSGEarthMapComponent::OnTileAdded(
		const osgEarth::TileKey&          /*key*/,
		osg::Node*              /*graph*/,
		osgEarth::TerrainCallbackContext& /*context*/)
	{
		m_TerrainChangedLastFrame = true;
	}

	void OSGEarthMapComponent::OnMapModelChanged(const osgEarth::MapModelChange& change)
	{
		const osgEarth::ElevationLayer* elevation_layer = change.getElevationLayer();
		if (elevation_layer)
		{
			m_OESceneManager->OnElevationChanged();
			GetSceneObject()->GetScene()->PostMessage(GASS_MAKE_SHARED<TerrainChangedEvent>());
		}
	}

	void OSGEarthMapComponent::SetEarthFile(const ResourceHandle& earth_file)
	{
		//Always store filename
		m_EarthFile = earth_file;

		//Stop here if we are uninitialized!
		if (!m_Initlized)
			return;

		IOSGGraphicsSceneManagerPtr osg_sm = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<IOSGGraphicsSceneManager>();
		GASSAssert(osg_sm, "Failed to find OSGGraphicsSceneManager in OSGEarthMapComponent::SetEarthFile");

		const std::string full_path = [&]
		{
			if (SimEngine::Get().GetResourceManager()->HasResource(earth_file.Name()))
				return m_EarthFile.GetResource()->Path().GetFullPath();

			if (osgDB::fileExists(m_EarthFile.Name()) && osgDB::isAbsolutePath(m_EarthFile.Name()))
			{
				return m_EarthFile.Name();
			}
			return std::string("");
		}();

		if (full_path == "")
		{
			GASS_LOG(LINFO) << "OSGEarthMapComponent::SetEarthFile, could not load:" << m_EarthFile.Name();
			return;
		}
		
		//Add to path to let oe find shaders
		osgDB::Registry::instance()->getDataFilePathList().push_back(osgDB::getFilePath(full_path));


		//set cache path

		std::string temp_str = System::GetEnvVar("OE_CACHE_DATA_PATH");
		if (!temp_str.empty())
		{
			osgEarth::Drivers::FileSystemCacheOptions osg_earth_cache_options;
			osg_earth_cache_options.rootPath() = temp_str;//"c:/OSGEarthCache/test_cache";
			osgEarth::Registry::instance()->setDefaultCache(osgEarth::CacheFactory::create(osg_earth_cache_options));
			osgEarth::Registry::instance()->setDefaultCachePolicy(osgEarth::CachePolicy::USAGE_READ_WRITE);
		}

		//read earth file
		osg::Node* read_node = osgDB::readNodeFile(full_path);
		if (!read_node)
		{
			//failed to load earth file!
			return;
		}

		//If successfully loaded, unload current map (if present)
		Shutdown();

		m_MapNode = osgEarth::MapNode::findMapNode(read_node);
		GASSAssert(m_MapNode, "Failed to find mapnode in OSGEarthMapComponent::SetEarthFile");

		if (!m_MapNode->open())
		{
			GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Failed to open map node", "OSGEarthMapComponent::OnInitialize");
		}

		
		m_MapNode->getMap()->addMapCallback(new OEMapListenerProxy(this));

		SetupNodeMasks();

		if (m_MapNode->getTerrain())
		{
			m_MapNode->getTerrain()->addTerrainCallback(m_TerrainCallbackProxy);
		}

		osg::ref_ptr<osg::Group> root = osg_sm->GetOSGShadowRootNode();
	
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

			osgEarth::Config viewpoints_conf = externals.child("viewpoints");

			if (viewpoints_conf.children("viewpoint").size() == 0)
				viewpoints_conf = m_MapNode->getConfig().child("viewpoints");

			// backwards-compatibility: read viewpoints at the top level:
			const osgEarth::ConfigSet& old_viewpoints = externals.children("viewpoint");
			for (auto i = old_viewpoints.begin(); i != old_viewpoints.end(); ++i)
				viewpoints_conf.add(*i);

			const osgEarth::ConfigSet& children = viewpoints_conf.children("viewpoint");
			if (children.size() > 0)
			{
				for (auto i = children.begin(); i != children.end(); ++i)
				{
					m_Viewpoints.emplace_back(*i);
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
			for (auto eiter = m_MapNode->getExtensions().begin();
				eiter != m_MapNode->getExtensions().end();
				++eiter)
			{
				osgEarth::Extension* e = eiter->get();

				// Check for a View interface:
				osgEarth::ExtensionInterface<osg::View>* view_if = osgEarth::ExtensionInterface<osg::View>::get(e);
				if (view_if)
					view_if->connect(view);

				// Check for a Control interface:
				osgEarth::ExtensionInterface<osgEarth::Util::Control>* control_if = osgEarth::ExtensionInterface<osgEarth::Util::Control>::get(e);
				if (control_if)
					control_if->connect(m_OESceneManager->GetGUI());
			}
		}

		if (!m_SkyNode && m_AddSky)
		{
			osgEarth::SimpleSky::SimpleSkyOptions sky_options;
			sky_options.atmosphericLighting() = false;
			
			std::string ext = m_MapNode->getMapSRS()->isGeographic() ? "sky_simple" : "sky_gl";
			m_MapNode->addExtension(osgEarth::Extension::create(ext, sky_options));
			m_SkyNode = osgEarth::findFirstParentOfType<osgEarth::Util::SkyNode>(m_MapNode);
			SetTimeOfDay(m_Hour);
		}

		if (m_SkyNode) //reflect our settings
		{
			//SetSkyExposure(m_SkyExposure);
			//SetSkyContrast(m_SkyContrast);
			//SetSkyAmbientBoost(m_SkyAmbientBoost);
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

#ifdef HAS_FOG
		if (true) //use fog
		{
			osg::ref_ptr<osg::Group> fog_root = osg_sm->GetOSGRootNode();
			osg::StateSet* state = fog_root->getOrCreateStateSet();
			osg::Fog* fog = (osg::Fog*) state->getAttribute(osg::StateAttribute::FOG);
			if (fog)
			{
				if (!fog->getUpdateCallback())
					fog->setUpdateCallback(new osgEarth::Util::FogCallback());
				m_FogEffect = new osgEarth::Util::FogEffect;
				m_FogEffect->attach(m_MapNode->getOrCreateStateSet());
			}
	}
#endif
		UpdateMapLayers();

		GetSceneObject()->PostEvent(std::make_shared<GeometryChangedEvent>(GASS_DYNAMIC_PTR_CAST<IGeometryComponent>(shared_from_this())));
		GetSceneObject()->GetScene()->PostMessage(GASS_MAKE_SHARED<TerrainChangedEvent>());

		osg::Group* object_root = nullptr;
		if (m_IsRoot)
		{
			object_root = new osg::Group();
			m_MapNode->addChild(object_root);
			osg_sm->SetMapNode(object_root);
		}

		if (m_ShadowEnabled)
		{
			int unit;

			if (m_MapNode->getTerrainEngine()->getResources()->reserveTextureImageUnit(unit, "ShadowCaster"))
			{
				// default slices:
				m_ShadowCaster = new osgEarth::ShadowCaster();
				m_ShadowCaster->setRanges(m_ShadowRanges);
				m_ShadowCaster->setBlurFactor(m_ShadowBlur);
				m_ShadowCaster->setShadowColor(m_ShadowColor);
				m_ShadowCaster->setTextureImageUnit(unit);
				m_ShadowCaster->setLight(view->getLight());
				m_ShadowCaster->getShadowCastingGroup()->addChild(m_MapNode->getLayerNodeGroup());
				m_ShadowCaster->getShadowCastingGroup()->addChild(m_MapNode->getTerrainEngine()->getNode());
				if (object_root)
					m_ShadowCaster->getShadowCastingGroup()->addChild(object_root);
				if (m_MapNode->getNumParents() > 0)
				{
					osgEarth::insertGroup(m_ShadowCaster, m_MapNode->getParent(0));
				}
				else
				{
					m_ShadowCaster->addChild(m_MapNode);
				}
			}
		}
		else
			m_ShadowCaster = nullptr;

		//Save top node, to be used during shutdown
		m_TopNode = osgEarth::findTopOfGraph(m_MapNode);
		root->addChild(m_TopNode);



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

	void OSGEarthMapComponent::SetupNodeMasks()
	{
		osgEarth::ModelLayerVector model_layers;
		m_MapNode->getMap()->getLayers(model_layers);
		if (m_MapNode->getTerrain())
			OSGConvert::SetOSGNodeMask(GEOMETRY_FLAG_GROUND, m_MapNode->getTerrain()->getGraph());

		for (unsigned i = 0; i < model_layers.size(); ++i)
		{
			if (model_layers[i]->getNode())
			{
				const GeometryFlags flags = model_layers[i]->options().terrainPatch() == true ? GEOMETRY_FLAG_GROUND : GEOMETRY_FLAG_STATIC_OBJECT;
				OSGConvert::SetOSGNodeMask(flags, model_layers[i]->getNode());
			}
		}
	}

	std::vector<std::string> OSGEarthMapComponent::GetVisibleMapLayers() const
	{
		std::vector<std::string> layer_names;
		// the active map layers:
		if (m_Initlized)
		{

			for (size_t i = 0; i < m_MapLayers.size(); i++)
			{
				if (m_MapLayers[i]->GetVisible())
					layer_names.push_back(m_MapLayers[i]->GetName());
			}
		}
		return layer_names;
	}

	std::vector<std::string> OSGEarthMapComponent::GetMapLayerNames() const
	{
		std::vector<std::string> layer_names;
		const MapLayers& layers = GetMapLayers();
		for (size_t i = 0; i < layers.size(); i++)
		{
			layer_names.push_back(layers[i]->GetName());
		}
		return layer_names;
	}

	void OSGEarthMapComponent::SetVisibleMapLayers(const std::vector<std::string>& layer_names)
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


	float OSGEarthMapComponent::GetSkyExposure() const
	{
		return m_SkyExposure;
	}

	void OSGEarthMapComponent::SetSkyExposure(float value)
	{
		m_SkyExposure = value;
		if (m_SkyNode)
		{
			m_SkyNode->getOrCreateStateSet()->getOrCreateUniform("oe_sky_exposure", osg::Uniform::FLOAT)->set(value);
		}
	}

	float OSGEarthMapComponent::GetSkyContrast() const
	{
		return m_SkyContrast;
	}

	void OSGEarthMapComponent::SetSkyContrast(float value)
	{
		m_SkyContrast = value;
		if (m_SkyNode)
		{
			m_SkyNode->getOrCreateStateSet()->getOrCreateUniform("oe_sky_contrast", osg::Uniform::FLOAT)->set(value);
		}
	}

	float OSGEarthMapComponent::GetSkyAmbientBoost() const
	{
		return m_SkyAmbientBoost;
	}

	void OSGEarthMapComponent::SetSkyAmbientBoost(float value)
	{
		m_SkyAmbientBoost = value;
		if (m_SkyNode)
		{
			m_SkyNode->getOrCreateStateSet()->getOrCreateUniform("oe_sky_ambientBoostFactor", osg::Uniform::FLOAT)->set(value);
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
			m_SkyNode->getSunLight()->setAmbient(osg::Vec4f(value,value,value,1));
		}
	}

	float OSGEarthMapComponent::GetMinimumAmbient() const
	{
		float value = 0;
		if (m_SkyNode)
		{
			value = m_SkyNode->getSunLight()->getAmbient().x();
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
		for (size_t i = 0; i < m_Viewpoints.size(); i++)
		{
			names.push_back(m_Viewpoints[i].name().value());
		}
		return names;
	}

	void OSGEarthMapComponent::SetViewpointByName(const std::string& name)
	{
		if (!m_Initlized)
			return;
		for (size_t i = 0; i < m_Viewpoints.size(); i++)
		{
			if (m_Viewpoints[i].name().value() == name)
			{
				osgEarth::Util::EarthManipulator* manip = m_OESceneManager->GetManipulator().get();
				if (manip)
					manip->setViewpoint(m_Viewpoints[i], 2.0);
				return; //done
			}
		}
	}

	const MapLayers& OSGEarthMapComponent::GetMapLayers() const
	{
		return m_MapLayers;
	}

	void OSGEarthMapComponent::UpdateMapLayers()
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
				auto* visible_layer = dynamic_cast<osgEarth::VisibleLayer*>(oe_layer);
				// only return layers that derive from VisibleLayer
				if (visible_layer)
				{
					m_MapLayers.emplace_back(std::make_unique<OSGEarthMapLayer>(OSGEarthMapLayer(visible_layer)));
				}
			}
		}
	}

	AABox OSGEarthMapComponent::GetBoundingBox() const
	{
		AABoxd bb;
		if (m_MapNode)
		{
			auto extent = m_MapNode->getMap()->getProfile()->getExtent();
			auto srs = m_MapNode->getMap()->getSRS();
			if (srs->isProjected())
			{
				const double min_elev = -1000;
				const double max_elev = 1000;
				osg::Vec3d w;
				osg::BoundingBoxd box;
				osgEarth::GeoPoint(srs, extent.xMin(), extent.yMin(), min_elev, osgEarth::ALTMODE_ABSOLUTE).toWorld(w); box.expandBy(w);
				osgEarth::GeoPoint(srs, extent.xMax(), extent.yMax(), max_elev, osgEarth::ALTMODE_ABSOLUTE).toWorld(w); box.expandBy(w);
				bb = OSGConvert::ToGASS(box);
			}
			else
			{
				auto sphere = GetBoundingSphere();
				bb.Min = sphere.m_Pos - Vec3(sphere.m_Radius, sphere.m_Radius, sphere.m_Radius);
				bb.Max = sphere.m_Pos + Vec3(sphere.m_Radius, sphere.m_Radius, sphere.m_Radius);
			}
		}
		return bb;
	}

	Sphere OSGEarthMapComponent::GetBoundingSphere() const
	{
		Sphere sphere;
		if (m_MapNode)
		{
			auto bounds = m_MapNode->getBound();
			sphere.m_Pos = OSGConvert::ToGASS(bounds.center());
			sphere.m_Radius = bounds.radius();
		}
		return sphere;
	}
	void OSGEarthMapComponent::SetShadowEnabled(bool value)
	{
		m_ShadowEnabled = value;
		if (m_ShadowCaster)
			m_ShadowCaster->setEnabled(value);
		
	}

	void OSGEarthMapComponent::SetShadowBlur(float value)
	{
		m_ShadowBlur = value;
		if (m_ShadowCaster)
			m_ShadowCaster->setBlurFactor(value);
	}

	void OSGEarthMapComponent::SetShadowRanges(std::vector<float> value)
	{
		m_ShadowRanges = value;
		if (m_ShadowCaster)
			m_ShadowCaster->setRanges(value);
	}

	void OSGEarthMapComponent::SetShadowColor(float value)
	{
		m_ShadowColor = value;
		if (m_ShadowCaster)
			m_ShadowCaster->setShadowColor(value);
	}

	void OSGEarthMapComponent::SetSkyHazeCutoff(float value)
	{
		m_SkyHazeCutoff = value;
		if (m_SkyNode)
			m_SkyNode->getOrCreateStateSet()->getOrCreateUniform("atmos_haze_cutoff", osg::Uniform::FLOAT)->set(value);
	}

	void OSGEarthMapComponent::SetSkyHazeStrength(float value)
	{
		m_SkyHazeStrength = value;
		if (m_SkyNode)
			m_SkyNode->getOrCreateStateSet()->getOrCreateUniform("atmos_haze_strength", osg::Uniform::FLOAT)->set(value);
	}
}

