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

#include "DetourCrowdComponent.h"
#include "RecastNavigationMessages.h"
#include "Sim/Interface/GASSIMissionSceneManager.h"
#include "RecastIncludes.h"

namespace GASS
{
	static const int MAX_AGENTS = 1024;

	DetourCrowdComponent::DetourCrowdComponent() :
		m_ShowDebug(false),
		m_Initialized(false),
		m_Crowd(NULL),
		m_AnticipateTurns(true),
		m_OptimizeVis(true),
		m_OptimizeTopo(true),
		m_Separation(true),
		m_ObstacleAvoidance(true),
		m_SeparationWeight(1.0f),
		//m_DefaultAgentHeight(1.75),
		m_DefaultAgentRadius(0.3f)
	{

	}

	DetourCrowdComponent::~DetourCrowdComponent()
	{
		dtFreeCrowd(m_Crowd);
	}

	void DetourCrowdComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("DetourCrowdComponent",new Creator<DetourCrowdComponent, Component>);
		RegisterProperty<bool>("AnticipateTurns", &DetourCrowdComponent::GetAnticipateTurns, &DetourCrowdComponent::SetAnticipateTurns);
		RegisterProperty<bool>("OptimizeVis", &DetourCrowdComponent::GetOptimizeVis, &DetourCrowdComponent::SetOptimizeVis);
		RegisterProperty<bool>("OptimizeTopo", &DetourCrowdComponent::GetOptimizeTopo, &DetourCrowdComponent::SetOptimizeTopo);
		RegisterProperty<bool>("Separation", &DetourCrowdComponent::GetSeparation, &DetourCrowdComponent::SetSeparation);
		RegisterProperty<bool>("ObstacleAvoidance", &DetourCrowdComponent::GetObstacleAvoidance, &DetourCrowdComponent::SetObstacleAvoidance);
		RegisterProperty<float>("DefaultAgentRadius", &DetourCrowdComponent::GetDefaultAgentRadius, &DetourCrowdComponent::SetDefaultAgentRadius);
	}

	void DetourCrowdComponent::OnInitialize()
	{
		SceneManagerListenerPtr listener = shared_from_this();
		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<IMissionSceneManager>()->Register(listener);

		//Get nav mesh by name?
		SceneObjectPtr obj = GetSceneObject()->GetScene()->GetRootSceneObject()->GetChildByID("AI_NAVIGATION_MESH");
		if(obj)
		{
			RecastNavigationMeshComponentPtr nm_comp = obj->GetFirstComponentByClass<RecastNavigationMeshComponent>();
			if(nm_comp)
			{
				dtNavMesh* nav = nm_comp->GetNavMesh();
				if(nav)
				{
					m_Mesh = nm_comp;
					m_Crowd = dtAllocCrowd();
					InitCrowd(m_Crowd,nav);
				}
			}
		}
		m_Initialized = true;
	}

	void DetourCrowdComponent::RegisterAgent(DetourCrowdAgentComponentPtr agent)
	{
		m_Agents.push_back(agent);
	}

	void DetourCrowdComponent::UnregisterAgent(DetourCrowdAgentComponentPtr agent)
	{
		std::vector<DetourCrowdAgentComponentPtr>::iterator iter = m_Agents.begin();
		while(iter != m_Agents.end())
		{
			if(*iter == agent)
				iter = m_Agents.erase(iter);
			else
				iter++;
		}

		//remove from crowd

		if(m_Crowd)
		{
			m_Crowd->removeAgent(agent->GetIndex());
		}
	}

	void DetourCrowdComponent::InitCrowd(dtCrowd* crowd, dtNavMesh* nav )
	{
		if (nav && crowd)
		{
			crowd->init(MAX_AGENTS, m_DefaultAgentRadius, nav);

			// Make polygons with 'disabled' flag invalid.
			crowd->getEditableFilter()->setExcludeFlags(SAMPLE_POLYFLAGS_DISABLED);

			// Setup local avoidance params to different qualities.
			dtObstacleAvoidanceParams params;
			// Use mostly default settings, copy from dtCrowd.
			memcpy(&params, crowd->getObstacleAvoidanceParams(0), sizeof(dtObstacleAvoidanceParams));

			// Low (11)
			params.velBias = 0.5f;
			params.adaptiveDivs = 5;
			params.adaptiveRings = 2;
			params.adaptiveDepth = 1;
			crowd->setObstacleAvoidanceParams(0, &params);

			// Medium (22)
			params.velBias = 0.5f;
			params.adaptiveDivs = 5;
			params.adaptiveRings = 2;
			params.adaptiveDepth = 2;
			crowd->setObstacleAvoidanceParams(1, &params);

			// Good (45)
			params.velBias = 0.5f;
			params.adaptiveDivs = 7;
			params.adaptiveRings = 2;
			params.adaptiveDepth = 3;
			crowd->setObstacleAvoidanceParams(2, &params);

			// High (66)
			params.velBias = 0.5f;
			params.adaptiveDivs = 7;
			params.adaptiveRings = 3;
			params.adaptiveDepth = 3;
			crowd->setObstacleAvoidanceParams(3, &params);
		}
	}

	RecastNavigationMeshComponentPtr DetourCrowdComponent::FindNavMesh(const std::string &name) const
	{
		RecastNavigationMeshComponentPtr nav_mesh;
		std::vector<SceneObjectPtr> objs;
		GetSceneObject()->GetScene()->GetRootSceneObject()->GetChildrenByName(objs,name);
		if(objs.size() > 0)
		{
			SceneObjectPtr obj = objs.front();
			nav_mesh = obj->GetFirstComponentByClass<RecastNavigationMeshComponent>();
		}
		return nav_mesh;
	}

	void DetourCrowdComponent::OnDelete()
	{
		m_DebugLines.reset();
	}

	void DetourCrowdComponent::SceneManagerTick(double delta_time)
	{
		if(m_Crowd)
			m_Crowd->update(static_cast<float>(delta_time), NULL);//&agentDebug);

		//update location for all agents
		for(size_t i = 0; i < m_Agents.size(); i++)
		{
			m_Agents[i]->UpdateLocation(delta_time);
		}
	}

	void DetourCrowdComponent::SetDefaultAgentRadius(float value)
	{
		m_DefaultAgentRadius = value;
	}

	float DetourCrowdComponent::GetDefaultAgentRadius() const
	{
		return m_DefaultAgentRadius;
	}
}



