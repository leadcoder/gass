/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                        *
*                                                                           *
* Copyright (c) 2008-2015 GASS team. See Contributors.txt for details.      *
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

#include "Sim/GASSCommon.h"
#include "Sim/Interface/GASSITerrainComponent.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/GASSSceneObjectRef.h"

#include "Sim/Messages/GASSPlatformMessages.h"
#include "Plugins/Base/CoreMessages.h"


namespace GASS
{

	enum PathFollowMode
	{
		PFM_STOP_AT_END,
		PFM_LOOP_TO_START,
		PFM_REVERSE_LOOP
	};


	START_ENUM_BINDER(PathFollowMode,PathFollowModeBinder)
		BIND(PFM_STOP_AT_END)
		BIND(PFM_LOOP_TO_START)
		BIND(PFM_REVERSE_LOOP)
	END_ENUM_BINDER(PathFollowMode,PathFollowModeBinder)


	class FollowWaypointListComponent :  public Reflection<FollowWaypointListComponent,BaseSceneComponent>
	{
	public:
		FollowWaypointListComponent();
		virtual ~FollowWaypointListComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();
		void SceneManagerTick(double delta);
		std::vector<SceneObjectPtr>  GetWaypointListEnumeration() const;
		std::vector<SceneObjectPtr>  GetNavigationEnumeration() const;
	private:
		void OnWaypointListUpdated(WaypointListUpdatedMessagePtr message);
		void OnTransMessage(TransformationChangedEventPtr message);
		ADD_PROPERTY(Float,WaypointRadius);
		ADD_PROPERTY(SceneObjectRef,NavigationObject);
		void SetInvertDirection(bool value);
		bool GetInvertDirection() const;
		void SetWaypointList(SceneObjectRef waypointlist);
		SceneObjectRef GetWaypointList() const;
		PathFollowModeBinder GetMode() const;
		void SetMode(const PathFollowModeBinder &mode);
		int GetCloesetWaypoint();
		
		
		Vec3  m_CurrentPos;
		std::vector<Vec3> m_Waypoints;
		SceneObjectRef m_WaypointList;
		float m_Direction;
		bool m_InvertDirection;
		bool m_HasWaypoints;
		int m_CurrentWaypoint;
		PathFollowModeBinder m_Mode;
	};
}
#endif
