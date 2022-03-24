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
#include "Sim/GASSGeoLocation.h"

namespace GASS
{
	class OSGEarthSceneManager;

	class OSGEarthGeoComponent : public Reflection<OSGEarthGeoComponent,Component> , public IWorldLocationComponent
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
		Vec3 GetWorldPosition() const;
		void LatOrLongChanged();
		void SetWorldPosition(const Vec3& pos);
		void OnTransformation(TransformationChangedEventPtr event);
		void OSGEarthGeoComponent::OnTerrainChanged(TerrainChangedEventPtr event);
		bool m_PreserveHAG{true};

		GeoLocation m_Location;
		double m_HeightAboveGround{0};
		OSGEarthSceneManager* m_OESM{nullptr};
		ILocationComponent* m_LocationComp{nullptr};
		bool m_HandleTransformations{true};
	};
	using OSGEarthGeoComponentWeakPtr = std::weak_ptr<OSGEarthGeoComponent>;
	using OSGEarthGeoComponentPtr = std::shared_ptr<OSGEarthGeoComponent>;
}