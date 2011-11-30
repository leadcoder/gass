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
#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Utils/Log.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Systems/Resource/IResourceSystem.h"
#include "Sim/SimEngine.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Scheduling/IRuntimeController.h"
#include "Sim/Systems/Input/ControlSettingsManager.h"
#include "Sim/Systems/Input/ControlSetting.h"


namespace GASS
{
	FollowWaypointListComponent::FollowWaypointListComponent()  : 
		m_WaypointRadius( 4),
		m_HasWaypoints(false),
		m_CurrentWaypoint(-1),
		m_Direction(1),
		m_Mode(PFM_LOOP_TO_START)
	{

	}

	FollowWaypointListComponent::~FollowWaypointListComponent()
	{

	}

	void FollowWaypointListComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("FollowWaypointListComponent",new Creator<FollowWaypointListComponent, IComponent>);
		RegisterProperty<std::string>("WaypointList", &FollowWaypointListComponent::GetWaypointList, &FollowWaypointListComponent::SetWaypointList);
		RegisterProperty<Float>("WaypointRadius", &FollowWaypointListComponent::GetWaypointRadius, &FollowWaypointListComponent::SetWaypointRadius);
		RegisterProperty<std::string>("Mode", &FollowWaypointListComponent::GetMode, &FollowWaypointListComponent::SetMode);
	}

	void FollowWaypointListComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(FollowWaypointListComponent::OnLoad,LoadGameComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(FollowWaypointListComponent::OnUnload,UnloadComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(FollowWaypointListComponent::OnTransMessage,TransformationNotifyMessage,0));
	}

	void FollowWaypointListComponent::OnLoad(LoadGameComponentsMessagePtr message)
	{
		SimEngine::GetPtr()->GetRuntimeController()->Register(this);
		SetWaypointList(m_WaypointListName);
	}

	void FollowWaypointListComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
		SimEngine::GetPtr()->GetRuntimeController()->Unregister(this);
	}



	void FollowWaypointListComponent::OnTransMessage(TransformationNotifyMessagePtr message)
	{
		m_CurrentPos = message->GetPosition();
	}

	void FollowWaypointListComponent::Update(double delta)
	{
		
		if(m_HasWaypoints)
		{
			Vec3 wp_pos = m_Waypoints[m_CurrentWaypoint];
			wp_pos.y = 0;
			Vec3 pos = m_CurrentPos;
			pos.y = 0;
			
			if((wp_pos - pos).Length() < m_WaypointRadius)
			{
				
				if(m_Mode == PFM_LOOP_TO_START)
				{
					m_CurrentWaypoint++;
					m_CurrentWaypoint = m_CurrentWaypoint % m_Waypoints.size();
				}
				else if(m_Mode == PFM_STOP_AT_END)
				{
					if(m_CurrentWaypoint == m_Waypoints.size())
						m_CurrentWaypoint = m_Waypoints.size()-1;
				}
				else if(m_Mode == PFM_REVERSE_LOOP)
				{
					m_CurrentWaypoint += m_Direction;
					
					if(m_Direction > 0 && m_CurrentWaypoint >= m_Waypoints.size())
					{
						m_Direction = -1;
						m_CurrentWaypoint = m_Waypoints.size();
					}
					else if(m_Direction < 0 && m_CurrentWaypoint < 0)
					{
						m_Direction = 1;
						m_CurrentWaypoint = 0;
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
			m_HasWaypoints = true;
		else
			m_HasWaypoints = false;
	}

	void FollowWaypointListComponent::SetWaypointList(const std::string &waypointlist)
	{
		m_WaypointListName = waypointlist; 
		if(GetSceneObject())
		{
			SceneObjectPtr obj = GetSceneObject()->GetObjectUnderRoot()->GetParentSceneObject()->GetFirstChildByName(waypointlist,false);
			if(obj) 
			{
				m_CurrentWaypoint = 0;
				
				obj->RegisterForMessage(REG_TMESS(FollowWaypointListComponent::OnWaypointListUpdated,WaypointListUpdatedMessage,0));
			}
		}
	}
	std::string FollowWaypointListComponent::GetWaypointList() const
	{
		return m_WaypointListName;
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
	

	TaskGroup FollowWaypointListComponent::GetTaskGroup() const
	{
		return "VEHICLE_TASK_GROUP";
	}
}
