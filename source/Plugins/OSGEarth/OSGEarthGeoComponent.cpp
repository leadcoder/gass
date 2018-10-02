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

#include "OSGEarthGeoComponent.h"
#include "OSGEarthSceneManager.h"
#include "Plugins/OSG/OSGNodeMasks.h"
#include "Plugins/OSG/OSGConvert.h"
#include "Plugins/OSG/IOSGGraphicsSceneManager.h"
#include "Plugins/OSG/IOSGGraphicsSystem.h"

namespace GASS
{
	OSGEarthGeoComponent::OSGEarthGeoComponent() : m_HeightAboveGround(0),
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
	}

	double OSGEarthGeoComponent::GetLatitude() const
	{ 
		return m_Location.Latitude;
	}

	void OSGEarthGeoComponent::SetLatitude(double lat)
	{ 
		m_Location.Latitude = lat;
		_LatOrLongChanged();
	}

	double OSGEarthGeoComponent::GetLongitude() const
	{ 
		return m_Location.Longitude;
	}

	void OSGEarthGeoComponent::SetLongitude(double lat)
	{ 
		m_Location.Longitude = lat;
		_LatOrLongChanged();
	}

	Vec3 OSGEarthGeoComponent::_GetWorldPosition() const
	{
		return GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
	}

	void OSGEarthGeoComponent::_SetWorldPosition(const Vec3& pos)
	{
		GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->SetWorldPosition(pos);
	}

	void OSGEarthGeoComponent::SetHeightAboveGround(double value)
	{
		m_HeightAboveGround = value;
		if (m_OESM)
		{
			//first update values from from current location
			m_OESM->SceneToWGS84(_GetWorldPosition(), m_Location);
			double terrain_height = 0;
			if(m_OESM->GetTerrainHeight(m_Location, terrain_height))
			{
				//update world location with new height
				m_Location.Height = terrain_height + m_HeightAboveGround;
				Vec3 pos;
				if(m_OESM->WGS84ToScene(m_Location,pos))
					_SetWorldPosition(pos);
			}
		}
	}

	double OSGEarthGeoComponent::GetHeightAboveGround() const
	{
		return m_HeightAboveGround;
	}

	void OSGEarthGeoComponent::SetHeightAboveMSL(double value)
	{
		//first update values from from current location
		if (m_OESM)
		{
			//first update geolocation from current world location
			if (m_OESM->SceneToWGS84(_GetWorldPosition(), m_Location))
			{
				m_Location.Height = value;
				Vec3 pos;
				if (m_OESM->WGS84ToScene(m_Location, pos))
				{
					_SetWorldPosition(pos);
					//update Height above ground
					m_OESM->GetHeightAboveTerrain(m_Location, m_HeightAboveGround);
				}
			}
		}
	}

	double OSGEarthGeoComponent::GetHeightAboveMSL() const
	{
		return m_Location.Height;
	}

	void OSGEarthGeoComponent::OnTransformation(TransformationChangedEventPtr message)
	{
		if (m_OESM->SceneToWGS84(message->GetPosition(), m_Location))
		{
			m_OESM->GetHeightAboveTerrain(m_Location, m_HeightAboveGround);
		}
	}

	void OSGEarthGeoComponent::_LatOrLongChanged()
	{
		if (m_OESM)
		{
			Vec3 pos;
			//preserve height above ground at new location
			GeoLocation relative_height(m_Location.Longitude,m_Location.Latitude, m_HeightAboveGround,true);
			if (m_OESM->WGS84ToScene(relative_height, pos))
			{
				_SetWorldPosition(pos);
				
			}
			
		}
	}	
}

