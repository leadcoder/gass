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


namespace GASS
{
	class OSGEarthSceneManager;

	class OSGEarthGeoComponent : public Reflection<OSGEarthGeoComponent,BaseSceneComponent> 
	{
	public:
		OSGEarthGeoComponent();
		virtual ~OSGEarthGeoComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();
		double GetLatitude() const;
		void SetLatitude(double lat);
		double GetLongitude() const;
		void SetLongitude(double lat);
		double GetAltitude() const;
		void SetAltitude(double alt);
		double GetHeightAboveMSL() const;
		void SetHeightAboveMSL(double height);
	protected:
		
		void OnTransformation(TransformationChangedEventPtr message);
		void OnLocationLoaded(LocationLoadedEventPtr message);
		void UpdateNode();
		double m_Latitude;
		double m_Longitude;
		double m_HeightAboveMSL;
		double m_Altitude;
		double m_InitialAlt;
		OSGEarthSceneManager* m_OESM;
	};
	typedef GASS_WEAK_PTR<OSGEarthGeoComponent> OSGEarthGeoComponentWeakPtr;
	typedef GASS_SHARED_PTR<OSGEarthGeoComponent> OSGEarthGeoComponentPtr;
}