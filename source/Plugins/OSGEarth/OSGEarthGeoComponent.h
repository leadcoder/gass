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
#pragma once

#include "Plugins/OSG/IOSGCameraManipulator.h"
#include "Sim/Messages/GASSGeoSceneObjectMessages.h"
#include "Sim/GASSGeoLocation.h"

namespace GASS
{
	class OSGEarthSceneManager;

	class OSGEarthGeoComponent : public Reflection<OSGEarthGeoComponent,BaseSceneComponent> , public IWorldLocationComponent
	{
	public:
		OSGEarthGeoComponent();
		~OSGEarthGeoComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;

		//IWorldLocationComponent
		double GetLatitude() const override;
		void SetLatitude(double lat) override;
		double GetLongitude() const override;
		void SetLongitude(double lat) override;
		void SetHeightAboveMSL(double value) override;
		double GetHeightAboveMSL() const override;
		void SetHeightAboveGround(double value) override;
		double GetHeightAboveGround() const override;
	protected:
		Vec3 _GetWorldPosition() const;
		void _LatOrLongChanged();
		void _SetWorldPosition(const Vec3& pos);
		void OnTransformation(TransformationChangedEventPtr event);
		void OSGEarthGeoComponent::OnTerrainChanged(TerrainChangedEventPtr event);
		bool m_PreserveHAG;

		GeoLocation m_Location;
		double m_HeightAboveGround;
		OSGEarthSceneManager* m_OESM;
		ILocationComponent* m_LocationComp;
		bool m_HandleTransformations;
	};
	typedef GASS_WEAK_PTR<OSGEarthGeoComponent> OSGEarthGeoComponentWeakPtr;
	typedef GASS_SHARED_PTR<OSGEarthGeoComponent> OSGEarthGeoComponentPtr;
}