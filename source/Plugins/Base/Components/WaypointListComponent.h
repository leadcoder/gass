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
#include "Core/Common.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Math/GASSVector.h"
#include "Core/Utils/GASSColorRGBA.h"
#include "Sim/Interface/GASSIWaypointListComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"

namespace GASS
{
	class SplineAnimation;

	/**
		Component that handles waypoints lists. 
	*/

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
		void OnPostInitializedEvent(PostInitializedEventPtr message);
		ADD_PROPERTY(bool,ShowPathLine);
		ADD_PROPERTY(bool,Closed);
		ADD_PROPERTY(bool,AutoRotateWaypoints);
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
		//Helpers
		void UpdatePath();

		SceneObjectPtr _GetConnectionLines() const {return m_ConnectionLines.lock();}

		float m_Radius;
		int m_SplineSteps;
		bool m_EnableSpline;
		bool m_Initialized;
		bool m_AutoUpdateTangents;
		bool m_ShowWaypoints;
		ColorRGBA m_LineColor;
		std::string m_WaypointTemplate;
		SceneObjectWeakPtr m_ConnectionLines;
	};

	//typedef GASS_SHARED_PTR<WaypointListComponent> WaypointListComponentPtr;
	//typedef GASS_WEAK_PTR<WaypointListComponent> WaypointListComponentWeakPtr;
	
}

