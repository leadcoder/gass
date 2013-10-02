/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                 *
*                                                                           *
* Copyright (c) 2008-2009 GASS team. See Contributors.txt for details.      *
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
#include "Core/Common.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Math/GASSVector.h"
#include "Core/Utils/GASSColorRGB.h"
#include "Sim/GASSCommon.h"
#include "Sim/Interface/GASSICameraComponent.h"
#include "Sim/Interface/GASSIWaypointListComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Plugins/Base/CoreMessages.h"

namespace GASS
{
	class SplineAnimation;
	class WaypointListComponent : public Reflection<WaypointListComponent,BaseSceneComponent>, public IWaypointListComponent
	{
		friend class WaypointComponent;
	public:
		WaypointListComponent();
		virtual ~WaypointListComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual std::vector<Vec3> GetWaypoints(bool relative_position = true) const;
		virtual std::string GetWaypointTemplate() const;
		float GetRadius()const;
	protected:
		ADD_PROPERTY(bool,ShowPathLine);
		ADD_PROPERTY(bool,Closed);
		
		
		void SetRadius(float radius);
		int GetSplineSteps()const;
		void SetSplineSteps(int steps);

		
		void SetWaypointTemplate(const std::string &name);
		
		bool GetEnableSpline()const;
		void SetEnableSpline(bool value);

		void SetShowWaypoints(bool value);
		bool GetShowWaypoints() const;

		bool GetAutoUpdateTangents()const;
		void SetAutoUpdateTangents(bool value);

		
		

		void SetExport(const FilePath &filename);
		FilePath GetExport() const;
		void RecursiveIncreaseResolution(const Vec3& line_start,  const Vec3& line_end, SplineAnimation &spline, Float min_dist) const;
		void OnMoved(TransformationNotifyMessagePtr message);
		void OnUpdate(UpdateWaypointListMessagePtr message);
		//Helpers
		
		void UpdatePath();

		float m_Radius;
		int m_SplineSteps;
		bool m_EnableSpline;
		bool m_Initialized;
		bool m_AutoUpdateTangents;
		bool m_ShowWaypoints;
		ColorRGBA m_LineColor;
		std::string m_WaypointTemplate;
	};

	//typedef SPTR<WaypointListComponent> WaypointListComponentPtr;
	//typedef WPTR<WaypointListComponent> WaypointListComponentWeakPtr;
	
}

