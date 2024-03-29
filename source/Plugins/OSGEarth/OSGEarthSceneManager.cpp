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
#include "OSGEarthSceneManager.h"
#include "OSGEarthGeoComponent.h"

#include "Plugins/OSG/IOSGGraphicsSceneManager.h"
#include "Plugins/OSG/IOSGGraphicsSystem.h"
#include "Plugins/OSG/OSGNodeData.h"
#include "Plugins/OSG/OSGConvert.h"
//#include <osgEarth/LogarithmicDepthBuffer>

namespace GASS
{
	/**
	* Toggles the main control canvas on and off.
	*/
	struct ToggleCanvasEventHandler : public osgGA::GUIEventHandler
	{
		ToggleCanvasEventHandler(osg::Node* canvas, char key) :
			_canvas(canvas), _key(key)
		{
		}

		bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& /*aa*/) override
		{
			if (ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN)
			{
				if (ea.getKey() == _key)
				{
					osg::ref_ptr< osg::Node > safe_node = _canvas.get();
					if (safe_node.valid())
					{
						safe_node->setNodeMask(safe_node->getNodeMask() ? 0 : ~0);
					}
					return true;
				}
			}
			return false;
		}

		osg::observer_ptr<osg::Node> _canvas;
		char _key;
	};

	void OSGEarthSceneManager::RegisterReflection()
	{
		SceneManagerFactory::GetPtr()->Register<OSGEarthSceneManager>("OSGEarthSceneManager");
	}

	OSGEarthSceneManager::OSGEarthSceneManager(SceneWeakPtr scene) : Reflection(scene),
		m_AutoAdd(false),
		m_Initlized(false),
		m_DisableGLSL(false),
		m_WGS84(nullptr),
		m_MapNode(nullptr),
		m_CollisionSceneManager(nullptr)
	{
		
	}

	void OSGEarthSceneManager::OnPostConstruction()
	{
		//if (m_DisableGLSL)
			//_putenv("OSGEARTH_NO_GLSL=1");
		GetScene()->RegisterForMessage(REG_TMESS(OSGEarthSceneManager::OnLoadSceneObject, PreSceneObjectInitializedEvent, 0));
	}

	void OSGEarthSceneManager::OnSceneShutdown()
	{

	}

	OSGEarthSceneManager::~OSGEarthSceneManager()
	{
		delete m_WorkingSet;
	}

	void OSGEarthSceneManager::OnSceneCreated()
	{
		m_Initlized = true;

		m_WGS84 = osgEarth::SpatialReference::create("wgs84");

		IOSGGraphicsSystemPtr osg_sys = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<IOSGGraphicsSystem>();
		m_CollisionSceneManager = GetScene()->GetFirstSceneManagerByClass<ICollisionSceneManager>().get();

		osgViewer::ViewerBase::Views views;
		osg_sys->GetViewer()->getViews(views);

		if (views.size() == 0)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed find view", "OSGEarthSceneManager::OnInit");

		auto* view = dynamic_cast<osgViewer::View*>(views[0]);

		if (view == nullptr)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed find cast view", "OSGEarthSceneManager::OnInit");

		m_EarthManipulator = new osgEarth::Util::EarthManipulator();
		views[0]->setCameraManipulator(m_EarthManipulator);

		//osgEarth::Util::LogarithmicDepthBuffer logDepth;
		//logDepth.setUseFragDepth(false);
		//logDepth.install(views[0]->getCamera());


		osgEarth::Util::Controls::ControlCanvas* canvas = osgEarth::Util::Controls::ControlCanvas::getOrCreate(view);

		osgEarth::Util::Controls::Container* main_container = canvas->addControl(new osgEarth::Util::Controls::VBox());
		main_container->setBackColor(osgEarth::Util::Controls::Color(osgEarth::Util::Controls::Color::Black, 0.8f));
		main_container->setHorizAlign(osgEarth::Util::Controls::Control::ALIGN_LEFT);
		main_container->setVertAlign(osgEarth::Util::Controls::Control::ALIGN_BOTTOM);

		IOSGGraphicsSceneManagerPtr osg_sm = GetScene()->GetFirstSceneManagerByClass<IOSGGraphicsSceneManager>();
		osg::ref_ptr<osg::Group> root = osg_sm->GetOSGRootNode();

		//hide GUI by default
		canvas->setNodeMask(0);

		root->addChild(canvas);
		m_GUI = main_container;


		view->addEventHandler(new ToggleCanvasEventHandler(canvas, 'x'));
	}

	void OSGEarthSceneManager::OnLoadSceneObject(PreSceneObjectInitializedEventPtr message)
	{
		//auto add component if location component exist?
		SceneObjectPtr obj = message->GetSceneObject();
		if (m_AutoAdd)
		{
			LocationComponentPtr location = obj->GetFirstComponentByClass<ILocationComponent>();
			if (location) //only add to objects that have location component
			{
				OSGEarthGeoComponentPtr comp = obj->GetFirstComponentByClass<OSGEarthGeoComponent>();
				if (!comp) //check that component not already exist
				{
					comp = GASS_DYNAMIC_PTR_CAST<OSGEarthGeoComponent>(ComponentFactory::Get().Create("OSGEarthGeoComponent"));
					if (comp)
					{
						obj->AddComponent(comp);
					}
				}
			}
		}
	}

	void OSGEarthSceneManager::SetMapNode(osgEarth::MapNode* map_node)
	{
		//Set EarthManipulator to only work on map node, otherwise entire scene is used for calculating home-position etc.
		osgEarth::Util::EarthManipulator* manip = GetManipulator().get();
		if (manip)
		{
			if (m_MapNode && map_node == nullptr)
			{
				m_OldVP = manip->getViewpoint();
				manip->setNode(map_node);
			}
			else
			{
				manip->setNode(map_node);
				//restore viewpoint
				if(m_OldVP.isValid())
					manip->setViewpoint(m_OldVP);
			}
			
		}
		if (map_node == nullptr)
		{ 
			delete m_WorkingSet;
			m_WorkingSet = nullptr;
		}
		m_MapNode = map_node;
		OnElevationChanged();
	}

	void OSGEarthSceneManager::OnElevationChanged()
	{
		if (m_MapNode && m_MapNode->getMap())
		{
			//const unsigned int elev_lod = 23u;
			//const unsigned int elev_lod = 15u;
			//Use m_WGS84 here because we ask for height in GeoCoords
			m_WorkingSet = new osgEarth::ElevationPool::WorkingSet();
		}
	}

#if 0
	void OSGEarthSceneManager::SetEarthFile(const std::string &earth_file)
	{
		m_EarthFile = earth_file;
		if (!m_Initlized)
			return;
		if (earth_file != "")
		{
			//disable GLSL
			IOSGGraphicsSceneManagerPtr osg_sm = GetScene()->GetFirstSceneManagerByClass<IOSGGraphicsSceneManager>();
			osg::ref_ptr<osg::Group> root = osg_sm->GetOSGRootNode();
			ResourceHandle rh(earth_file);
			std::string full_path = rh.GetResource()->Path().GetFullPath();
			//osg::Node* node = osgDB::readNodeFile(full_path);
			//m_MapNode = osgEarth::MapNode::findMapNode(node);
			//root->addChild(m_MapNode);

			//Add dummy component for collisison

			/*ComponentPtr  geom_comp(GASS_DYNAMIC_PTR_CAST<Component>(ComponentFactory::Get().Create("ManualMeshComponent")));
			OSGNodeData* data = new OSGNodeData(geom_comp);
			m_MapNode->setUserData(data);
			GeometryFlagsBinder flags;
			flags.SetValue(GEOMETRY_FLAG_GROUND);
			geom_comp->SetPropertyValue("GeometryFlags", flags);
			dummy = SceneObjectPtr(new SceneObject);
			dummy->AddComponent(geom_comp);*/


			/*const osgEarth::Config& external = m_MapNode->externalConfig();
			const osgEarth::Config& skyConf         = externals.child("sky");
			const osgEarth::Config& oceanConf       = externals.child("ocean");
			const osgEarth::Config& annoConf        = externals.child("annotations");
			const osgEarth::Config& declutterConf   = externals.child("decluttering");

			osgEarth::Config        viewpointsConf  = externals.child("viewpoints");

			// backwards-compatibility: read viewpoints at the top level:
			const osgEarth::ConfigSet& old_viewpoints = externals.children("viewpoint");
			for( osgEarth::ConfigSet::const_iterator i = old_viewpoints.begin(); i != old_viewpoints.end(); ++i )
				viewpointsConf.add( *i );

				*/
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

				/*if(m_UseSky)
				{
					osgEarth::Util::SkyNode* sky = osgEarth::Util::SkyNode::create( m_MapNode);
					//osgEarth::Util::SkyNode* sky = new osgEarth::Util::SkyNode::vre( m_MapNode->getMap());
					sky->setDateTime(osgEarth::DateTime(2013, 1, 6, 17.0));
					sky->attach( views[0] );
					root->addChild( sky );
					if(m_ShowSkyControl)
					{
						osgEarth::Util::Controls::Control* c = osgEarth::Util::SkyControlFactory().create(sky);
						if ( c )
							mainContainer->addControl( c );
					}
				}*/

			if (m_UseOcean)
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

		}
	}
#endif

	

	/*void OSGEarthSceneManager::FromLatLongToMap(double latitude, double longitude, Vec3 &pos, Quaternion &rot) const
	{
		if (m_MapNode)
		{
			const osgEarth::SpatialReference* geoSRS = m_MapNode->getMapSRS()->getGeographicSRS();
			const osgEarth::SpatialReference* mapSRS = m_MapNode->getMapSRS();

			double height = 0;
			//Create geocentric coordinates from lat long, use  Geographic-SRS!
			//m_MapNode->getTerrain()->getHeight(0L, geoSRS, longitude, latitude, &height, 0L);

			//include all osgEarth geometries
			m_MapNode->getTerrain()->getHeight(m_MapNode, geoSRS, longitude, latitude, &height, 0L);

			osgEarth::GeoPoint mapPoint(geoSRS, longitude, latitude, height, osgEarth::ALTMODE_ABSOLUTE);

			//Transform geocentric coordinates to map-space using map-SRS!
			osgEarth::GeoPoint mapPos = mapPoint.transform(mapSRS);
			osg::Matrixd out_local2world;
			mapPos.createLocalToWorld(out_local2world);

			osg::Quat osg_rot = out_local2world.getRotate();
			osg::Vec3d osg_pos = out_local2world.getTrans();
			pos = OSGConvert::ToGASS(osg_pos);
			rot = Quaternion(osg_rot.w(), -osg_rot.x(), -osg_rot.z(), osg_rot.y());
		}
	}*/

	bool OSGEarthSceneManager::SceneToWGS84(const Vec3 &scene_location, GeoLocation &geo_location) const
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

	bool OSGEarthSceneManager::WGS84ToScene(const GeoLocation &geo_location, Vec3 &scene_location) const
	{
		const osgEarth::GeoPoint input(m_WGS84, geo_location.Longitude, geo_location.Latitude, geo_location.Height, osgEarth::ALTMODE_ABSOLUTE);
		const osgEarth::GeoPoint output = input.transform(m_MapNode->getMapSRS());
		osg::Vec3d osg_location;
		const bool status = output.toWorld(osg_location);
		scene_location = OSGConvert::ToGASS(osg_location);
		return status;
	}

	bool OSGEarthSceneManager::GetTerrainHeight(const Vec3 &location, double &height, GeometryFlags flags) const
	{
		GeoLocation geo_location;
		SceneToWGS84(location, geo_location);
		return GetTerrainHeight(geo_location,height, flags);
	}


	bool OSGEarthSceneManager::GetSceneHeight(const GeoLocation &location, double &height, GeometryFlags flags) const
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
		if(result.Coll)
		{
			GeoLocation collision_point;
			SceneToWGS84(result.CollPosition, collision_point);
			height = collision_point.Height;
		}
		return result.Coll;
	}
	
	bool OSGEarthSceneManager::GetTerrainHeight(const GeoLocation &location, double &height, GeometryFlags flags) const
	{
		bool status = false;
		if(m_MapNode)
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
				double object_h =0 ;
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

	bool OSGEarthSceneManager::GetHeightAboveTerrain(const Vec3 &location, double &height, GeometryFlags flags) const
	{
		GeoLocation geo_location;
		bool status = SceneToWGS84(location, geo_location);
		status = GetHeightAboveTerrain(geo_location, height, flags);
		return status;
	}

	bool OSGEarthSceneManager::GetHeightAboveTerrain(const GeoLocation &location, double &height, GeometryFlags flags) const
	{
		bool status = false;
		double terrain_height = 0;
		if (GetTerrainHeight(location, terrain_height,flags))
		{
			height = location.Height - terrain_height;
			status = true;
		}
		return status;
	}

	bool OSGEarthSceneManager::GetUpVector(const Vec3 &location, Vec3 &up_vec) const
	{
		bool status = false;
		const osgEarth::SpatialReference* map_srs = m_MapNode->getMapSRS();
		const osg::Vec3d osg_pos = OSGConvert::ToOSG(location);
		osgEarth::GeoPoint gp;
		if(gp.fromWorld(map_srs, osg_pos))
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

	bool OSGEarthSceneManager::GetOrientation(const Vec3 &location, Quaternion &rot) const
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

}
