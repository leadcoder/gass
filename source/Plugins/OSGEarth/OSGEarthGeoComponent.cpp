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
#include <osg/PositionAttitudeTransform>
#include <osgGA/NodeTrackerManipulator>
#include <osgGA/TrackballManipulator>
#include <osg/MatrixTransform>
#include <osgShadow/ShadowTechnique>
#include <osgEarth/MapNode>
#include <osgEarth/ECEF>

#include <osgEarthUtil/AnnotationEvents>
#include <osgEarthUtil/ExampleResources>
#include <osgEarthAnnotation/AnnotationEditing>
#include <osgEarthAnnotation/AnnotationRegistry>
#include <osgEarthAnnotation/ImageOverlay>
#include <osgEarthAnnotation/ImageOverlayEditor>
#include <osgEarthAnnotation/CircleNode>
#include <osgEarthAnnotation/RectangleNode>
#include <osgEarthAnnotation/EllipseNode>
#include <osgEarthAnnotation/PlaceNode>
#include <osgEarthAnnotation/LabelNode>
#include <osgEarthAnnotation/LocalGeometryNode>
#include <osgEarthAnnotation/FeatureNode>
//#include <osgEarthAnnotation/HighlightDecoration>
//#include <osgEarthAnnotation/ScaleDecoration>
#include <osgEarthSymbology/GeometryFactory>
#include <osg/Camera>


#include "OSGEarthGeoComponent.h"
#include "OSGEarthSceneManager.h"

#include "Plugins/OSG/OSGNodeMasks.h"
#include "Plugins/OSG/OSGConvert.h"
#include "Plugins/OSG/IOSGGraphicsSceneManager.h"
#include "Plugins/OSG/IOSGGraphicsSystem.h"



namespace GASS
{
	OSGEarthGeoComponent::OSGEarthGeoComponent() : m_Latitude(0),
		m_Longitude(0),
		m_HeightAboveMSL(0),
		m_HeightAboveGround(0),
		m_OESM(NULL)
	{

	}

	OSGEarthGeoComponent::~OSGEarthGeoComponent()
	{

	}

	void OSGEarthGeoComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("OSGEarthGeoComponent",new Creator<OSGEarthGeoComponent, Component>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("Component used to handle object position, rotation", OF_VISIBLE)));
		RegisterProperty<double>("Latitude", &OSGEarthGeoComponent::GetLatitude, &OSGEarthGeoComponent::SetLatitude,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("", PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<double>("Longitude", &OSGEarthGeoComponent::GetLongitude, &OSGEarthGeoComponent::SetLongitude,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("", PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<double>("HeightAboveMSL", &OSGEarthGeoComponent::GetHeightAboveMSL, &OSGEarthGeoComponent::SetHeightAboveMSL,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("", PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<double>("HeightAboveGround", &OSGEarthGeoComponent::GetHeightAboveGround, &OSGEarthGeoComponent::SetHeightAboveGround,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("", PF_VISIBLE | PF_EDITABLE)));
	}
	
	void OSGEarthGeoComponent::OnDelete()
	{

	}

	void OSGEarthGeoComponent::OnInitialize()
	{

		GASS_SHARED_PTR<OSGEarthSceneManager> earth_sm = GASS_DYNAMIC_PTR_CAST<OSGEarthSceneManager>(GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OSGEarthSceneManager>());
		if (!earth_sm)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to find OSGEarthSceneManager", "OSGEarthGeoComponent::OnInitialize");

		m_OESM = earth_sm.get();

		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGEarthGeoComponent::OnTransformation, TransformationChangedEvent, 0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGEarthGeoComponent::OnLocationLoaded, LocationLoadedEvent, 0));

	}


	void OSGEarthGeoComponent::OnLocationLoaded(LocationLoadedEventPtr message)
	{
		//update altitude
		//if (m_Latitude != 0 && m_Longitude != 0)
		//	SetAltitude(m_InitialAlt);
	}

	double OSGEarthGeoComponent::GetLatitude() const
	{ 
		return m_Latitude;
	}

	void OSGEarthGeoComponent::SetLatitude(double lat)
	{ 
		m_Latitude = lat;
		UpdateNode();
	}

	double OSGEarthGeoComponent::GetLongitude() const
	{ 
		return m_Longitude;
	}

	void OSGEarthGeoComponent::SetLongitude(double lat)
	{ 
		m_Longitude = lat;
		UpdateNode();
	}

	void OSGEarthGeoComponent::SetHeightAboveGround(double value)
	{
		const double terrain_height = m_HeightAboveMSL - m_HeightAboveGround;
		m_HeightAboveGround = value;
		m_HeightAboveMSL = terrain_height + value;
		//note that m_HeightAboveMSL will later be update on TransformationChangedEvent 
		UpdateNode();
	}

	double OSGEarthGeoComponent::GetHeightAboveGround() const
	{
		return m_HeightAboveGround;
	}

	void OSGEarthGeoComponent::SetHeightAboveMSL(double value)
	{
		const double terrain_height = m_HeightAboveMSL - m_HeightAboveGround;
		//we need to udpate m_HeightAboveGround because it's used in UpdateNode, TODO: to height updates in own method
		m_HeightAboveGround = value - terrain_height;
		m_HeightAboveMSL = value;
		UpdateNode();
	}

	double OSGEarthGeoComponent::GetHeightAboveMSL() const
	{
		return m_HeightAboveMSL;
	}

	void OSGEarthGeoComponent::OnTransformation(TransformationChangedEventPtr message)
	{
		m_OESM->FromMapToLatLong(message->GetPosition(), m_Latitude, m_Longitude, m_HeightAboveMSL , &m_HeightAboveGround);
	}

	void OSGEarthGeoComponent::UpdateNode()
	{
		if (m_OESM)
		{
			Vec3 pos;
			m_OESM->FromLatLongToMap(m_Latitude, m_Longitude, m_HeightAboveGround, pos, true);
			GetSceneObject()->PostRequest(WorldPositionRequestPtr(new WorldPositionRequest(pos)));
			//GetSceneObject()->PostRequest(BaseRotationRequestPtr(new BaseRotationRequest(rot)));
		}
	}	
}

