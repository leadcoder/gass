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

#ifndef FOLLOW_WAYPOINT_LIST_H
#define FOLLOW_WAYPOINT_LIST_H

#include "Sim/Components/Graphics/Geometry/ITerrainComponent.h"
#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Scenario/Scene/Messages/GraphicsSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/PhysicsSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/CoreSceneObjectMessages.h"
#include "Sim/Common.h"
#include "Sim/Scheduling/ITaskListener.h"
#include "Sim/Systems/Input/ControlSetting.h"
#include "Plugins/Game/GameMessages.h"
#include "Plugins/Base/CoreMessages.h"


namespace GASS
{
	class FollowWaypointListComponent :  public Reflection<FollowWaypointListComponent,BaseSceneComponent>, public ITaskListener
	{
	public:
		enum PathFollowMode
		{
			PFM_STOP_AT_END,
			PFM_LOOP_TO_START,
			PFM_REVERSE_LOOP
		};
		FollowWaypointListComponent();
		virtual ~FollowWaypointListComponent();
		static void RegisterReflection();
		virtual void OnCreate();

		//ITaskListener interface
		void Update(double delta);
		TaskGroup GetTaskGroup() const;
	private:
		ADD_ATTRIBUTE(Float,WaypointRadius);
		void OnWaypointListUpdated(WaypointListUpdatedMessagePtr message);
		void SetWaypointList(const std::string &waypointlist);
		std::string GetWaypointList() const;
		std::string GetMode() const;
		void SetMode(const std::string &mode);

		void OnLoad(LoadGameComponentsMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		void OnTransMessage(TransformationNotifyMessagePtr message);
		
		Vec3  m_CurrentPos;

		std::vector<Vec3> m_Waypoints;
		std::string m_WaypointListName;
		float m_Direction;
	
		bool m_HasWaypoints;
		int m_CurrentWaypoint;
		PathFollowMode m_Mode;
	};
}
#endif
