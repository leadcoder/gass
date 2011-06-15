/****************************************************************************
*                                                                           *
* GASS                                                                *
* Copyright (C)2003 - 2005 Johan Hedstrom                                   *
* Email: GASS@gmail.com                                               *
* Web page: http://n00b.dyndns.org/GASS                               *
*                                                                           *
* GASS is only used with knowledge from the author. This software     *
* is not allowed to redistribute without permission from the author.        *
* For further license information, please turn to the product home page or  *
* contact author. Abuse against the GASS license is prohibited by law.*
*                                                                           *
*****************************************************************************/ 

#include <boost/bind.hpp>
#include "RoadComponent.h"
#include "Core/Utils/Log.h"
#include "Core/Math/Quaternion.h"
#include "Core/Math/SplineAnimation.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/IComponent.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/IComponent.h"
#include "Core/ComponentSystem/BaseComponentContainerTemplateManager.h"
#include "Core/ComponentSystem/ComponentContainerFactory.h"
#include "Sim/Scheduling/IRuntimeController.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Components/Graphics/ILocationComponent.h"
#include "Sim/SimEngine.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Components/Graphics/MeshData.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Systems/Collision/ICollisionSystem.h"
#include "WaypointComponent.h"
#include "WaypointListComponent.h"
#include "Sim/Components/Graphics/Geometry/ITerrainComponent.h"

namespace GASS
{
	RoadComponent::RoadComponent() : m_Initialized(false), m_FlatFade(10),m_FlatWidth(30)
	{

	}

	RoadComponent::~RoadComponent()
	{

	}

	void RoadComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("RoadComponent",new Creator<RoadComponent, IComponent>);
		RegisterProperty<bool>("FlatTerrain", &GASS::RoadComponent::GetFlatTerrain, &GASS::RoadComponent::SetFlatTerrain);
		RegisterProperty<float>("FlatWidth", &GASS::RoadComponent::GetFlatWidth, &GASS::RoadComponent::SetFlatWidth);
		RegisterProperty<float>("FlatFade", &GASS::RoadComponent::GetFlatFade, &GASS::RoadComponent::SetFlatFade);
	}

	void RoadComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(RoadComponent::OnLoad,LoadCoreComponentsMessage,2));
		GetSceneObject()->RegisterForMessage(REG_TMESS(RoadComponent::OnUnload,UnloadComponentsMessage,2));
	}


	void RoadComponent::OnUnload(UnloadComponentsMessagePtr message)
	{

	}

	void RoadComponent::OnLoad(LoadCoreComponentsMessagePtr message)
	{
		m_Initialized = true;
		//get waypoint list
		WaypointListComponentPtr wpl = GetSceneObject()->GetFirstComponentByClass<WaypointListComponent>();
		if(!wpl)
			Log::Warning("RoadComponent depends on WaypointListComponent");
	}

	void RoadComponent::SetFlatTerrain(bool value)
	{
		if(!m_Initialized)
			return;
		
		WaypointListComponentPtr wpl = GetSceneObject()->GetFirstComponentByClass<WaypointListComponent>();
		if(wpl)
		{

			std::vector<Vec3> points = wpl->GetWaypoints();
			LocationComponentPtr location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
			const Vec3 origo = location->GetWorldPosition();
			//create absolute positions
			for(int i = 0 ; i < points.size(); i++)
				points[i] = points[i] + origo;


			TerrainComponentPtr terrain = GetSceneObject()->GetSceneObjectManager()->GetSceneRoot()->GetFirstComponentByClass<ITerrainComponent>(true);
			if(terrain)
			{
				BaseSceneComponentPtr bsc = boost::shared_dynamic_cast<BaseSceneComponent>(terrain);
				bsc->GetSceneObject()->PostMessage(MessagePtr(new RoadMessage(points,m_FlatWidth,m_FlatFade,TL_0)));
			}
		}
	}

	bool RoadComponent::GetFlatTerrain() const
	{
		return false;
	}
}
