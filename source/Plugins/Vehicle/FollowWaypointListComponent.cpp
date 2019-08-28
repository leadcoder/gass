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

#include "FollowWaypointListComponent.h"
#include "Sim/Messages/GASSPlatformMessages.h"
#include "Sim/Interface/GASSIMissionSceneManager.h"
#include "Core/Math/GASSPath.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Interface/GASSINavigationComponent.h"
#include "Sim/Interface/GASSIWaypointListComponent.h"

namespace GASS
{
	FollowWaypointListComponent::FollowWaypointListComponent() : m_WaypointRadius( 4),
		m_HasWaypoints(false),
		m_CurrentWaypoint(-1),
		m_Direction(1),
		m_Mode(PFM_LOOP_TO_START),
		m_InvertDirection(false)
	{

	}

	FollowWaypointListComponent::~FollowWaypointListComponent()
	{

	}

	std::vector<SceneObjectPtr> WaypointListEnumeration(BaseReflectionObjectPtr obj)
	{
		GASS_SHARED_PTR<FollowWaypointListComponent> wpl= GASS_DYNAMIC_PTR_CAST<FollowWaypointListComponent>(obj);
		return  wpl->GetWaypointListEnumeration();
	}

	std::vector<SceneObjectPtr>  FollowWaypointListComponent::GetWaypointListEnumeration() const
	{
		std::vector<SceneObjectPtr> ret;
		SceneObjectPtr so = GetSceneObject();
		if(so)
		{
			ComponentContainer::ComponentVector comps;
			so->GetScene()->GetRootSceneObject()->GetComponentsByClass<IWaypointListComponent>(comps);
			for(size_t i = 0 ; i < comps.size();i++)
			{
				if(comps[i]->GetOwner() != so)
				{
					WaypointListComponentPtr wpl = GASS_DYNAMIC_PTR_CAST<IWaypointListComponent>(comps[i]);
					if(wpl)
					{
						SceneObjectPtr wp_so = GASS_DYNAMIC_PTR_CAST<SceneObject>(comps[i]->GetOwner());
						ret.push_back(wp_so);
					}
				}
			}
		}
		return ret;
	}



	std::vector<SceneObjectPtr> NavigationEnumeration(BaseReflectionObjectPtr obj)
	{
		GASS_SHARED_PTR<FollowWaypointListComponent> wpl= GASS_DYNAMIC_PTR_CAST<FollowWaypointListComponent>(obj);
		return  wpl->GetNavigationEnumeration();
	}

	std::vector<SceneObjectPtr>  FollowWaypointListComponent::GetNavigationEnumeration() const
	{
		std::vector<SceneObjectPtr> ret;
		SceneObjectPtr so = GetSceneObject();
		if(so)
		{
			ComponentContainer::ComponentVector comps;
			so->GetScene()->GetRootSceneObject()->GetComponentsByClass<INavigationComponent>(comps);
			for(size_t i = 0 ; i < comps.size();i++)
			{
				if(comps[i]->GetOwner() != so)
				{
					NavigationComponentPtr wpl = GASS_DYNAMIC_PTR_CAST<INavigationComponent>(comps[i]);
					if(wpl)
					{
						SceneObjectPtr nav_so = GASS_DYNAMIC_PTR_CAST<SceneObject>(comps[i]->GetOwner());
						ret.push_back(nav_so);
					}
				}
			}
		}
		return ret;
	}


	void FollowWaypointListComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register<FollowWaypointListComponent>();
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("Component used to let vehicles follow any waypoint list by sending goto messages to autopilot component", OF_VISIBLE)));
		RegisterProperty<SceneObjectRef>("WaypointList", &FollowWaypointListComponent::GetWaypointList, &FollowWaypointListComponent::SetWaypointList,
			SceneObjectEnumerationProxyPropertyMetaDataPtr(new SceneObjectEnumerationProxyPropertyMetaData("Waypoint list that we should follow",PF_VISIBLE,WaypointListEnumeration,false)));
		RegisterMember("NavigationObject", &FollowWaypointListComponent::m_NavigationObject,
			SceneObjectEnumerationProxyPropertyMetaDataPtr(new SceneObjectEnumerationProxyPropertyMetaData("Object that hold navigation component",PF_VISIBLE,NavigationEnumeration,false)));
		RegisterMember("WaypointRadius", &FollowWaypointListComponent::m_WaypointRadius,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Radius that should be used to consider a waypoint reached",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<PathFollowModeBinder>("Mode", &FollowWaypointListComponent::GetMode, &FollowWaypointListComponent::SetMode,
			EnumerationProxyPropertyMetaDataPtr(new EnumerationProxyPropertyMetaData("Follow mode",PF_VISIBLE,&PathFollowModeBinder::GetStringEnumeration)));
		RegisterProperty<bool>("InvertDirection", &FollowWaypointListComponent::GetInvertDirection, &FollowWaypointListComponent::SetInvertDirection,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Invert direction",PF_VISIBLE | PF_EDITABLE)));
	}

	void FollowWaypointListComponent::SetInvertDirection(bool value)
	{
		m_InvertDirection = value;
		if(m_InvertDirection)
			m_Direction = m_Direction * -1;
	}

	bool FollowWaypointListComponent::GetInvertDirection() const
	{
		return m_InvertDirection;
	}

	void FollowWaypointListComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(FollowWaypointListComponent::OnTransMessage,TransformationChangedEvent,0));
		RegisterForPostUpdate<IMissionSceneManager>();
		//update
		SetWaypointList(m_WaypointList);
	}

	void FollowWaypointListComponent::OnDelete()
	{

	}

	void FollowWaypointListComponent::OnTransMessage(TransformationChangedEventPtr message)
	{
		m_CurrentPos = message->GetPosition();
	}

	int FollowWaypointListComponent::GetCloesetWaypoint()
	{
		Vec3 pos = m_CurrentPos;
		pos.y = 0;
		int wp_index = -1;
		double shortest_dist = 0;
		if(m_Waypoints.size() > 0)
		{
			for(size_t i = 0; i < m_Waypoints.size(); i++)
			{
				Vec3 wp_pos = m_Waypoints[i];
				wp_pos.y = 0;
				double dist = (wp_pos - pos).Length();
				if(i == 0 || dist < shortest_dist)
				{
					wp_index = static_cast<int>(i);
					shortest_dist = dist;
				}
			}
		}
		return wp_index;
	}

	void FollowWaypointListComponent::SceneManagerTick(double /*delta*/)
	{
		if(m_HasWaypoints)
		{
			//if(true)
			//{
			int num_waypoints = static_cast<int>( m_Waypoints.size());
			int wp_index;
			//Vec3 point_on_path;
			Float ditance_to_path_dist;
			Float now_distance = Path::GetPathDistance(m_CurrentPos,m_Waypoints,wp_index,ditance_to_path_dist);
			double look_ahead = 10;

			if(look_ahead < 3)
				look_ahead = 3;

			if(look_ahead > 10)
				look_ahead = 10;


			bool cyclic = false;
			///look_ahead *= m_Direction;
			switch(m_Mode.GetValue())
			{
			case PFM_LOOP_TO_START:
				cyclic = true;
				break;
			case PFM_STOP_AT_END:
				if(wp_index == num_waypoints-2)
				{
					Vec3 last_wp = m_Waypoints[num_waypoints-1];
					//Check distance to last wp
					if((last_wp - m_CurrentPos).Length() < m_WaypointRadius)
					{
						GetSceneObject()->PostRequest(DesiredSpeedMessagePtr(new DesiredSpeedMessage(0)));
					}
				}
				break;
			case PFM_REVERSE_LOOP:
				break;
				/*case PFM_REVERSE_LOOP:
				if(num_waypoints > 1)
				{
				if(m_Direction > 0 && wp_index == num_waypoints-1 && ditance_to_path_dist < m_WaypointRadius)
				{
				m_Direction = -1;
				}
				if(m_Direction < 0 && wp_index == 0 && ditance_to_path_dist < m_WaypointRadius)
				{
				m_Direction = 1;
				}
				}
				break;*/
			}
			Float new_distance = now_distance + look_ahead;
			Vec3 target_point = Path::GetPointOnPath(new_distance, m_Waypoints, cyclic, wp_index);
			GetSceneObject()->PostRequest(GotoPositionRequestPtr(new GotoPositionRequest(target_point)));

			/*	}
			else
			{
			Vec3 wp_pos = m_Waypoints[m_CurrentWaypoint];
			wp_pos.y = 0;
			Vec3 pos = m_CurrentPos;
			pos.y = 0;

			int num_waypoints = (int) m_Waypoints.size();

			if((wp_pos - pos).Length() < m_WaypointRadius)
			{
			switch(m_Mode.GetValue())
			{
			case PFM_LOOP_TO_START:
			{
			m_CurrentWaypoint += m_Direction;
			if(m_CurrentWaypoint >= num_waypoints)
			m_CurrentWaypoint = 0;
			if(m_CurrentWaypoint < 0)
			m_CurrentWaypoint = num_waypoints-1;
			}
			break;
			case PFM_STOP_AT_END:
			{

			m_CurrentWaypoint += m_Direction;
			if(m_Direction > 0 && m_CurrentWaypoint >= num_waypoints)
			{
			m_CurrentWaypoint = num_waypoints-1;
			GetSceneObject()->PostRequest(DesiredSpeedMessagePtr(new DesiredSpeedMessage(0)));
			}
			if(m_Direction < 0 && m_CurrentWaypoint < 0)
			{
			m_CurrentWaypoint = 0;
			GetSceneObject()->PostRequest(DesiredSpeedMessagePtr(new DesiredSpeedMessage(0)));
			}

			if(m_CurrentWaypoint >= num_waypoints)
			m_CurrentWaypoint = 0;
			if(m_CurrentWaypoint < 0)
			m_CurrentWaypoint = num_waypoints-1;
			}
			break;
			case PFM_REVERSE_LOOP:
			{
			if(num_waypoints > 1)
			{
			m_CurrentWaypoint += m_Direction;

			if(m_CurrentWaypoint < 0)
			{
			m_Direction = 1;
			m_CurrentWaypoint = 1;
			}
			if(m_CurrentWaypoint >= num_waypoints)
			{
			m_Direction = -1;
			m_CurrentWaypoint = num_waypoints-2;
			}
			}
			}
			break;
			}
			}

			pos = m_Waypoints[m_CurrentWaypoint];
			GetSceneObject()->PostRequest(GotoPositionRequestPtr(new GotoPositionRequest(pos)));
		}*/
		//DriveTo(m_DesiredPos,m_LastPos, m_DesiredSpeed, delta);
	}
}

void FollowWaypointListComponent::OnWaypointListUpdated(WaypointListUpdatedMessagePtr message)
{
	m_Waypoints = message->m_Waypoints;
	if(m_Waypoints.size() > 0)
	{

		if(m_NavigationObject.IsValid() && m_Waypoints.size() > 1)
		{
			std::vector<Vec3> final_path;
			bool path_found = false;
			NavigationComponentPtr nav = m_NavigationObject->GetFirstComponentByClass<INavigationComponent>();
			for(size_t i = 1; i < m_Waypoints.size(); i++)
			{
				if(path_found && i > 1 && final_path.size() > 0)
				{
					final_path.pop_back();
				}
				path_found = nav->GetShortestPath(m_Waypoints[i-1],m_Waypoints[i],final_path);
			}

			m_Waypoints = final_path;
		}

		m_HasWaypoints = true;
		if(m_CurrentWaypoint == 0) //only select new start waypoint if current is first wp
			m_CurrentWaypoint = GetCloesetWaypoint();
	}
	else
		m_HasWaypoints = false;
}

void FollowWaypointListComponent::SetWaypointList(SceneObjectRef waypointlist)
{
	if(GetSceneObject())
	{
		//first unregister from previous waypointlist
		if(m_WaypointList.IsValid())
		{
			m_WaypointList->UnregisterForMessage(UNREG_TMESS(FollowWaypointListComponent::OnWaypointListUpdated,WaypointListUpdatedMessage));
		}
		if(waypointlist.IsValid())
		{
			m_CurrentWaypoint = 0;
			waypointlist->RegisterForMessage(REG_TMESS(FollowWaypointListComponent::OnWaypointListUpdated,WaypointListUpdatedMessage,0));

			//force update
			WaypointListComponentPtr wpl = waypointlist->GetFirstComponentByClass<IWaypointListComponent>();
			waypointlist->PostEvent(WaypointListUpdatedMessagePtr(new WaypointListUpdatedMessage(wpl->GetWaypoints())));
		}
	}
	m_WaypointList = waypointlist;
}


SceneObjectRef FollowWaypointListComponent::GetWaypointList() const
{
	return m_WaypointList;
}

void FollowWaypointListComponent::SetMode(const PathFollowModeBinder &mode)
{
	m_Mode = mode;
}

PathFollowModeBinder FollowWaypointListComponent::GetMode() const
{
	return m_Mode;
}

}
