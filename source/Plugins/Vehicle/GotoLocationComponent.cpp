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

#include "GotoLocationComponent.h"

#include <memory>
#include "Core/Math/GASSMath.h"
#include "Core/Math/GASSPath.h"
#include "Sim/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSPlatformMessages.h"
#include "Sim/Interface/GASSIMissionSceneManager.h"
#include "Sim/Interface/GASSINavigationComponent.h"

namespace GASS
{
	GoToLocationComponent::GoToLocationComponent()
	{

	}

	GoToLocationComponent::~GoToLocationComponent()
	{

	}

	

	void GoToLocationComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register<GoToLocationComponent>();
		GetClassRTTI()->SetMetaData(std::make_shared<ClassMetaData>("Component used to let vehicles follow any waypoint list by sending goto messages to autopilot component", OF_VISIBLE));
		auto nav_prop = RegisterMember("NavigationObject", &GoToLocationComponent::m_NavigationObject, PF_VISIBLE, "Object that hold navigation component");
		nav_prop->SetObjectOptionsFunction(&GoToLocationComponent::GetNavigationEnumeration);
	}

	std::vector<SceneObjectRef>  GoToLocationComponent::GetNavigationEnumeration() const
	{
		std::vector<SceneObjectRef> ret;
		SceneObjectPtr so = GetSceneObject();
		if (so)
		{
			SceneObject::ComponentVector comps;
			so->GetScene()->GetRootSceneObject()->GetComponentsByClass<INavigationComponent>(comps);
			for (size_t i = 0; i < comps.size(); i++)
			{
				if (comps[i]->GetOwner() != so)
				{
					NavigationComponentPtr wpl = GASS_DYNAMIC_PTR_CAST<INavigationComponent>(comps[i]);
					if (wpl)
					{
						SceneObjectPtr new_so = comps[i]->GetOwner();
						ret.emplace_back(new_so);
					}
				}
			}
		}
		return ret;
	}

	void GoToLocationComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(GoToLocationComponent::OnTransformationMessage,TransformationChangedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(GoToLocationComponent::OnPathfindToLocation,PathfindToPositionMessage,0));
		RegisterForPostUpdate<IMissionSceneManager>();
	}

	void GoToLocationComponent::OnTransformationMessage(TransformationChangedEventPtr message)
	{
		m_CurrentLocation = message->GetPosition();
	}

	void GoToLocationComponent::OnPathfindToLocation(PathfindToPositionMessagePtr message)
	{
		m_DestinationLocation = message->GetPosition();

		if(m_NavigationObject.IsValid())
		{
			//std::vector<Vec3> final_path;
			NavigationComponentPtr nav = m_NavigationObject->GetFirstComponentByClass<INavigationComponent>();
			m_Path.clear();
			/*bool path_found = */nav->GetShortestPath(m_CurrentLocation,m_DestinationLocation,m_Path);
		}
	}

	void GoToLocationComponent::SceneManagerTick(double /*delta*/)
	{
		if(m_Path.size() > 1)
		{
			//int num_waypoints = (int) m_Path.size();
			int wp_index;
			//Vec3 point_on_path;
			Float ditance_to_path_dist;
			Float now_distance = Path::GetPathDistance(m_CurrentLocation,m_Path,wp_index,ditance_to_path_dist);
			double look_ahead = 10;
			if(look_ahead < 3)
				look_ahead = 3;
			if(look_ahead > 10)
				look_ahead = 10;
			bool cyclic = false;
			Float new_distance = now_distance + look_ahead;
			Vec3 target_point = Path::GetPointOnPath(new_distance, m_Path, cyclic, wp_index);
			GetSceneObject()->PostRequest(std::make_shared<GotoPositionRequest>(target_point));
		}
	}
}
