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

#include "FollowWaypointListComponent.h"
#include "GameMessages.h"
#include "Plugins/Game/GameSceneManager.h"

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
#include "Sim/Interface/GASSIWaypointListComponent.h"


namespace GASS
{
	FollowWaypointListComponent::FollowWaypointListComponent()  : 
		m_WaypointRadius( 4),
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
		SPTR<FollowWaypointListComponent> wpl= DYNAMIC_PTR_CAST<FollowWaypointListComponent>(obj);
		return  wpl->GetWaypointListEnumeration();
	}

	std::vector<SceneObjectPtr>  FollowWaypointListComponent::GetWaypointListEnumeration() const
	{
		std::vector<SceneObjectPtr> ret;
		SceneObjectPtr so = GetSceneObject();
		if(so)
		{
			IComponentContainer::ComponentVector comps;
			so->GetScene()->GetRootSceneObject()->GetComponentsByClass<IWaypointListComponent>(comps);
			for(int i = 0 ; i < comps.size();i++)
			{
				if(comps[i]->GetOwner() != so)
				{
					WaypointListComponentPtr wpl = DYNAMIC_PTR_CAST<IWaypointListComponent>(comps[i]);
					if(wpl)
					{
						SceneObjectPtr so = DYNAMIC_PTR_CAST<SceneObject>(comps[i]->GetOwner());
						ret.push_back(so);
					}
				}
			}
		}
		return ret;
	}


	void FollowWaypointListComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("FollowWaypointListComponent",new Creator<FollowWaypointListComponent, IComponent>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("Component used to let vehicles follow any waypoint list by sending goto messages to autopilot component", OF_VISIBLE)));
		RegisterProperty<SceneObjectRef>("WaypointList", &FollowWaypointListComponent::GetWaypointList, &FollowWaypointListComponent::SetWaypointList,
			//BasePropertyMetaDataPtr(new BasePropertyMetaData("Waypoint list that we should follow",PF_VISIBLE | PF_EDITABLE)));
			SceneObjectEnumerationProxyPropertyMetaDataPtr(new SceneObjectEnumerationProxyPropertyMetaData("Waypoint list that we should follow",PF_VISIBLE,WaypointListEnumeration)));
		RegisterProperty<Float>("WaypointRadius", &FollowWaypointListComponent::GetWaypointRadius, &FollowWaypointListComponent::SetWaypointRadius,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Radius that should be used to consider a waypoint reached",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<std::string>("Mode", &FollowWaypointListComponent::GetMode, &FollowWaypointListComponent::SetMode,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Follow mode",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<bool>("InvertDirection", &FollowWaypointListComponent::GetInvertDirection, &FollowWaypointListComponent::SetInvertDirection,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Invert direction",PF_VISIBLE | PF_EDITABLE)));
	}

	void FollowWaypointListComponent::SetInvertDirection(bool value)
	{
		m_InvertDirection = value;
		m_Direction = m_Direction * -1;
	}

	bool FollowWaypointListComponent::GetInvertDirection() const
	{
		return m_InvertDirection;
	}

	void FollowWaypointListComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(FollowWaypointListComponent::OnTransMessage,TransformationNotifyMessage,0));
		SceneManagerListenerPtr listener = shared_from_this();
		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<GameSceneManager>()->Register(listener);
		//update
		SetWaypointList(m_WaypointList);
	}

	void FollowWaypointListComponent::OnDelete()
	{
		
	}

	void FollowWaypointListComponent::OnTransMessage(TransformationNotifyMessagePtr message)
	{
		m_CurrentPos = message->GetPosition();
	}

	int FollowWaypointListComponent::GetCloesetWaypoint()
	{
		Vec3 pos = m_CurrentPos;
		pos.y = 0;
		int wp_index = -1;
		double shortest_dist = 100000000000000000;
		if(m_Waypoints.size() > 0)
		{
			for(int i = 0; i < m_Waypoints.size(); i++)
			{
				Vec3 wp_pos = m_Waypoints[i];
				wp_pos.y = 0;
				double dist = (wp_pos - pos).FastLength();
				if(i == 0 || dist < shortest_dist)
				{
					wp_index = i;
					shortest_dist = dist;
				}
			}
		}
		return wp_index;
	}

	void FollowWaypointListComponent::SceneManagerTick(double delta)
	{
		
		if(m_HasWaypoints)
		{
			Vec3 wp_pos = m_Waypoints[m_CurrentWaypoint];
			wp_pos.y = 0;
			Vec3 pos = m_CurrentPos;
			pos.y = 0;

			int num_waypoints = (int) m_Waypoints.size();
			
			if((wp_pos - pos).Length() < m_WaypointRadius)
			{
				
				if(m_Mode == PFM_LOOP_TO_START)
				{
					m_CurrentWaypoint += m_Direction;
					if(m_CurrentWaypoint >= num_waypoints)
						m_CurrentWaypoint = 0;
					if(m_CurrentWaypoint < 0)
						m_CurrentWaypoint = num_waypoints-1;
				}

				else if(m_Mode == PFM_STOP_AT_END)
				{
					
					m_CurrentWaypoint += m_Direction;

					
					if(m_Direction > 0 && m_CurrentWaypoint >= num_waypoints)
					{
						m_CurrentWaypoint = num_waypoints-1;
						GetSceneObject()->PostMessage(MessagePtr(new DesiredSpeedMessage(0)));
						
					}
					if(m_Direction < 0 && m_CurrentWaypoint < 0)
					{
						m_CurrentWaypoint = 0;
						GetSceneObject()->PostMessage(MessagePtr(new DesiredSpeedMessage(0)));
					
					}


					if(m_CurrentWaypoint >= num_waypoints)
						m_CurrentWaypoint = 0;
					if(m_CurrentWaypoint < 0)
						m_CurrentWaypoint = num_waypoints-1;

					
				}

				else if(m_Mode == PFM_REVERSE_LOOP)
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
			}

			pos = m_Waypoints[m_CurrentWaypoint];
			
			GetSceneObject()->PostMessage(MessagePtr(new GotoPositionMessage(pos)));
			//DriveTo(m_DesiredPos,m_LastPos, m_DesiredSpeed, delta);
		}
		
	}

	void FollowWaypointListComponent::OnWaypointListUpdated(WaypointListUpdatedMessagePtr message)
	{
		m_Waypoints = message->m_Waypoints;
		if(m_Waypoints.size() > 0)
		{
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
				waypointlist->PostMessage(MessagePtr(new UpdateWaypointListMessage()));
			}
		}
		m_WaypointList = waypointlist; 
	}


	SceneObjectRef FollowWaypointListComponent::GetWaypointList() const
	{
		return m_WaypointList;
	}

	void FollowWaypointListComponent::SetMode(const std::string &mode)
	{
		if(mode == "PFM_STOP_AT_END")
			m_Mode = PFM_STOP_AT_END;
		else if(mode == "PFM_LOOP_TO_START")
			m_Mode = PFM_LOOP_TO_START;
		else if(mode == "PFM_REVERSE_LOOP")
			m_Mode = PFM_REVERSE_LOOP;
	}

	std::string FollowWaypointListComponent::GetMode() const
	{
		switch(m_Mode)
		{
		case PFM_STOP_AT_END:
			return "PFM_STOP_AT_END";
		case PFM_LOOP_TO_START:
			return "PFM_LOOP_TO_START";
		case PFM_REVERSE_LOOP:
			return "PFM_REVERSE_LOOP";
		}
		return "Error"; 
	}

}
