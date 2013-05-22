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
		m_InvertDirection(false)
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
		SceneManagerListenerPtr listener = shared_from_this();
		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<AISceneManager>()->Register(listener);
		//SetWaypointList(m_WaypointListName);
	}

	void FollowRoadComponent::OnDelete()
	{
		
	}

	void FollowRoadComponent::OnTransMessage(TransformationNotifyMessagePtr message)
	{
		m_CurrentPos = message->GetPosition();
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
			}
		}
		else
		{
			double look_ahead = 5;
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
			Float now_distance = Math::GetPathDistance(m_CurrentPos,wps3,index);

			if(index >= wps1.size()) //get new road!
			{
				m_CurrentRoad = m_NextRoad;
				m_CurrentIntersection = m_CurrentRoad->GetNextIntersection(m_CurrentIntersection);
				m_NextRoad = m_CurrentIntersection->GetRandomRoad(m_CurrentRoad);
			}

			Float new_distance = now_distance + look_ahead;

		
			
			Vec3 target_point = Math::GetPointOnPath(new_distance, wps3, false, index);

				std::cout << "now_distance" << now_distance << " pos:" << target_point << "\n";
			//Get aim point on path
			//if(Math::GetClosestPointOnPath(m_CurrentPos,wps3,index,point))
			{
				GetSceneObject()->GetChildByID("AIM_POINT")->PostMessage(MessagePtr(new WorldPositionMessage(target_point)));
				GetSceneObject()->PostMessage(MessagePtr(new GotoPositionMessage(target_point)));
				//check if index is in next road!
				
			}
			
		}
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
