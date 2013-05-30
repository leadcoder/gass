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

#include "FollowRoadComponent.h"
#include "Plugins/AI/AISceneManager.h"
#include "Plugins/AI/RoadSegmentComponent.h"
#include "Plugins/AI/RoadIntersectionComponent.h"

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
	FollowRoadComponent::FollowRoadComponent()  : 
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

	FollowRoadComponent::~FollowRoadComponent()
	{

	}

	void FollowRoadComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("FollowRoadComponent",new Creator<FollowRoadComponent, IComponent>);
		//RegisterProperty<std::string>("WaypointList", &FollowRoadComponent::GetWaypointList, &FollowRoadComponent::SetWaypointList);
		RegisterProperty<Float>("WaypointRadius", &FollowRoadComponent::GetWaypointRadius, &FollowRoadComponent::SetWaypointRadius);
//		RegisterProperty<std::string>("Mode", &FollowRoadComponent::GetMode, &FollowRoadComponent::SetMode);
		RegisterProperty<bool>("InvertDirection", &FollowRoadComponent::GetInvertDirection, &FollowRoadComponent::SetInvertDirection);
	}

	void FollowRoadComponent::GoToIntersection(RoadIntersectionComponentPtr inter)
	{
		m_CurrentIntersection = inter;
		//inter->GetRandomIntersection();
	}

	void FollowRoadComponent::SetInvertDirection(bool value)
	{
		m_InvertDirection = value;
		m_Direction = m_Direction * -1;
	}

	bool FollowRoadComponent::GetInvertDirection() const
	{
		return m_InvertDirection;
	}

	void FollowRoadComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(FollowRoadComponent::OnTransMessage,TransformationNotifyMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(FollowRoadComponent::OnPhysicsMessage,VelocityNotifyMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(FollowRoadComponent::OnSpawnOnRoad,SpawnOnRoadMessage,0));
		SceneManagerListenerPtr listener = shared_from_this();
		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<AISceneManager>()->Register(listener);
		//std::cout << "Init:" << GetSceneObject()->GetName() <<  "\n";
		//SetWaypointList(m_WaypointListName);
	}

	void FollowRoadComponent::OnDelete()
	{
		
	}

	void FollowRoadComponent::OnTransMessage(TransformationNotifyMessagePtr message)
	{
		m_CurrentPos = message->GetPosition();

		/*if(m_DebugReset)
		{
			std::cout << "New Position:" << m_CurrentPos  << "\n";
			m_DebugReset++;
			if(m_DebugReset > 10)
				m_DebugReset = 0;
		}*/
	}


	void FollowRoadComponent::OnSpawnOnRoad(SpawnOnRoadMessagePtr message)
	{
		bool acticve = false;
		if(m_CurrentRoad)
		{
			acticve = true;
			m_CurrentRoad->UnregisterVehicle(m_RoadVehicle,m_CurrentRoad->StartInIntersection(m_CurrentIntersection));
		}


		
		m_CurrentRoad = GetFreeRoad(message->m_RoadObject->GetFirstComponentByClass<RoadSegmentComponent>(true));
		
		
		m_CurrentIntersection = m_CurrentRoad->GetEndNode()->GetFirstComponentByClass<RoadIntersectionComponent>();
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
			m_CurrentRoad.reset();
	}

	RoadSegmentComponentPtr FollowRoadComponent::GetFreeRoad(RoadSegmentComponentPtr road)
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
			RoadIntersectionComponentPtr inter;
			if(end)
				inter = road->GetEndNode()->GetFirstComponentByClass<RoadIntersectionComponent>();
			else
				inter = road->GetStartNode()->GetFirstComponentByClass<RoadIntersectionComponent>();

			road = inter->GetRandomRoad(road);
			return GetFreeRoad(road);
		}
	}

	void FollowRoadComponent::SceneManagerTick(double delta)
	{
		if(!m_CurrentRoad)
		{
			//Initialize:)
			
			m_CurrentRoad = GetSceneObject()->GetScene()->GetRootSceneObject()->GetFirstComponentByClass<RoadSegmentComponent>(true);
			if(m_CurrentRoad)
			{
				m_CurrentIntersection = m_CurrentRoad->GetEndNode()->GetFirstComponentByClass<RoadIntersectionComponent>();

				m_NextRoad = m_CurrentIntersection->GetRandomRoad(m_CurrentRoad);

				m_CurrentRoad->RegisterVehicle(m_RoadVehicle,m_CurrentRoad->StartInIntersection(m_CurrentIntersection));

				m_Turn = m_CurrentIntersection->CheckTurn(m_CurrentRoad,m_NextRoad);
			}
		}
		else
		{
			const Float current_speed = -m_VehicleSpeed.z;
			double look_ahead = current_speed;
			
			//clamp
			if(look_ahead < 4)
				look_ahead = 4;
			if(look_ahead > 20)
				look_ahead = 20;

			//get target position on current path
			//merge waypoint lists
			std::vector<Vec3> wps1 = m_CurrentRoad->GetLane(0,m_CurrentRoad->StartInIntersection(m_CurrentIntersection));
			std::vector<Vec3> wps2 = m_NextRoad->GetLane(0,!m_NextRoad->StartInIntersection(m_CurrentIntersection));
			std::vector<Vec3> wps3 = wps1;
			
			for(size_t i = 0; i < wps2.size(); i++)
			{
				wps3.push_back(wps2[i]);
			}

			int index = 0;
			Float path_dist;
			Float now_distance = Math::GetPathDistance(m_CurrentPos,wps3,index,path_dist);

			
			const Float friction = 0.6;
			Float stop_distance = current_speed*current_speed/(2 * friction *9.81);
			//add some safty margin
			stop_distance *= 2;

			if(stop_distance < 10)
				stop_distance = 10;

			double d = 0;
			for(size_t i =  1 ;  i < wps1.size(); i++)
				{
				d += (wps1[i] - wps1[i-1]).FastLength();
			}

			Float intersection_distance = d - now_distance;


			Float desired_speed = 14;
			//Check light

			
			TrafficLight light;
			if(m_CurrentIntersection->GetTrafficLight(m_CurrentRoad,light))
			{
				if(light.m_Stop) // check distance to light
				{
					if(intersection_distance < stop_distance &&  intersection_distance > -1)
					{
						//GetSceneObject()->PostMessage(MessagePtr(new DesiredSpeedMessage(0)));
						
						//slam on brakes!
						desired_speed  = 0;
						//look_ahead = light_distance;
						//if(look_ahead < 1)
						//	look_ahead  = 0;
					}
					else
					{
						
						//GetSceneObject()->PostMessage(MessagePtr(new DesiredSpeedMessage(desired_speed)));
					}
				}
				/*else if(m_Turn == TURN_LEFT)
				{
					for(size_t i = 0; i < light.m_Roads.size(); i++)
					{
						if(light.m_Roads[i] != m_CurrentRoad)
						{
							bool up_stream = light.m_Roads[i]->StartInIntersection(m_CurrentIntersection);
							if(!light.m_Roads[i]->IsRoadFree(up_stream, 40))
							{
								desired_speed  = 0;
							}
						}
					}
				}*/
			}
			else
			{
				//GetSceneObject()->PostMessage(MessagePtr(new DesiredSpeedMessage(desired_speed)));
			}

			//Check that next road is free!
			if(intersection_distance < stop_distance && !m_NextRoad->IsStartOfRoadFree(!m_NextRoad->StartInIntersection(m_CurrentIntersection), 2))
			{
				desired_speed  = 0;
			}

			
			m_RoadVehicle->m_Distance = now_distance;
			m_RoadVehicle->m_Speed = current_speed;
			m_RoadVehicle->m_DistanceToPath = path_dist;

			//Check lane
			LaneVehicle* closest = m_CurrentRoad->GetClosest(m_CurrentRoad->StartInIntersection(m_CurrentIntersection), m_RoadVehicle);
			
			if(closest && desired_speed > 0 && closest->m_DistanceToPath < 2)
			{
				const Float speed_diff = current_speed - closest->m_Speed;
				if(speed_diff > 0)
				{
					const Float distance_to_next = closest->m_Distance - now_distance;
					
					Float stop_distance_to_next = speed_diff*speed_diff/(2 * friction *9.81);
					//add some safty margin
					stop_distance_to_next *= 2;

					if(stop_distance_to_next < 15)
						stop_distance_to_next = 15;

					if(distance_to_next < stop_distance_to_next)
					{
						desired_speed = 0;
					}
				}
			}


			
			
			

			if(index >= wps1.size()) //get new road!
			{
				m_CurrentRoad->UnregisterVehicle(m_RoadVehicle,m_CurrentRoad->StartInIntersection(m_CurrentIntersection));
				m_CurrentRoad = m_NextRoad;
				m_CurrentIntersection = m_CurrentRoad->GetNextIntersection(m_CurrentIntersection);
				m_NextRoad = m_CurrentIntersection->GetRandomRoad(m_CurrentRoad);
				m_CurrentRoad->RegisterVehicle(m_RoadVehicle,m_CurrentRoad->StartInIntersection(m_CurrentIntersection));
				//Check if left turn!
				m_Turn = m_CurrentIntersection->CheckTurn(m_CurrentRoad,m_NextRoad);

				//std::cout << "Change Position:" << m_CurrentPos << "\n";
			}
		
			Float new_distance = now_distance + look_ahead;
			Vec3 target_point = Math::GetPointOnPath(new_distance, wps3, false, index);

			//Check if inside intersection
			if(index == wps1.size()-1)
			{
				if(desired_speed > 0)
				{
					if(m_Turn == TURN_RIGHT)
						desired_speed = 5;
					else if(m_Turn == TURN_LEFT)
						desired_speed = 10;
				}
			}
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
			ss  <<  "\nDist along Path:" << now_distance;
			ss  <<  "\nDist to Path:" << path_dist;
			//GetSceneObject()->PostMessage(MessagePtr(new TextCaptionMessage(ss.str())));
		}
	}


	void FollowRoadComponent::OnPhysicsMessage(VelocityNotifyMessagePtr message)
	{
		m_AngularVelocity  = message->GetAngularVelocity();
		m_VehicleSpeed  = message->GetLinearVelocity();
	}

	void FollowRoadComponent::OnWaypointListUpdated(WaypointListUpdatedMessagePtr message)
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
