#include <memory>

#include "Sim/GASSScene.h"
#include "Core/Utils/GASSSystem.h"

#include "Modules/OSGEarth/OSGEarthGraphicsSceneManager.h"
#include "Modules/OSGEarth/OSGEarthGraphicsSystem.h"
#include "Modules/OSG/OSGNodeData.h"
#include "Modules/OSG/OSGMaterial.h"
#include <osgDB/FileUtils>

namespace GASS
{
	struct OEMapListenerProxy : public osgEarth::MapCallback
	{
		OEMapListenerProxy(OSGEarthGraphicsSceneManager* sm) : m_OESceneManager(sm) {}
		void onMapModelChanged(const osgEarth::MapModelChange& change) override
		{
			m_OESceneManager->OnMapModelChanged(change);
		}
		OSGEarthGraphicsSceneManager* m_OESceneManager;
	};

	class OETerrainCallbackProxy : public osgEarth::TerrainCallback
	{
	public:
		OETerrainCallbackProxy(OSGEarthGraphicsSceneManager* sm) : m_OESceneManager(sm)
		{

		}

		void onTileAdded(
			const osgEarth::TileKey& key,
			osg::Node* graph,
			osgEarth::TerrainCallbackContext& context) override
		{
			m_OESceneManager->OnTileAdded(key, graph, context);
		}

		OSGEarthGraphicsSceneManager* m_OESceneManager;
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

		void SetOpacity(float value) override
		{
			if (auto image = dynamic_cast<osgEarth::ImageLayer*>(m_Layer))
				image->setOpacity(value);
		}

		float GetOpacity() const override
		{
			float value = 1.0f;
			if (auto image = dynamic_cast<osgEarth::ImageLayer*>(m_Layer))
				value = image->getOpacity();
			return value;
		}

		int GetUID() const override
		{
			return m_Layer->getUID();
		}
	private:
		osgEarth::VisibleLayer* m_Layer;
	};

	void OSGEarthGraphicsSceneManager::RegisterReflection()
	{
		GetClassRTTI()->SetMetaData(std::make_shared<ClassMetaData>("OSG Scene Manager", OF_VISIBLE));
		RegisterGetSet("EarthFile", &OSGEarthGraphicsSceneManager::GetEarthFile, &OSGEarthGraphicsSceneManager::SetEarthFile, PF_VISIBLE, "OSGEarth map file");
		RegisterGetSet("TimeOfDay", &OSGEarthGraphicsSceneManager::GetTimeOfDay, &OSGEarthGraphicsSceneManager::SetTimeOfDay, PF_VISIBLE | PF_EDITABLE, "Time of day");
		RegisterGetSet("MinimumAmbient", &OSGEarthGraphicsSceneManager::GetMinimumAmbient, &OSGEarthGraphicsSceneManager::SetMinimumAmbient, PF_VISIBLE | PF_EDITABLE, "Minimum ambient sky light");
		RegisterGetSet("SkyExposure", &OSGEarthGraphicsSceneManager::GetSkyExposure, &OSGEarthGraphicsSceneManager::SetSkyExposure, PF_VISIBLE | PF_EDITABLE, "Sky light exposure");
		RegisterGetSet("SkyContrast", &OSGEarthGraphicsSceneManager::GetSkyContrast, &OSGEarthGraphicsSceneManager::SetSkyContrast, PF_VISIBLE | PF_EDITABLE, "Sky light contrast");
		RegisterGetSet("SkyAmbientBoost", &OSGEarthGraphicsSceneManager::GetSkyAmbientBoost, &OSGEarthGraphicsSceneManager::SetSkyAmbientBoost, PF_VISIBLE | PF_EDITABLE, "Sky light ambient boost fasctor, (ONeal only)");
		RegisterGetSet("SkyLighting", &OSGEarthGraphicsSceneManager::GetSkyLighting, &OSGEarthGraphicsSceneManager::SetSkyLighting, PF_VISIBLE | PF_EDITABLE, "Enable/disable sky light");
		RegisterGetSet("SkyHazeCutoff", &OSGEarthGraphicsSceneManager::GetSkyHazeCutoff, &OSGEarthGraphicsSceneManager::SetSkyHazeCutoff, PF_VISIBLE | PF_EDITABLE, "SkyHazeCutoff");
		RegisterGetSet("SkyHazeStrength", &OSGEarthGraphicsSceneManager::GetSkyHazeStrength, &OSGEarthGraphicsSceneManager::SetSkyHazeStrength, PF_VISIBLE | PF_EDITABLE, "SkyHazeStrength");
		RegisterGetSet("ShadowEnabled", &OSGEarthGraphicsSceneManager::GetShadowEnabled, &OSGEarthGraphicsSceneManager::SetShadowEnabled, PF_VISIBLE | PF_EDITABLE, "");
		RegisterGetSet("ShadowBlur", &OSGEarthGraphicsSceneManager::GetShadowBlur, &OSGEarthGraphicsSceneManager::SetShadowBlur, PF_VISIBLE | PF_EDITABLE, "Shadow blur factor");
		RegisterGetSet("ShadowRanges", &OSGEarthGraphicsSceneManager::GetShadowRanges, &OSGEarthGraphicsSceneManager::SetShadowRanges, PF_VISIBLE | PF_EDITABLE, "Shadow ranges");
		RegisterGetSet("ShadowColor", &OSGEarthGraphicsSceneManager::GetShadowColor, &OSGEarthGraphicsSceneManager::SetShadowColor, PF_VISIBLE | PF_EDITABLE, "Shadow Color");

		auto layers_prop = RegisterGetSet("VisibleMapLayers", &OSGEarthGraphicsSceneManager::GetVisibleMapLayers, &OSGEarthGraphicsSceneManager::SetVisibleMapLayers, PF_VISIBLE, "Map Layers");
		layers_prop->SetObjectOptionsFunction(&OSGEarthGraphicsSceneManager::GetMapLayerNames);

		RegisterMember("AddSky", &OSGEarthGraphicsSceneManager::m_AddSky, PF_VISIBLE, "Add sky light");
		RegisterMember("MapIsRoot", &OSGEarthGraphicsSceneManager::m_MapIsRoot, PF_VISIBLE | PF_EDITABLE, "Objects should be placed under map node");
	}

	OSGEarthGraphicsSceneManager::OSGEarthGraphicsSceneManager(SceneWeakPtr scene) : Reflection(scene)
	{
		m_ShadowRanges = { 100.0f, 200.0f, 400.0f};
	}

	void OSGEarthGraphicsSceneManager::LoadXML(tinyxml2::XMLElement* elem)
	{
		OSGGraphicsSceneManager::LoadXML(elem);
		m_Initlized = true;
		SetEarthFile(m_EarthFile);
		UpdateMapLayers();
	}

	void OSGEarthGraphicsSceneManager::OnPostConstruction()
	{
		RegisterForPostUpdate<OSGEarthGraphicsSystem>();
		ScenePtr scene = GetScene();
		if (!scene)
		{
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Scene not present", "OSGEarthGraphicsSceneManager::OnInitialize");
		}

		m_RootNode = new osg::PositionAttitudeTransform();
		m_RootNode->setName("GASSRootNode");
		
		m_RootNode->setStateSet(new SimpleMaterial());
		osg::StateSet* stateset = m_RootNode->getOrCreateStateSet();
		Material::SetLighting(stateset, osg::StateAttribute::ON);
		Material::SetReceiveShadows(stateset, osg::StateAttribute::ON);


		m_ObjectRoot = new osg::Group();
		m_ObjectRoot->setName("GASSObjectRootNode");


		m_WGS84 = osgEarth::SpatialReference::create("wgs84");
		OSGGraphicsSystemPtr osg_sys = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<OSGGraphicsSystem>();
	
		osgViewer::ViewerBase::Views views;
		osg_sys->GetViewer()->getViews(views);

		if (views.size() == 0)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed find view", "OSGEarthGraphicsSceneManager::OnPostConstruction");

		auto* view = dynamic_cast<osgViewer::View*>(views[0]);

		if (view == nullptr)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed find cast view", "OSGEarthGraphicsSceneManager::OnPostConstruction");

		m_EarthManipulator = new osgEarth::Util::EarthManipulator();
		view->setCameraManipulator(m_EarthManipulator);

#if 1
		//major hack for intel cards to get white vertex-colors on 3d-models.
		//When using VertexAttributeAliasing we don't get vertex-color 
		//from models processed with oe shadergenerator, instead previous/current state is used, 
		//maybe driver bug?
		//Temp fix is to create a geometry with color array, we use the osgEarth ControlCanvas 
		//for this to be sure the geometry is visible/rendered every frame.
		//Also set the color to white and alpha to one and hope that this will be used by all model nodes
		{
			osgEarth::Util::Controls::ControlCanvas* canvas = osgEarth::Util::Controls::ControlCanvas::getOrCreate(view);
			osgEarth::Util::Controls::Container* main_container = canvas->addControl(new osgEarth::Util::Controls::VBox());
			main_container->setBackColor(osgEarth::Util::Controls::Color(osgEarth::Util::Controls::Color::White,1.0f));
			main_container->setHorizAlign(osgEarth::Util::Controls::Control::ALIGN_LEFT);
			main_container->setVertAlign(osgEarth::Util::Controls::Control::ALIGN_BOTTOM);
			osg::ref_ptr<osg::Group> root = GetOSGRootNode();
			root->addChild(canvas);
		}
#endif
	}

	OSGEarthGraphicsSceneManager::~OSGEarthGraphicsSceneManager(void)
	{
		delete m_WorkingSet;
	}

	bool OSGEarthGraphicsSceneManager::GetShadowEnabled() const
	{
		return m_ShadowEnabled;
	}

	void OSGEarthGraphicsSceneManager::SetShadowEnabled(bool value)
	{
		m_ShadowEnabled = value;

		if (m_ShadowCaster)
			m_ShadowCaster->setEnabled(value);
	}

	void OSGEarthGraphicsSceneManager::OnSceneCreated()
	{
		m_CollisionSceneManager = GetScene()->GetFirstSceneManagerByClass<ICollisionSceneManager>().get();	
		void* main_root = static_cast<void*>(m_RootNode.get());
		void* shadow_node = main_root;
		SystemMessagePtr loaded_msg(new GraphicsSceneManagerLoadedEvent(std::string("OSG"), main_root,shadow_node));
		SimEngine::Get().GetSimSystemManager()->SendImmediate(loaded_msg);
	}

	void OSGEarthGraphicsSceneManager::OnSceneShutdown()
	{

	}

	void OSGEarthGraphicsSceneManager::PostProcess(osg::Node* node)
	{
		if(m_MapIsRoot)
			osgEarth::Registry::shaderGenerator().run(node);
	}

	void OSGEarthGraphicsSceneManager::DrawLine(const Vec3 &, const Vec3 &, const ColorRGBA &, const ColorRGBA &)
	{
	}

	void OSGEarthGraphicsSceneManager::OnUpdate(double /*delta_time*/)
	{

	}

	void OSGEarthGraphicsSceneManager::SetEarthFile(const ResourceHandle& earth_file)
	{
		//Always store filename
		m_EarthFile = earth_file;

		if (!m_Initlized)
			return;
		if (m_EarthFile.Name() == "")
			return;


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

		osg::Group* root = m_RootNode.get();

		//if no sky is present (projected mode) but we still want get terrain lightning  
		//m_Lighting = new osgEarth::PhongLightingEffect();
		//m_Lighting->setCreateLightingUniform(false);
		//m_Lighting->attach(m_MapNode->getOrCreateStateSet());

		m_SkyNode = osgEarth::findFirstParentOfType<osgEarth::Util::SkyNode>(m_MapNode);
		if (!m_SkyNode && m_AddSky)
		{
			osgEarth::SimpleSky::SimpleSkyOptions sky_options;
			sky_options.atmosphericLighting() = true;
			sky_options.quality() = osgEarth::SkyOptions::QUALITY_HIGH;
			sky_options.contrast() = m_SkyContrast;
			sky_options.exposure() = m_SkyExposure;
			sky_options.daytimeAmbientBoost() = m_SkyAmbientBoost;
			sky_options.atmosphereVisible() = true;
			std::string ext = m_MapNode->getMapSRS()->isGeographic() ? "sky_simple" : "sky_gl";
			m_MapNode->addExtension(osgEarth::Extension::create(ext, sky_options));
			m_SkyNode = osgEarth::findFirstParentOfType<osgEarth::Util::SkyNode>(m_MapNode);
			SetTimeOfDay(m_Hour);
		}
		if (m_SkyNode)
		{
			//set default year/month and day to get good lighting
			m_SkyNode->setDateTime(osgEarth::DateTime(2017, 6, 6, m_Hour));
		}


		auto scene_root = GetScene()->GetRootSceneObject();
		//Connect component with osg by adding user data, this is needed if we want to used the intersection implementated by the OSGCollisionSystem
		osg::ref_ptr<OSGNodeData> data = new OSGNodeData(static_cast<int>(GEOMETRY_FLAG_GROUND) | static_cast<int>(GEOMETRY_FLAG_STATIC_OBJECT));
		m_MapNode->setUserData(data);

		osgEarth::Util::EarthManipulator* manip = m_EarthManipulator.get();
		manip->setNode(m_MapNode);
		OnElevationChanged();
		

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

		OSGGraphicsSystemPtr osg_sys = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<OSGGraphicsSystem>();
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
			}
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

		//GetSceneObject()->PostEvent(std::make_shared<GeometryChangedEvent>(GASS_DYNAMIC_PTR_CAST<IGeometryComponent>(shared_from_this())));
		GetScene()->PostMessage(GASS_MAKE_SHARED<TerrainChangedEvent>());

		
		//if (m_IsRoot)
		{
			OSGConvert::SetOSGNodeMask(GEOMETRY_FLAG_ALL, m_ObjectRoot);
			if (m_MapIsRoot)
				m_MapNode->addChild(m_ObjectRoot);
			else
				root->addChild(m_ObjectRoot);
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
				m_ShadowCaster->setTraversalMask(NM_CAST_SHADOWS);
				m_ShadowCaster->getShadowCastingGroup()->addChild(m_ObjectRoot);

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
		m_Initlized = true;
	}

	void OSGEarthGraphicsSceneManager::Shutdown()
	{
		if (m_Initlized && m_MapNode)
		{
			if (m_MapNode->getTerrain())
				m_MapNode->getTerrain()->removeTerrainCallback(m_TerrainCallbackProxy);
			// disconnect extensions
			osgViewer::ViewerBase::Views views;
			OSGGraphicsSystemPtr osg_sys = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<OSGGraphicsSystem>();
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

		}
	}

	void OSGEarthGraphicsSceneManager::UpdateMapLayers()
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
					m_MapLayers.emplace_back(std::unique_ptr<OSGEarthMapLayer>(new OSGEarthMapLayer(visible_layer)));
				}
			}
		}
	}

	void OSGEarthGraphicsSceneManager::SetupNodeMasks()
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

	void OSGEarthGraphicsSceneManager::SetTimeOfDay(double hour)
	{
		m_Hour = hour;
		if (m_SkyNode)
		{
			m_SkyNode->setDateTime(osgEarth::DateTime(2017, 6, 6, m_Hour));
		}
	}

	void OSGEarthGraphicsSceneManager::OnMapModelChanged(const osgEarth::MapModelChange& change)
	{
		const osgEarth::ElevationLayer* elevation_layer = change.getElevationLayer();
		if (elevation_layer)
		{
			OnElevationChanged();
			GetScene()->PostMessage(GASS_MAKE_SHARED<TerrainChangedEvent>());
		}
	}

	void OSGEarthGraphicsSceneManager::OnElevationChanged()
	{
		if (m_MapNode && m_MapNode->getMap())
		{
			delete m_WorkingSet;
			m_WorkingSet = new osgEarth::ElevationPool::WorkingSet();
		}
	}


	bool OSGEarthGraphicsSceneManager::SceneToWGS84(const Vec3& scene_location, GeoLocation& geo_location) const
	{
		const osg::Vec3d osg_location = OSGConvert::ToOSG(scene_location);
		osgEarth::GeoPoint input;
		const bool status = input.fromWorld(m_MapNode->getMapSRS(), osg_location);
		const osgEarth::GeoPoint output = input.transform(m_WGS84);// SpatialReference::create("epsg:4326"));
		geo_location.Latitude = output.y();
		geo_location.Longitude = output.x();
		geo_location.Height = output.z();
		return status;
	}

	bool OSGEarthGraphicsSceneManager::WGS84ToScene(const GeoLocation& geo_location, Vec3& scene_location) const
	{
		const osgEarth::GeoPoint input(m_WGS84, geo_location.Longitude, geo_location.Latitude, geo_location.Height, osgEarth::ALTMODE_ABSOLUTE);
		const osgEarth::GeoPoint output = input.transform(m_MapNode->getMapSRS());
		osg::Vec3d osg_location;
		const bool status = output.toWorld(osg_location);
		scene_location = OSGConvert::ToGASS(osg_location);
		return status;
	}

	bool OSGEarthGraphicsSceneManager::GetTerrainHeight(const Vec3& location, double& height, GeometryFlags flags) const
	{
		GeoLocation geo_location;
		SceneToWGS84(location, geo_location);
		return GetTerrainHeight(geo_location, height, flags);
	}


	bool OSGEarthGraphicsSceneManager::GetSceneHeight(const GeoLocation& location, double& height, GeometryFlags flags) const
	{
		const osgEarth::SpatialReference* mapSRS = m_MapNode->getMapSRS();
		const auto* em = &mapSRS->getEllipsoid();
		const double r = osg::minimum(em->getRadiusEquator(), em->getRadiusPolar());

		// calculate the endpoints for an intersection test:
		Vec3 start, end;
		WGS84ToScene(GeoLocation(location.Longitude, location.Latitude, r), start);
		WGS84ToScene(GeoLocation(location.Longitude, location.Latitude, -r), end);

		CollisionResult result;
		Vec3 dir = end - start;
		m_CollisionSceneManager->Raycast(start, dir, flags, result);
		if (result.Coll)
		{
			GeoLocation collision_point;
			SceneToWGS84(result.CollPosition, collision_point);
			height = collision_point.Height;
		}
		return result.Coll;
	}

	bool OSGEarthGraphicsSceneManager::GetTerrainHeight(const GeoLocation& location, double& height, GeometryFlags flags) const
	{
		bool status = false;
		if (m_MapNode)
		{
			if (flags & GEOMETRY_FLAG_GROUND_LOD) //get terrain height at pre defined LOD
			{
				osgEarth::GeoPoint map_point(m_WGS84, location.Longitude, location.Latitude);
				// Query the elevation at the map location:
				osgEarth::ElevationSample sample = m_MapNode->getMap()->getElevationPool()->getSample(
					map_point,
					m_WorkingSet);


				double elevation = NO_DATA_VALUE;
				if (sample.hasData())
				{
					// convert to geodetic to get the HAE:
					elevation = sample.elevation().as(osgEarth::Units::METERS);
				}


				if (elevation != NO_DATA_VALUE)
					height = elevation;
				else
					height = 0;
				status = true;

				//test model layer 
				osgEarth::ModelLayerVector model_layers;
				m_MapNode->getMap()->getLayers(model_layers);
				for (unsigned i = 0; i < model_layers.size(); ++i)
				{
					double model_elevation = 0;
					if (model_layers[i]->getNode() && model_layers[i]->options().terrainPatch() == true)
					{
						bool model_status = m_MapNode->getTerrain()->getHeight(model_layers[i]->getNode(), m_WGS84, location.Longitude, location.Latitude, &model_elevation, nullptr);
						if (model_status)
						{
							//Always use terrain patch height to support terrain mask?
							if (model_elevation > height)
							{
								height = model_elevation;
							}
						}
					}
				}
			}
			else
			{
				status = m_MapNode->getTerrain()->getHeight(m_MapNode->getTerrain()->getGraph(), m_WGS84, location.Longitude, location.Latitude, &height, nullptr);
			}

			//include height from GASS-geometry
			if (flags & GEOMETRY_FLAG_SCENE_OBJECTS)
			{
				//remove ground...checked above!
				auto no_ground_flags = GeometryFlags(flags & ~GEOMETRY_FLAG_GROUND);
				double object_h = 0;
				if (GetSceneHeight(location, object_h, no_ground_flags))
				{
					if (object_h > height)
					{
						height = object_h;
						status = true;
					}
				}
			}
		}
		return status;
	}

	bool OSGEarthGraphicsSceneManager::GetHeightAboveTerrain(const Vec3& location, double& height, GeometryFlags flags) const
	{
		GeoLocation geo_location;
		bool status = SceneToWGS84(location, geo_location);
		status = GetHeightAboveTerrain(geo_location, height, flags);
		return status;
	}

	bool OSGEarthGraphicsSceneManager::GetHeightAboveTerrain(const GeoLocation& location, double& height, GeometryFlags flags) const
	{
		bool status = false;
		double terrain_height = 0;
		if (GetTerrainHeight(location, terrain_height, flags))
		{
			height = location.Height - terrain_height;
			status = true;
		}
		return status;
	}

	bool OSGEarthGraphicsSceneManager::GetUpVector(const Vec3& location, Vec3& up_vec) const
	{
		bool status = false;
		const osgEarth::SpatialReference* map_srs = m_MapNode->getMapSRS();
		const osg::Vec3d osg_pos = OSGConvert::ToOSG(location);
		osgEarth::GeoPoint gp;
		if (gp.fromWorld(map_srs, osg_pos))
		{
			osg::Vec3d osg_up_vec;
			if (gp.createWorldUpVector(osg_up_vec))
			{
				up_vec = OSGConvert::ToGASS(osg_up_vec);
				status = true;
			}
		}
		return status;
	}

	bool OSGEarthGraphicsSceneManager::GetOrientation(const Vec3& location, Quaternion& rot) const
	{
		bool status = false;
		const osgEarth::SpatialReference* map_srs = m_MapNode->getMapSRS();
		const osg::Vec3d osg_pos = OSGConvert::ToOSG(location);
		osgEarth::GeoPoint gp;
		if (gp.fromWorld(map_srs, osg_pos))
		{
			osg::Matrixd local2world;
			if (gp.createLocalToWorld(local2world))
			{
				osg::Quat osg_rot = local2world.getRotate();
				rot = OSGConvert::ToGASS(osg_rot);
				status = true;
			}
		}
		return status;
	}

	float OSGEarthGraphicsSceneManager::GetSkyExposure() const
	{
		return m_SkyExposure;
	}

	void OSGEarthGraphicsSceneManager::SetSkyExposure(float value)
	{
		m_SkyExposure = value;
		if (m_SkyNode)
		{
			m_SkyNode->getOrCreateStateSet()->getOrCreateUniform("oe_sky_exposure", osg::Uniform::FLOAT)->set(value);
		}
	}

	float OSGEarthGraphicsSceneManager::GetSkyContrast() const
	{
		return m_SkyContrast;
	}

	void OSGEarthGraphicsSceneManager::SetSkyContrast(float value)
	{
		m_SkyContrast = value;
		if (m_SkyNode)
		{
			m_SkyNode->getOrCreateStateSet()->getOrCreateUniform("oe_sky_contrast", osg::Uniform::FLOAT)->set(value);
		}
	}

	float OSGEarthGraphicsSceneManager::GetSkyAmbientBoost() const
	{
		return m_SkyAmbientBoost;
	}

	void OSGEarthGraphicsSceneManager::SetSkyAmbientBoost(float value)
	{
		m_SkyAmbientBoost = value;
		if (m_SkyNode)
		{
			m_SkyNode->getOrCreateStateSet()->getOrCreateUniform("oe_sky_ambientBoostFactor", osg::Uniform::FLOAT)->set(value);
		}
	}

	void OSGEarthGraphicsSceneManager::SetMinimumAmbient(float value)
	{
		if (m_SkyNode)
		{
			m_SkyNode->getSunLight()->setAmbient(osg::Vec4f(value, value, value, 1));
		}
	}

	float OSGEarthGraphicsSceneManager::GetMinimumAmbient() const
	{
		float value = 0;
		if (m_SkyNode)
		{
			value = m_SkyNode->getSunLight()->getAmbient().x();
		}
		return value;
	}

	void OSGEarthGraphicsSceneManager::SetSkyLighting(bool value)
	{
		if (m_SkyNode)
		{
			m_SkyNode->setLighting(value);
			const auto state_mode = osg::StateAttribute::OVERRIDE | (value ? osg::StateAttribute::ON : osg::StateAttribute::OFF);
			osgEarth::GLUtils::setLighting(m_MapNode->getOrCreateStateSet(), state_mode);
		}
	}

	bool OSGEarthGraphicsSceneManager::GetSkyLighting() const
	{
		bool value = false;
		if (m_SkyNode)
		{
			value = m_SkyNode->getLighting();
		}
		return value;
	}


	std::vector<std::string> OSGEarthGraphicsSceneManager::GetViewpointNames() const
	{
		std::vector<std::string> names;
		for (size_t i = 0; i < m_Viewpoints.size(); i++)
		{
			names.push_back(m_Viewpoints[i].name().value());
		}
		return names;
	}

	void OSGEarthGraphicsSceneManager::SetViewpointByName(const std::string& name)
	{
		if (!m_Initlized)
			return;
		for (size_t i = 0; i < m_Viewpoints.size(); i++)
		{
			if (m_Viewpoints[i].name().value() == name)
			{
				osgEarth::Util::EarthManipulator* manip = GetManipulator().get();
				if (manip)
					manip->setViewpoint(m_Viewpoints[i], 2.0);
				return; //done
			}
		}
	}

	const MapLayers& OSGEarthGraphicsSceneManager::GetMapLayers() const
	{
		return m_MapLayers;
	}

	std::vector<std::string> OSGEarthGraphicsSceneManager::GetMapLayerNames() const
	{
		std::vector<std::string> layer_names;
		const MapLayers& layers = GetMapLayers();
		for (size_t i = 0; i < layers.size(); i++)
		{
			layer_names.push_back(layers[i]->GetName());
		}
		return layer_names;
	}

	std::vector<std::string> OSGEarthGraphicsSceneManager::GetVisibleMapLayers() const
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

	void OSGEarthGraphicsSceneManager::SetVisibleMapLayers(const std::vector<std::string>& layer_names)
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

	void OSGEarthGraphicsSceneManager::SetSkyHazeCutoff(float value)
	{
		m_SkyHazeCutoff = value;
		if (m_SkyNode)
			m_SkyNode->getOrCreateStateSet()->getOrCreateUniform("atmos_haze_cutoff", osg::Uniform::FLOAT)->set(value);
	}

	void OSGEarthGraphicsSceneManager::SetSkyHazeStrength(float value)
	{
		m_SkyHazeStrength = value;
		if (m_SkyNode)
			m_SkyNode->getOrCreateStateSet()->getOrCreateUniform("atmos_haze_strength", osg::Uniform::FLOAT)->set(value);
	}

	void OSGEarthGraphicsSceneManager::SetShadowBlur(float value)
	{
		m_ShadowBlur = value;
		if (m_ShadowCaster)
			m_ShadowCaster->setBlurFactor(value);
	}

	void OSGEarthGraphicsSceneManager::SetShadowRanges(std::vector<float> value)
	{
		m_ShadowRanges = value;
		if (m_ShadowCaster)
			m_ShadowCaster->setRanges(value);
	}

	void OSGEarthGraphicsSceneManager::SetShadowColor(float value)
	{
		m_ShadowColor = value;
		if (m_ShadowCaster)
			m_ShadowCaster->setShadowColor(value);
	}
}

