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
#include "OSGEarthGeoComponent.h"

#include <memory>
#include "OSGEarthSceneManager.h"
#include "Plugins/OSG/OSGNodeMasks.h"
#include "Plugins/OSG/OSGConvert.h"
#include "Plugins/OSG/IOSGGraphicsSceneManager.h"
#include "Plugins/OSG/IOSGGraphicsSystem.h"

namespace GASS
{
	OSGEarthGeoComponent::OSGEarthGeoComponent() 
		
	{

	}

	OSGEarthGeoComponent::~OSGEarthGeoComponent()
	{

	}

	void OSGEarthGeoComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<OSGEarthGeoComponent>();
		GetClassRTTI()->SetMetaData(std::make_shared<ClassMetaData>("Component used to handle object position, rotation", OF_VISIBLE));
		RegisterGetSet("Latitude", &OSGEarthGeoComponent::GetLatitude, &OSGEarthGeoComponent::SetLatitude, PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("Longitude", &OSGEarthGeoComponent::GetLongitude, &OSGEarthGeoComponent::SetLongitude, PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("HeightAboveMSL", &OSGEarthGeoComponent::GetHeightAboveMSL, &OSGEarthGeoComponent::SetHeightAboveMSL, PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("HeightAboveGround", &OSGEarthGeoComponent::GetHeightAboveGround, &OSGEarthGeoComponent::SetHeightAboveGround, PF_VISIBLE | PF_EDITABLE,"");
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

		LocationComponentPtr lc = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		if(!lc)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to find ILocationComponent", "OSGEarthGeoComponent::OnInitialize");

		m_LocationComp = lc.get();
#if 1
		if (fabs(GetLatitude()) > 0.00000001) //assume we have valid location, lat long, height
		{
#if 1
			if (!m_LocationComp->HasParentLocation()) //top node?
			{
				_LatOrLongChanged(); //update location component to reflect current terrain elevation
			}
#else
			const Vec3 pos = m_LocationComp->GetPosition();
			const double half_earth_radius = 6371000.0 / 2.0;
			if(pos.Length() > half_earth_radius) //assume this is geocentric world coordinates
				_LatOrLongChanged(); //update location component to reflect current terrain elevation
#endif
		}
#endif 
		GetSceneObject()->RegisterForMessage(REG_TMESS(OSGEarthGeoComponent::OnTransformation, TransformationChangedEvent, 0));
		GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS(OSGEarthGeoComponent::OnTerrainChanged, TerrainChangedEvent, 0));
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
		return m_LocationComp->GetWorldPosition();
	}

	void OSGEarthGeoComponent::_SetWorldPosition(const Vec3& pos)
	{
		m_HandleTransformations = false;
		m_LocationComp->SetWorldPosition(pos);
		m_HandleTransformations = true;
	}

	void OSGEarthGeoComponent::SetHeightAboveGround(double value)
	{
		m_HeightAboveGround = value;
		if (m_OESM)
		{
			//first update values from from current location
			m_OESM->SceneToWGS84(_GetWorldPosition(), m_Location);
			double terrain_height = 0;
			if(m_OESM->GetTerrainHeight(m_Location, terrain_height, GEOMETRY_FLAG_GROUND_LOD))
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
		//double hag = m_HeightAboveGround;
		//if(m_OESM)
		//	m_OESM->GetHeightAboveTerrain(m_Location, hag);
		//return hag;
	}

	void OSGEarthGeoComponent::SetHeightAboveMSL(double value)
	{
		//first update values from from current location
		m_Location.Height = value;
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
					m_OESM->GetHeightAboveTerrain(m_Location, m_HeightAboveGround, GEOMETRY_FLAG_GROUND_LOD);
				}
			}
		}
	}

	double OSGEarthGeoComponent::GetHeightAboveMSL() const
	{
		return m_Location.Height;
	}

	void OSGEarthGeoComponent::OnTransformation(TransformationChangedEventPtr event)
	{
		if (m_HandleTransformations)
		{
			//update location
			if (m_OESM->SceneToWGS84(event->GetPosition(), m_Location))
			{
				//update HAG
				m_OESM->GetHeightAboveTerrain(m_Location, m_HeightAboveGround, GEOMETRY_FLAG_GROUND_LOD);
			}
		}
	}

	void OSGEarthGeoComponent::OnTerrainChanged(TerrainChangedEventPtr event)
	{
		if (!m_LocationComp->HasParentLocation() && m_PreserveHAG)
		{
			//_LatOrLongChanged(); //trig new height values
			SetHeightAboveGround(m_HeightAboveGround);
		}
	}

	void OSGEarthGeoComponent::_LatOrLongChanged()
	{
		if (m_OESM)
		{
			Vec3 pos;

			if (m_PreserveHAG) //preserve height above ground at new location
			{
				double terrain_height = 0;
				if (m_OESM->GetTerrainHeight(m_Location, terrain_height, GEOMETRY_FLAG_GROUND_LOD))
					m_Location.Height = m_HeightAboveGround + terrain_height;
			}
			else // updated height above ground
			{
				m_OESM->GetHeightAboveTerrain(m_Location, m_HeightAboveGround, GEOMETRY_FLAG_GROUND_LOD);
			}
		
			if (m_OESM->WGS84ToScene(m_Location, pos))
			{
				_SetWorldPosition(pos);
			}
		}
	}	
}

