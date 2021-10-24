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
	class OSGEarthLocationComponent : public Reflection<OSGEarthLocationComponent,BaseSceneComponent> 
	{
	public:
		OSGEarthLocationComponent();
		~OSGEarthLocationComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;
		double GetLatitude() const;
		void SetLatitude(double lat);
		double GetLongitude() const;
		void SetLongitude(double lat);
	protected:
		
		void OnTransformation(TransformationChangedEventPtr message);
		void OnGeoLocationRequest(GeoLocationRequestPtr message);
		void OnNameChanged(SceneObjectNameChangedEventPtr event);
		void OnCaptionChanged(TextCaptionRequestPtr message);
		void UpdateNode();
		double m_Latitude{0};
		double m_Longitude{0};
		double m_Offset{0};
		osgEarth::PlaceNode* m_DebugNode{NULL};
		osg::ref_ptr<osgEarth::MapNode> m_MapNode;
	};
	using OSGEarthLocationComponentWeakPtr = std::weak_ptr<OSGEarthLocationComponent>;
	using OSGEarthLocationComponentPtr = std::shared_ptr<OSGEarthLocationComponent>;
}