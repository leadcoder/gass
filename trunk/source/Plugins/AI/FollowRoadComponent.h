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

#ifndef FOLLOW_ROAD_H
#define FOLLOW_ROAD_H


#include "Sim/GASSCommon.h"
#include "Sim/Interface/GASSITerrainComponent.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"


#include "Plugins/Game/GameMessages.h"
#include "Plugins/Base/CoreMessages.h"
#include "RoadIntersectionComponent.h"

namespace GASS
{
	class RoadSegmentComponent;
	typedef SPTR<RoadSegmentComponent> RoadSegmentComponentPtr;

	class LaneVehicle;

	

	class FollowRoadComponent :  public Reflection<FollowRoadComponent,BaseSceneComponent>
	{
	public:
		FollowRoadComponent();
		virtual ~FollowRoadComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();
		void SceneManagerTick(double delta);
	private:
		void OnSpawnOnRoad(SpawnOnRoadMessagePtr message);
		void GoToIntersection(RoadIntersectionComponentPtr inter);
		Vec3 GetClosestPointOnPath(const Vec3& source_pos , Float look_ahead, const std::vector<Vec3> &wps);
		void OnWaypointListUpdated(WaypointListUpdatedMessagePtr message);
		void OnTransMessage(TransformationChangedEventPtr message);
		void OnPhysicsMessage(VelocityNotifyMessagePtr message);
		ADD_PROPERTY(Float,WaypointRadius);
		void SetInvertDirection(bool value);
		bool GetInvertDirection() const;
		RoadSegmentComponentPtr GetFreeRoad(RoadSegmentComponentPtr road);
		
		
		Vec3  m_CurrentPos;
		//std::vector<Vec3> m_Waypoints;

		float m_Direction;
		bool m_InvertDirection;
		bool m_HasWaypoints;
		int m_CurrentWaypoint;
		//PathFollowMode m_Mode;
		RoadIntersectionComponentPtr m_CurrentIntersection;
		RoadSegmentComponentPtr m_CurrentRoad;
		RoadSegmentComponentPtr m_NextRoad;

		TurnDir m_Turn;

		LaneVehicle* m_RoadVehicle;

		Vec3 m_AngularVelocity;
		Vec3 m_VehicleSpeed;

		int m_DebugReset;

	};
}
#endif
