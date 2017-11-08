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
#include "OSGEarthGeoComponent.h"
#include "OSGEarthSceneManager.h"
#include "Plugins/OSG/OSGNodeData.h"
#include "Plugins/OSG/OSGConvert.h"

namespace GASS
{
	OSGEarthSceneManager::OSGEarthSceneManager() : 	m_AutoAdd(true), 
		m_Initlized(false),
		m_DisableGLSL(false)
	{

	}

	OSGEarthSceneManager::~OSGEarthSceneManager()
	{

	}

	void OSGEarthSceneManager::RegisterReflection()
	{
		SceneManagerFactory::GetPtr()->Register("OSGEarthSceneManager",new GASS::Creator<OSGEarthSceneManager, ISceneManager>);
		/*RegisterProperty<std::string>("EarthFile", &OSGEarthSceneManager::GetEarthFile, &OSGEarthSceneManager::SetEarthFile);
		RegisterProperty<bool>("UseSky", &OSGEarthSceneManager::GetUseSky, &OSGEarthSceneManager::SetUseSky);
		RegisterProperty<bool>("ShowSkyControl", &OSGEarthSceneManager::GetShowSkyControl, &OSGEarthSceneManager::SetShowSkyControl);
		RegisterProperty<bool>("UseOcean", &OSGEarthSceneManager::GetUseOcean, &OSGEarthSceneManager::SetUseOcean);
		RegisterProperty<bool>("ShowOceanControl", &OSGEarthSceneManager::GetShowOceanControl, &OSGEarthSceneManager::SetShowOceanControl);
		RegisterProperty<bool>("DisableGLSL", &OSGEarthSceneManager::GetDisableGLSL, &OSGEarthSceneManager::SetDisableGLSL);*/
	}

	void OSGEarthSceneManager::OnCreate()
	{
		if(m_DisableGLSL)
			_putenv("OSGEARTH_NO_GLSL=1");
		GetScene()->RegisterForMessage(REG_TMESS(OSGEarthSceneManager::OnLoadSceneObject,PreSceneObjectInitializedEvent,0));
	}

	void OSGEarthSceneManager::OnLoadSceneObject(PreSceneObjectInitializedEventPtr message)
	{
		//auto add component if location component exist?
		SceneObjectPtr obj = message->GetSceneObject();
		if(m_AutoAdd)
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

	void OSGEarthSceneManager::OnInit()
	{
		m_Initlized = true;

		IOSGGraphicsSystemPtr osg_sys = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<IOSGGraphicsSystem>();

		osgViewer::ViewerBase::Views views;
		osg_sys->GetViewer()->getViews(views);

		m_EarthManipulator = new osgEarth::Util::EarthManipulator();
		views[0]->setCameraManipulator(m_EarthManipulator);
		osgEarth::Util::Controls::ControlCanvas* canvas = osgEarth::Util::Controls::ControlCanvas::getOrCreate(views[0]);

		osgEarth::Util::Controls::Container* mainContainer = canvas->addControl(new osgEarth::Util::Controls::VBox());
		mainContainer->setBackColor(osgEarth::Util::Controls::Color(osgEarth::Util::Controls::Color::Black, 0.8));
		mainContainer->setHorizAlign(osgEarth::Util::Controls::Control::ALIGN_LEFT);
		mainContainer->setVertAlign(osgEarth::Util::Controls::Control::ALIGN_BOTTOM);
		
		IOSGGraphicsSceneManagerPtr osg_sm = GetScene()->GetFirstSceneManagerByClass<IOSGGraphicsSceneManager>();
		osg::ref_ptr<osg::Group> root = osg_sm->GetOSGRootNode();
		root->addChild(canvas);
		m_GUI = mainContainer;
	}

#if 0
	void OSGEarthSceneManager::SetEarthFile(const std::string &earth_file)
	{
		m_EarthFile = earth_file;
		if(!m_Initlized)
			return;
		if(earth_file != "")
		{
			//disable GLSL
			IOSGGraphicsSceneManagerPtr osg_sm  = GetScene()->GetFirstSceneManagerByClass<IOSGGraphicsSceneManager>();
			osg::ref_ptr<osg::Group> root = osg_sm->GetOSGRootNode();
			ResourceHandle rh(earth_file);
			std::string full_path = rh.GetResource()->Path().GetFullPath();
			//osg::Node* node = osgDB::readNodeFile(full_path);
			//m_MapNode = osgEarth::MapNode::findMapNode(node);
			//root->addChild(m_MapNode);

			//Add dummy component for collisison

			/*BaseSceneComponentPtr  geom_comp(GASS_DYNAMIC_PTR_CAST<BaseSceneComponent>(ComponentFactory::Get().Create("ManualMeshComponent")));
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

	void OSGEarthSceneManager::OnShutdown()
	{

	}

	void OSGEarthSceneManager::FromLatLongToMap(double latitude, double longitude, Vec3 &pos, Quaternion &rot)
	{
		if (m_MapNode)
		{
			const osgEarth::SpatialReference* geoSRS = m_MapNode->getMapSRS()->getGeographicSRS();
			const osgEarth::SpatialReference* mapSRS = m_MapNode->getMapSRS();

			double height = 0;
			//Create geocentric coordinates from lat long, use  Geographic-SRS!
			m_MapNode->getTerrain()->getHeight(0L, geoSRS, longitude, latitude, &height, 0L);
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
	}

	void OSGEarthSceneManager::FromLatLongToMap(double latitude, double longitude, double height, Vec3 &pos, bool relative_height)
	{
		if (m_MapNode)
		{
			if (m_MapNode->isGeocentric())
			{
				const osgEarth::SpatialReference* geoSRS = m_MapNode->getMapSRS()->getGeographicSRS();
				const osgEarth::SpatialReference* mapSRS = m_MapNode->getMapSRS();

				osgEarth::GeoPoint gp(geoSRS, longitude, latitude, height, relative_height ? osgEarth::ALTMODE_RELATIVE : osgEarth::ALTMODE_ABSOLUTE);
				osg::Vec3d ptXYZ;
				osgEarth::GeoPoint map_gp = gp.transform(mapSRS);
				map_gp.toWorld(ptXYZ, m_MapNode->getTerrain());

				pos = OSGConvert::ToGASS(ptXYZ);
			}
			else //Hack to get correct height in projected mode, have to investigate vertical datum usages further
			{
				const osgEarth::SpatialReference* geoSRS = m_MapNode->getMapSRS()->getGeographicSRS();
				const osgEarth::SpatialReference* mapSRS = m_MapNode->getMapSRS();
				
				osgEarth::GeoPoint gp(geoSRS, longitude, latitude, height, relative_height ? osgEarth::ALTMODE_RELATIVE : osgEarth::ALTMODE_ABSOLUTE);
				osg::Vec3d ptXYZ;
				osgEarth::GeoPoint map_gp = gp.transform(mapSRS);
				map_gp.toWorld(ptXYZ, m_MapNode->getTerrain());

				//update with correct height, ptXYZ.z don't match scene height
				//double h_above_msl = 0;
				//if (ground_clamp)
				//	m_MapNode->getTerrain()->getHeight(0L, mapSRS, map_gp.x(), map_gp.y(), &h_above_msl, 0L);
				//ptXYZ.z() = h_above_msl;
				pos = OSGConvert::ToGASS(ptXYZ);
			}
		}
	}

	void OSGEarthSceneManager::FromMapToLatLong(const Vec3 &pos, double &latitude, double &longitude, double &height, double *altitude)
	{
		if (m_MapNode)
		{
			const osgEarth::SpatialReference* geoSRS = m_MapNode->getMapSRS()->getGeographicSRS();
			const osgEarth::SpatialReference* mapSRS = m_MapNode->getMapSRS();
			const osg::Vec3d osg_pos = OSGConvert::ToOSG(pos);
			osgEarth::GeoPoint gp;
			gp.fromWorld(mapSRS, osg_pos);
			const osg::Vec3d ptLatLong = gp.transform(geoSRS).vec3d();
			latitude = ptLatLong.y();
			longitude = ptLatLong.x();
			height = ptLatLong.z();
		
			if (altitude)
			{
				double h_above_msl = 0;
				m_MapNode->getTerrain()->getHeight(0L, geoSRS, longitude, latitude, &h_above_msl, 0L);
				*altitude = height - h_above_msl;
			}
		}
	}

	void OSGEarthSceneManager::WGS84ToScene(double lat, double lon, double &x, double &y)
	{
		Vec3 pos(0,0,0);
		double height = 0;
		FromLatLongToMap(lat, lon, height, pos, false);
	}

	void OSGEarthSceneManager::SceneToWGS84(double x, double y, double &lat, double &lon)
	{
		double height;
		FromMapToLatLong(GASS::Vec3(x, y, 0), lat, lon, height,NULL);
	}

	std::string OSGEarthSceneManager::GetProjection() const
	{
		return m_DummyProjection;
	}

	void OSGEarthSceneManager::SetProjection(const std::string &projection)
	{
		m_DummyProjection = projection;
	}
}