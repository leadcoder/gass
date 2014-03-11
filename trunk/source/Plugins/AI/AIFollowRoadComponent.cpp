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
	AIFollowRoadComponent::AIFollowRoadComponent()  : m_WaypointRadius( 4),
		m_HasWaypoints(false),
		m_CurrentWaypoint(-1),
		m_Direction(1),
		//m_Mode(PFM_LOOP_TO_START),
		m_InvertDirection(false),
		m_CurrentPos(0,0,0),
		m_AngularVelocity(0,0,0),
		m_VehicleSpeed(0,0,0),
		m_DebugReset(0),
		m_LaneObject(new LaneObject()),
		m_TargetSpeed(0),
		m_LaneBufferSize(4),
		m_RandomSpeed(10,20),
		m_CurrentDistanceOnPath(0),
		m_Debug(false)
	{

	}

	AIFollowRoadComponent::~AIFollowRoadComponent()
	{
		if(m_CurrentLane)
		{
			m_CurrentLane->UnregisterLaneObject(m_LaneObject);
		}
		delete m_LaneObject;
	}

	void AIFollowRoadComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("AIFollowRoadComponent",new Creator<AIFollowRoadComponent, IComponent>);
		RegisterProperty<int>("LaneBufferSize", &AIFollowRoadComponent::GetLaneBufferSize, &AIFollowRoadComponent::SetLaneBufferSize);
		RegisterProperty<Vec2>("RandomSpeed", &AIFollowRoadComponent::GetRandomSpeed, &AIFollowRoadComponent::SetRandomSpeed);
		RegisterProperty<bool>("Debug", &AIFollowRoadComponent::GetDebug, &AIFollowRoadComponent::SetDebug);
		
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
		GetSceneObject()->RegisterForMessage(REG_TMESS(AIFollowRoadComponent::OnTransMessage,TransformationChangedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(AIFollowRoadComponent::OnPhysicsMessage,VelocityNotifyMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(AIFollowRoadComponent::OnSpawnOnRoad,SpawnOnRoadMessage,0));
		SceneManagerListenerPtr listener = shared_from_this();
		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<AISceneManager>()->Register(listener);

		Float speed_span = m_RandomSpeed.y - m_RandomSpeed.x;
		Float norm_rand = (Float)rand()/(Float)RAND_MAX;
		m_TargetSpeed = m_RandomSpeed.x + norm_rand * speed_span;
	}

	void AIFollowRoadComponent::OnDelete()
	{

	}

	void AIFollowRoadComponent::OnTransMessage(TransformationChangedEventPtr message)
	{
		m_CurrentPos = message->GetPosition();
	}


	void AIFollowRoadComponent::OnSpawnOnRoad(SpawnOnRoadMessagePtr message)
	{
		bool acticve = false;
		if(m_CurrentLane)
		{
			acticve = true;
			m_CurrentLane->UnregisterLaneObject(m_LaneObject);
		}

		AIRoadComponentPtr road = message->m_RoadObject->GetFirstComponentByClass<AIRoadComponent>(true);
		m_LaneBuffer.clear();
		m_CurrentLane = AIFollowRoadComponent::GetFreeLane(road->GetStartLanes().back());
		m_CurrentLane->RegisterLaneObject(m_LaneObject);
		m_LaneBuffer.push_back(m_CurrentLane);

		m_LaneObject->m_Distance = -10;
		m_LaneObject->m_Speed = 0;
		m_LaneObject->m_DistanceToPath = 0;

		Vec3 pos;
		Quaternion rot;
		double distance;
		if(m_CurrentLane && m_CurrentLane->FirstFreeLocation(pos,rot,distance,10))
		{
			pos.y += 1;
			m_LaneObject->m_Distance = distance;
			m_LaneObject->m_Speed = 0;
			m_LaneObject->m_DistanceToPath = 0;
			m_CurrentPos = pos;
			if(acticve)
			{
				//Clear messages
				GetSceneObject()->ClearMessages();
				GetSceneObject()->SendImmediate(MessagePtr(new PositionRequest(pos)));
				GetSceneObject()->SendImmediate(MessagePtr(new RotationRequest(rot)));
			}
			else
			{
				GetSceneObject()->SendImmediate(MessagePtr(new PositionRequest(pos)));
				GetSceneObject()->SendImmediate(MessagePtr(new RotationRequest(rot)));
			}
		}
		//else
		//m_CurrentLane.reset();
	}

	AIRoadLaneComponentPtr AIFollowRoadComponent::GetFreeLane(AIRoadLaneComponentPtr lane)
	{
		Vec3 pos;
		Quaternion rot;
		double distance;
		if(lane->FirstFreeLocation(pos,rot,distance,10))
		{
			return lane;
		}
		else
		{
			std::vector<AIRoadLaneComponentPtr>* lanes = lane->GetNextLanesPtr();
			if(lanes->size() > 0)
			{
				int next = rand() % lanes->size();
				return GetFreeLane(lanes->at(next));
			}
			else 
				return lane;
		}
	}


	void AIFollowRoadComponent::UpdateLaneBuffer()
	{
		if(m_LaneBuffer.size()   == 0)
			return;
		//update lane buffer
		while(m_LaneBuffer.size() < m_LaneBufferSize)
		{
			AIRoadLaneComponentPtr lane = m_LaneBuffer.back();
			std::vector<AIRoadLaneComponentPtr>* lanes = lane->GetNextLanesPtr();
			if(lanes->size() > 0)
			{
				int random_lane = rand()%lanes->size();
				//just pick first for now
				m_LaneBuffer.push_back(lanes->at(random_lane ));
			}
			else
			{

				//dead end, do U-turn
				AIRoadComponentPtr road = lane->GetSceneObject()->GetFirstParentComponentByClass<AIRoadComponent>();
				std::vector<AIRoadLaneComponentPtr> turn_lanes;

				if(lane->GetDirection() == LD_DOWNSTREAM)
				{
					turn_lanes = road->GetEndLanes(LD_UPSTREAM);
				}
				else if(lane->GetDirection() == LD_UPSTREAM)
				{
					turn_lanes = road->GetStartLanes(LD_DOWNSTREAM);
				}

				if(turn_lanes.size() > 0)
				{
					m_LaneBuffer.push_back(turn_lanes[0]);
				}
				else
				{
					//no lane back? what to do, reverse?
					return;
				}
			}
		}

		m_CurrentPath.clear();

		for(size_t i = 0; i < m_LaneBuffer.size(); i++)
		{
			std::vector<Vec3>* waypoints = m_LaneBuffer[i]->GetWaypointsPtr();
			for(size_t j = 0; j < waypoints->size(); j++)
			{
				m_CurrentPath.push_back(waypoints->at(j));
			}
		}

		if(m_CurrentPath.size()  > 0)
		{
			int index = 0;
			m_CurrentDistanceOnPath = Math::GetPathDistance(m_CurrentPos,m_CurrentPath,index,m_CurrentDistanceToPath);
			m_CurrentLane = m_LaneBuffer.front();
			size_t num_wps = m_CurrentLane->GetWaypointsPtr()->size();
			if(index >= num_wps) //remove first lane from buffer
			{
				m_CurrentLane->UnregisterLaneObject(m_LaneObject);
				m_LaneBuffer.erase(m_LaneBuffer.begin());
				m_LaneBuffer.front()->RegisterLaneObject(m_LaneObject);
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

			m_LaneObject->m_Distance = m_CurrentDistanceOnPath;
			m_LaneObject->m_Speed = current_speed;
			m_LaneObject->m_DistanceToPath = m_CurrentDistanceToPath;




			double look_ahead = current_speed;

			if(look_ahead < 3)
				look_ahead = 3;

			if(look_ahead > 10)
				look_ahead = 10;
			Float new_distance = m_CurrentDistanceOnPath + look_ahead;
			int index;
			Vec3 target_point = Math::GetPointOnPath(new_distance, m_CurrentPath, false, index);


			Float desired_speed = m_TargetSpeed;
			const Float friction = 0.6;
			//Check lane for objects
			LaneObject* closest = m_CurrentLane->GetClosest(m_LaneObject);
			if(closest && desired_speed > 0 && closest->m_DistanceToPath < 6)
			{
				const Float speed_diff = current_speed - closest->m_Speed;
				if(speed_diff > 0)
				{
					const Float distance_to_next = closest->m_Distance - m_CurrentDistanceOnPath;
					Float stop_distance_to_next = speed_diff*speed_diff/(2 * friction *9.81);
					//add some safty margin
					stop_distance_to_next *= 3;

					if(stop_distance_to_next < 15)
						stop_distance_to_next = 15;

					if(distance_to_next < stop_distance_to_next)
					{
						desired_speed = 0;
					}
				}
			}

			GetSceneObject()->PostMessage(MessagePtr(new DesiredSpeedMessage(desired_speed)));
			
			GetSceneObject()->PostMessage(MessagePtr(new GotoPositionMessage(target_point)));
			
			

			if(m_Debug)
			{
				if(GetSceneObject()->GetChildByID("AIM_POINT"))
					GetSceneObject()->GetChildByID("AIM_POINT")->PostMessage(MessagePtr(new WorldPositionRequest(target_point)));

				GraphicsMeshPtr mesh_data(new GraphicsMesh());
				GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
				mesh_data->SubMeshVector.push_back(sub_mesh_data);
				sub_mesh_data->Type = LINE_LIST;
				sub_mesh_data->MaterialName = "WhiteTransparentNoLighting";

				ColorRGBA color(1,0.2,0,1);
			
				for(size_t i =  1; i < m_CurrentPath.size(); i++)
				{
					Vec3 pos =m_CurrentPath[i-1];
					pos.y += 1;
					sub_mesh_data->PositionVector.push_back(pos);
					sub_mesh_data->ColorVector.push_back(color);
					pos = m_CurrentPath[i];
					pos.y += 1;
					sub_mesh_data->PositionVector.push_back(pos);
					sub_mesh_data->ColorVector.push_back(color);
				}
				MessagePtr mesh_message(new ManualMeshDataRequest(mesh_data));
				GetSceneObject()->GetChildByID("CPATH")->PostMessage(mesh_message);

				//if(m_Debug)
				{
					std::stringstream ss;
					ss  <<  GetSceneObject()->GetName();
					if(closest)
						ss  <<  "\nclosest:" << closest->m_Distance - m_CurrentDistanceOnPath;
					ss  <<  "\nDist along Path:" << m_CurrentDistanceOnPath;
					ss  <<  "\nSpeed:" << desired_speed;

					ss  <<  "\nDist to Path:" << m_CurrentDistanceToPath;
					GetSceneObject()->PostMessage(MessagePtr(new TextCaptionRequest(ss.str())));
				}
			}
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
