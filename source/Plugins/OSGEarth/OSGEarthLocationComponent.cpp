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
#include "OSGEarthLocationComponent.h"

#include <memory>
#include "Plugins/OSG/OSGNodeMasks.h"
#include "Plugins/OSG/OSGConvert.h"
#include "Plugins/OSG/IOSGGraphicsSceneManager.h"
#include "Plugins/OSG/IOSGGraphicsSystem.h"


namespace GASS
{
	OSGEarthLocationComponent::OSGEarthLocationComponent() 
		
	{

	}

	OSGEarthLocationComponent::~OSGEarthLocationComponent()
	{

	}

	void OSGEarthLocationComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register<OSGEarthLocationComponent>();
		GetClassRTTI()->SetMetaData(std::make_shared<ClassMetaData>("Component used to handle object position, rotation", OF_VISIBLE));
		RegisterGetSet("Latitude", &OSGEarthLocationComponent::GetLatitude, &OSGEarthLocationComponent::SetLatitude, PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("Longitude", &OSGEarthLocationComponent::GetLongitude, &OSGEarthLocationComponent::SetLongitude, PF_VISIBLE | PF_EDITABLE,"");
		RegisterMember("Offset", &OSGEarthLocationComponent::m_Offset, PF_VISIBLE | PF_EDITABLE,"");
	}
	
	void OSGEarthLocationComponent::OnDelete()
	{

	}

	void OSGEarthLocationComponent::OnInitialize()
	{
		IOSGGraphicsSceneManagerPtr osg_sm  = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<IOSGGraphicsSceneManager>();
		osg::ref_ptr<osg::Group> root = osg_sm->GetOSGRootNode();
		//osg::Group* root = static_cast<osg::Group*> (message->GetUserData());
		osgEarth::MapNode* map_node = osgEarth::MapNode::findMapNode(root);
		if(map_node)
		{
			m_MapNode = map_node;
			//const osgEarth::SpatialReference* geoSRS = mapNode->getMapSRS()->getGeographicSRS();
			/*osgEarth::Style labelStyle;
			labelStyle.getOrCreate<osgEarth::TextSymbol>()->alignment() = osgEarth::TextSymbol::ALIGN_CENTER_CENTER;
			labelStyle.getOrCreate<osgEarth::TextSymbol>()->fill()->color() = osgEarth::Color::Yellow;
			m_DebugNode = new osgEarth::Annotation::PlaceNode(mapNode, osgEarth::GeoPoint(geoSRS, -80.28, 25.82), "Miami", labelStyle);
			m_DebugNode->setDynamic(true);
			root->addChild(m_DebugNode);
			*/
			//Get lat lon from location
			UpdateNode();
		
		}

		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGEarthLocationComponent::OnTransformation, TransformationChangedEvent, 0));
		
		if(m_DebugNode)
		{
			GetSceneObject()->RegisterForMessage(REG_TMESS(OSGEarthLocationComponent::OnNameChanged, SceneObjectNameChangedEvent,0));
		}
	}

	double OSGEarthLocationComponent::GetLatitude() const
	{ 
		return m_Latitude;
	}

	void OSGEarthLocationComponent::SetLatitude(double lat)
	{ 
		m_Latitude = lat;
		UpdateNode();
	}

	double OSGEarthLocationComponent::GetLongitude() const
	{ 
		return m_Longitude;
	}

	void OSGEarthLocationComponent::SetLongitude(double lat)
	{ 
		m_Longitude = lat;
		UpdateNode();
	}

	void OSGEarthLocationComponent::OnTransformation(TransformationChangedEventPtr message)
	{
		const osgEarth::SpatialReference* geo_srs = m_MapNode->getMapSRS()->getGeographicSRS();
		osg::Vec3d pos = OSGConvert::ToOSG(message->GetPosition());
		osgEarth::GeoPoint map_pos;
		map_pos.fromWorld(geo_srs,pos);
		m_Latitude = map_pos.y();
		m_Longitude = map_pos.x();
	}

	void OSGEarthLocationComponent::UpdateNode()
	{
		if(m_MapNode)
		{
			 const osgEarth::SpatialReference* geo_srs = m_MapNode->getMapSRS()->getGeographicSRS();
			 const osgEarth::SpatialReference* map_srs = m_MapNode->getMapSRS();
			 
			 double height = 0;

			 //Create geocentric coordinates from lat long, use  Geographic-SRS!
			 m_MapNode->getTerrain()->getHeight(nullptr, geo_srs,m_Longitude,m_Latitude, &height, nullptr);
			 osgEarth::GeoPoint map_point(geo_srs, m_Longitude, m_Latitude, height,osgEarth::ALTMODE_ABSOLUTE);

			 height += m_Offset;
			 
			 //Transform geocentric coordinates to map-space using map-SRS!
			 osgEarth::GeoPoint map_pos = map_point.transform(map_srs);
			 osg::Matrixd out_local2world;
			 map_pos.createLocalToWorld(out_local2world);
			 
			 //osg::Quat osg_rot = out_local2world.getRotate();
			 osg::Vec3d osg_pos = out_local2world.getTrans();
			 
			 GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->SetWorldPosition(OSGConvert::ToGASS(osg_pos));
			//GetSceneObject()->PostRequest(std::make_shared<BaseRotationRequest>(OSGConvert::ToGASS(osg_rot)));
			 if (m_DebugNode)
				 m_DebugNode->setPosition(map_point);
		}
	}

	void OSGEarthLocationComponent::OnNameChanged(SceneObjectNameChangedEventPtr event)
	{
		if(m_DebugNode)
			m_DebugNode->setText(event->GetName());
	}

	
}

