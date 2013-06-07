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

#include "AIFollowRoadComponent.h"
#include "Plugins/AI/AISceneManager.h"
#include "Plugins/AI/RoadSegmentComponent.h"
#include "Plugins/AI/AIRoadIntersectionComponent.h"

#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include <limits>
#undef min
#undef max

namespace GASS
{
	AIFollowRoadComponent::AIFollowRoadComponent()  : 
		m_WaypointRadius( 4),
		m_HasWaypoints(false),
		m_CurrentWaypoint(-1),
		m_Direction(1),
		//m_Mode(PFM_LOOP_TO_START),
		m_InvertDirection(false),
		m_RoadVehicle(new LaneVehicle()),
		m_CurrentPos(0,0,0),
		m_AngularVelocity(0,0,0),
		m_VehicleSpeed(0,0,0),
		m_DebugReset(0)
	{

	}

	AIFollowRoadComponent::~AIFollowRoadComponent()
	{

	}

	void AIFollowRoadComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("AIFollowRoadComponent",new Creator<AIFollowRoadComponent, IComponent>);
		RegisterProperty<Float>("WaypointRadius", &AIFollowRoadComponent::GetWaypointRadius, &AIFollowRoadComponent::SetWaypointRadius);
		RegisterProperty<bool>("InvertDirection", &AIFollowRoadComponent::GetInvertDirection, &AIFollowRoadComponent::SetInvertDirection);
	}

	void AIFollowRoadComponent::GoToIntersection(AIRoadIntersectionComponentPtr inter)
	{
		m_CurrentIntersection = inter;
	}

	void AIFollowRoadComponent::SetInvertDirection(bool value)
	{
		m_InvertDirection = value;
		m_Direction = m_Direction * -1;
	}

	bool AIFollowRoadComponent::GetInvertDirection() const
	{
		return m_InvertDirection;
	}

	void AIFollowRoadComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(AIFollowRoadComponent::OnTransMessage,TransformationNotifyMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(AIFollowRoadComponent::OnPhysicsMessage,VelocityNotifyMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(AIFollowRoadComponent::OnSpawnOnRoad,SpawnOnRoadMessage,0));
		SceneManagerListenerPtr listener = shared_from_this();
		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<AISceneManager>()->Register(listener);
	}

	void AIFollowRoadComponent::OnDelete()
	{
		
	}

	void AIFollowRoadComponent::OnTransMessage(TransformationNotifyMessagePtr message)
	{
		m_CurrentPos = message->GetPosition();
	}


	void AIFollowRoadComponent::OnSpawnOnRoad(SpawnOnRoadMessagePtr message)
	{
		/*bool acticve = false;
		if(m_CurrentLane)
		{
			acticve = true;
			m_CurrentLane->UnregisterVehicle(m_CurrentLane);
		}
		
		m_CurrentRoad = GetFreeRoad(message->m_RoadObject->GetFirstComponentByClass<RoadSegmentComponent>(true));
		
		
		m_CurrentIntersection = m_CurrentRoad->GetEndNode()->GetFirstComponentByClass<AIRoadIntersectionComponent>();
		m_NextRoad = m_CurrentIntersection->GetRandomRoad(m_CurrentRoad);
		m_CurrentRoad->RegisterVehicle(m_RoadVehicle,false);
		m_Turn = m_CurrentIntersection->CheckTurn(m_CurrentRoad,m_NextRoad);

		m_RoadVehicle->m_Distance = -10;
		m_RoadVehicle->m_Speed = 0;
		m_RoadVehicle->m_DistanceToPath = 0;

		Vec3 pos;
		Quaternion rot;
		double distance;
		if(m_CurrentRoad && m_CurrentRoad->FirstFreeLocation(false,pos,rot,distance,10))
		{
			pos.y += 1;
			m_RoadVehicle->m_Distance = distance;
			m_RoadVehicle->m_Speed = 0;
			m_RoadVehicle->m_DistanceToPath = 0;
			m_CurrentPos = pos;
			if(acticve)
			{
				//Clear messages
				GetSceneObject()->ClearMessages();
				GetSceneObject()->SendImmediate(MessagePtr(new PositionMessage(pos)));
				GetSceneObject()->SendImmediate(MessagePtr(new RotationMessage(rot)));
				//std::cout << "Spawn Position:" << m_CurrentPos  << "\n";
				m_DebugReset = 1;
				//GetSceneObject()->PostMessage(MessagePtr(new VehicleStateMessage(CS_RUN,1)));
			}
			else
			{
				GetSceneObject()->SendImmediate(MessagePtr(new PositionMessage(pos)));
				GetSceneObject()->SendImmediate(MessagePtr(new RotationMessage(rot)));
			}
		}
		else
			m_CurrentRoad.reset();*/
	}

	/*RoadSegmentComponentPtr AIFollowRoadComponent::GetFreeRoad(RoadSegmentComponentPtr road)
	{
		Vec3 pos;
		Quaternion rot;
		double distance;
		if(road->FirstFreeLocation(false,pos,rot,distance,10))
		{
			return road;
		}
		else
		{
			int end = rand() % 2;
			AIRoadIntersectionComponentPtr inter;
			if(end)
				inter = road->GetEndNode()->GetFirstComponentByClass<AIRoadIntersectionComponent>();
			else
				inter = road->GetStartNode()->GetFirstComponentByClass<AIRoadIntersectionComponent>();

			road = inter->GetRandomRoad(road);
			return GetFreeRoad(road);
		}
	}*/


	void AIFollowRoadComponent::UpdateLaneBuffer()
	{
		//update lane buffer
		while(m_LaneBuffer.size() < 4)
		{
			AIRoadLaneComponentPtr lane = m_LaneBuffer.back();
			std::vector<AIRoadLaneComponentPtr>* lanes = lane->GetNextLanesPtr();
			if(lanes->size() > 0)
			{
				//just pick first for now
				m_LaneBuffer.push_back(lanes->at(0));
			}
			else
			{
				//dead end, what to do?
				//do U-turn?
				return;
			}
		}

		m_CurrentPath.clear();
		
		for(size_t i = 0; i < m_LaneBuffer.size(); i++)
		{
			std::vector<Vec3>* waypoints = m_LaneBuffer[i]->GetWaypointsPtr();
			m_CurrentPath.push_back(waypoints->at(i));
		}

		if(m_CurrentPath.size()  > 0)
		{
			int index = 0;
			m_CurrentDistanceOnPath = Math::GetPathDistance(m_CurrentPos,m_CurrentPath,index,m_CurrentDistanceToPath);

			m_CurrentLane = m_LaneBuffer.front();
			int num_wps = m_CurrentLane->GetWaypointsPtr()->size();
			if(index >= num_wps) //remove first lane from buffer
			{
				m_LaneBuffer.erase(m_LaneBuffer.begin());
			}
			// check if passed last lane
		}
	}

	void AIFollowRoadComponent::SceneManagerTick(double delta)
	{
		/*if(!m_CurrentRoad)
		{
			m_CurrentRoad = GetSceneObject()->GetScene()->GetRootSceneObject()->GetFirstComponentByClass<RoadSegmentComponent>(true);
			if(m_CurrentRoad)
			{
				m_CurrentIntersection = m_CurrentRoad->GetEndNode()->GetFirstComponentByClass<AIRoadIntersectionComponent>();

				m_NextRoad = m_CurrentIntersection->GetRandomRoad(m_CurrentRoad);

				m_CurrentRoad->RegisterVehicle(m_RoadVehicle,m_CurrentRoad->StartInIntersection(m_CurrentIntersection));

				m_Turn = m_CurrentIntersection->CheckTurn(m_CurrentRoad,m_NextRoad);
			}
		}
		else*/
		
		UpdateLaneBuffer();

		if(m_CurrentLane)
		{
			const Float current_speed = -m_VehicleSpeed.z;
			double look_ahead = current_speed;
			Float new_distance = m_CurrentDistanceOnPath + look_ahead;
			int index;
			Vec3 target_point = Math::GetPointOnPath(new_distance, m_CurrentPath, false, index);

			Float desired_speed =5;
			GetSceneObject()->PostMessage(MessagePtr(new DesiredSpeedMessage(desired_speed)));
			
			//if(m_LeftTurn)
				//target_point.y = desired_speed;

			//std::cout << "now_distance" << now_distance << " pos:" << target_point << "\n";
			//Get aim point on path
			//if(Math::GetClosestPointOnPath(m_CurrentPos,wps3,index,point))
			{
				GetSceneObject()->GetChildByID("AIM_POINT")->PostMessage(MessagePtr(new WorldPositionMessage(target_point)));
				GetSceneObject()->PostMessage(MessagePtr(new GotoPositionMessage(target_point)));
				//check if index is in next road!
				
			}
			std::stringstream ss;
			ss  <<  GetSceneObject()->GetName();
			ss  <<  "\nSpeed:" << desired_speed;
			ss  <<  "\nDist along Path:" << m_CurrentDistanceOnPath;
			ss  <<  "\nDist to Path:" << m_CurrentDistanceToPath;
			//GetSceneObject()->PostMessage(MessagePtr(new TextCaptionMessage(ss.str())));
		}
	}


	void AIFollowRoadComponent::OnPhysicsMessage(VelocityNotifyMessagePtr message)
	{
		m_AngularVelocity  = message->GetAngularVelocity();
		m_VehicleSpeed  = message->GetLinearVelocity();
	}

	void AIFollowRoadComponent::OnWaypointListUpdated(WaypointListUpdatedMessagePtr message)
	{
		/*m_Waypoints = message->m_Waypoints;
		if(m_Waypoints.size() > 0)
		{
			m_HasWaypoints = true;
			if(m_CurrentWaypoint == 0) //only select new start waypoint if current is first wp
				m_CurrentWaypoint = GetCloesetWaypoint();
		}
		else
			m_HasWaypoints = false;*/

	}

}
