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
#include "Sim/GASSComponent.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"

namespace GASS
{
	class SplineAnimation;

	/**
		Component that handles waypoints lists. 
	*/

	class WaypointListComponent : public Reflection<WaypointListComponent,Component>, public IWaypointListComponent
	{
		friend class WaypointComponent;
	public:
		WaypointListComponent();
		~WaypointListComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		std::vector<Vec3> GetWaypoints(bool relative_position = true) const override;
		std::string GetWaypointTemplate() const override;
		float GetRadius()const override;
	protected:
		void OnPostInitializedEvent(PostInitializedEventPtr message);
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

		SceneObjectPtr GetConnectionLines() const {return m_ConnectionLines.lock();}

		float m_Radius{0};
		int m_SplineSteps{10};
		bool m_EnableSpline{false};
		bool m_Initialized{false};
		bool m_AutoUpdateTangents{true};
		bool m_ShowWaypoints{true};
		ColorRGBA m_LineColor;
		std::string m_WaypointTemplate;
		SceneObjectWeakPtr m_ConnectionLines;
		bool m_ShowPathLine{false};
		bool m_Closed{false};
		bool m_AutoRotateWaypoints{false};
	};
}

