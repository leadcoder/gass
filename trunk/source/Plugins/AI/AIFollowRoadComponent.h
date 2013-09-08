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

#ifndef AI_FOLLOW_ROAD_H
#define AI_FOLLOW_ROAD_H


#include "Sim/GASSCommon.h"
#include "Sim/Interface/GASSITerrainComponent.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"


#include "Plugins/Game/GameMessages.h"
#include "Plugins/Base/CoreMessages.h"
#include "AIRoadIntersectionComponent.h"

namespace GASS
{
	class AIRoadSegmentComponent;
	typedef SPTR<AIRoadComponent> AIRoadComponentPtr;
	class LaneVehicle;

	class AIFollowRoadComponent :  public Reflection<AIFollowRoadComponent,BaseSceneComponent>
	{
	public:
		AIFollowRoadComponent();
		virtual ~AIFollowRoadComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();
		void SceneManagerTick(double delta);
	private:
		ADD_ATTRIBUTE(int,LaneBufferSize);
		ADD_ATTRIBUTE(Vec2,RandomSpeed);
		
		AIRoadLaneComponentPtr GetFreeLane(AIRoadLaneComponentPtr lane);
		void UpdateLaneBuffer();
		void OnSpawnOnRoad(SpawnOnRoadMessagePtr message);
		void GoToIntersection(AIRoadIntersectionComponentPtr inter);
		Vec3 GetClosestPointOnPath(const Vec3& source_pos , Float look_ahead, const std::vector<Vec3> &wps);
		void OnWaypointListUpdated(WaypointListUpdatedMessagePtr message);
		void OnTransMessage(TransformationNotifyMessagePtr message);
		void OnPhysicsMessage(VelocityNotifyMessagePtr message);
		ADD_ATTRIBUTE(Float,WaypointRadius);
		void SetInvertDirection(bool value);
		bool GetInvertDirection() const;
		AIRoadComponentPtr GetFreeRoad(AIRoadComponentPtr road);
		Vec3  m_CurrentPos;
		float m_Direction;
		bool m_InvertDirection;
		bool m_HasWaypoints;
		int m_CurrentWaypoint;
		
		AIRoadIntersectionComponentPtr m_CurrentIntersection;
		AIRoadComponentPtr m_CurrentRoad;
		AIRoadComponentPtr m_NextRoad;

		AIRoadLaneComponentPtr m_CurrentLane;

		std::vector<AIRoadLaneComponentPtr> m_LaneBuffer;

		Vec3 m_AngularVelocity;
		Vec3 m_VehicleSpeed;
		int m_DebugReset;
		std::vector<Vec3> m_CurrentPath;
		Float m_CurrentDistanceOnPath;
		Float m_CurrentDistanceToPath;
		LaneObject* m_LaneObject;
		Float m_TargetSpeed;
	};
}
#endif
