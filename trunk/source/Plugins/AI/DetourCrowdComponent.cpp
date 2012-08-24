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

#include "DetourCrowdComponent.h"
#include "DetourCrowd/DetourCrowd.h"
#include "AISceneManager.h"

namespace GASS
{
	static const int MAX_AGENTS = 1024;

	DetourCrowdComponent::DetourCrowdComponent() :	m_ScatteringRadius(10),
		m_NumberOfScattringCharacters(2),
		m_ShowDebug(false),
		m_Initialized(false),
		m_Crowd(NULL),
		m_AnticipateTurns(true),
		m_OptimizeVis(true),
		m_OptimizeTopo(true),
		m_Separation(true),
		m_ObstacleAvoidance(true),
		m_SeparationWeight(1.0),
		m_DefaultAgentHeight(1.75),
		m_DefaultAgentRadius(0.3)
	{

	}

	DetourCrowdComponent::~DetourCrowdComponent()
	{
		dtFreeCrowd(m_Crowd);
	}

	void DetourCrowdComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("DetourCrowdComponent",new Creator<DetourCrowdComponent, IComponent>);
		RegisterVectorProperty<std::string>("ScatteringSelection", &GetScatteringSelection, &SetScatteringSelection);
		RegisterProperty<int>("NumberOfCharacters", &GetNumberOfCharacters, &SetNumberOfCharacters);
		RegisterProperty<bool>("CircularScattering", &GetCircularScattering, &SetCircularScattering);
		RegisterProperty<float>("CircularScatteringRadius", &GetCircularScatteringRadius, &SetCircularScatteringRadius);
		RegisterProperty<bool>("NavMeshScattering", &GetNavMeshScattering, &SetNavMeshScattering);
		RegisterProperty<std::string>("NavigationMesh", &GetNavigationMesh, &SetNavigationMesh);
		RegisterProperty<std::string>("Script", &GetScript, &SetScript);
		RegisterProperty<bool>("AnticipateTurns", &GetAnticipateTurns, &SetAnticipateTurns);
		RegisterProperty<bool>("OptimizeVis", &GetOptimizeVis, &SetOptimizeVis);
		RegisterProperty<bool>("OptimizeTopo", &GetOptimizeTopo, &SetOptimizeTopo);
		RegisterProperty<bool>("Separation", &GetSeparation, &SetSeparation);
		RegisterProperty<bool>("ObstacleAvoidance", &GetObstacleAvoidance, &SetObstacleAvoidance);
		RegisterProperty<float>("SeparationWeight", &GetSeparationWeight, &SetSeparationWeight);
		RegisterProperty<float>("DefaultAgentRadius", &GetDefaultAgentRadius, &SetDefaultAgentRadius);
		RegisterProperty<float>("DefaultAgentHeight", &GetDefaultAgentHeight, &SetDefaultAgentHeight);
	}

	void DetourCrowdComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(DetourCrowdComponent::OnLoad,LoadComponentsMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(DetourCrowdComponent::OnMoved,PositionMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(DetourCrowdComponent::OnUnload,UnloadComponentsMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(DetourCrowdComponent::OnChangeName,SceneObjectNameMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(DetourCrowdComponent::OnGoToPosisiton,GotoPositionMessage,0));

		SceneManagerListenerPtr listener = shared_from_this();
		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<AISceneManager>()->Register(listener);
	}

	void DetourCrowdComponent::OnLoad(LoadComponentsMessagePtr message)
	{
		
		//Get nav mesh by name?
		RecastNavigationMeshComponentPtr nm_comp = FindNavMesh(m_NavMeshName);
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

	void DetourCrowdComponent::SetNavigationMesh(const std::string &value)
	{
		bool new_mesh = false;
		if(m_NavMeshName != value)
			new_mesh = true;

		m_NavMeshName = value;

		if(m_NavMeshName != "" && m_Initialized)
		{
			RecastNavigationMeshComponentPtr  mesh = FindNavMesh(m_NavMeshName);
			if(new_mesh && mesh)
			{
				m_Mesh = mesh;
				if(!m_Crowd)
					m_Crowd = dtAllocCrowd();
				InitCrowd(m_Crowd,mesh->GetNavMesh());

				IComponentContainer::ComponentContainerIterator children = GetSceneObject()->GetChildren();
				while(children.hasMoreElements())
				{
					SceneObjectPtr child_obj =  boost::shared_static_cast<SceneObject>(children.getNext());
					DetourCrowdAgentComponentPtr agent = child_obj->GetFirstComponentByClass<DetourCrowdAgentComponent>();
					if(agent)
					{
						agent->Init(m_Crowd);
					}
				}
			}
		}
	}


	std::string DetourCrowdComponent::GetNavigationMesh()const 
	{
		return m_NavMeshName;
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

	void DetourCrowdComponent::SetScript(const std::string &value)
	{
		m_Script = value;

		if(m_Script != "" && m_Initialized)
		{
			IComponentContainer::ComponentContainerIterator children = GetSceneObject()->GetChildren();
			while(children.hasMoreElements())
			{
				SceneObjectPtr child_obj =  boost::shared_static_cast<SceneObject>(children.getNext());
			}
		}
	}

	std::string DetourCrowdComponent::GetScript() const
	{
		return m_Script;
	}


	void DetourCrowdComponent::OnChangeName(GASS::MessagePtr message)
	{

	}

	void DetourCrowdComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
		m_DebugLines.reset();
	}

	void DetourCrowdComponent::SceneManagerTick(double delta_time)
	{
		if(m_Crowd)
			m_Crowd->update(delta_time, NULL);//&agentDebug);

		//update location for all agents
		for(int i = 0; i < m_Agents.size(); i++)
		{
			m_Agents[i]->UpdateLocation(delta_time);
		}
	}

	void DetourCrowdComponent::OnMoved(PositionMessagePtr message)
	{

	}

	void DetourCrowdComponent::SetScatteringSelection(const std::vector<std::string> &selection) 
	{
		m_ScatteringSelection = selection;
	}

	std::vector<std::string> DetourCrowdComponent::GetScatteringSelection() const 
	{
		return m_ScatteringSelection;
	}


	void DetourCrowdComponent::SetNumberOfCharacters(int value) 
	{
		m_NumberOfScattringCharacters = value;
	}

	int DetourCrowdComponent::GetNumberOfCharacters() const 
	{
		return m_NumberOfScattringCharacters;
	}



	bool DetourCrowdComponent::GetCircularScattering() const 
	{
		return false;
	}

	void DetourCrowdComponent::SetCircularScattering(bool value)
	{
		if(value)
		{
			Vec3 pos = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
			CircularScattering(m_ScatteringSelection,m_NumberOfScattringCharacters, pos, m_ScatteringRadius);
		}
	}


	bool DetourCrowdComponent::GetNavMeshScattering() const 
	{
		return false;
	}

	void DetourCrowdComponent::SetNavMeshScattering(bool value)
	{
		if(value)
		{
			NavMeshScattering(m_ScatteringSelection,m_NumberOfScattringCharacters);
		}
	}

	void DetourCrowdComponent::ReleaseAllChildren()
	{
		IComponentContainer::ComponentContainerIterator children = GetSceneObject()->GetChildren();
		while(children.hasMoreElements())
		{
			SceneObjectPtr child_obj =  boost::shared_static_cast<SceneObject>(children.getNext());
			DetourCrowdAgentComponentPtr agent = child_obj->GetFirstComponentByClass<DetourCrowdAgentComponent>();
			if(agent)
			{
				GetSceneObject()->RemoveChild(child_obj);
				children = GetSceneObject()->GetChildren();
			}
		}
	}

	void DetourCrowdComponent::OnGoToPosisiton(GotoPositionMessagePtr message)
	{
		DetourCrowdComponentPtr crowd_comp = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<DetourCrowdComponent>();
		//relay message to children

		IComponentContainer::ComponentVector components;
		GetSceneObject()->GetComponentsByClass<DetourCrowdAgentComponent>(components);
		for(int i = 0; i < components.size(); i++)
		{
			DetourCrowdAgentComponentPtr comp = boost::shared_dynamic_cast<DetourCrowdAgentComponent>(components[i]);
			comp->GetSceneObject()->SendImmediate(message);
		}
	}


	static float frand()
	{
		return (float)rand()/(float)RAND_MAX;
	}


	bool DetourCrowdComponent::GetRandomPointInCircle(Vec3 &pos, const Vec3 &center, float radius)
	{
		RecastNavigationMeshComponentPtr nav_mesh = FindNavMesh(m_NavMeshName);
		dtNavMeshQuery* navquery = nav_mesh->GetNavMeshQuery();
		dtCrowd* crowd = GetCrowd();
		const dtQueryFilter* filter = crowd->getFilter();
		const float* ext = crowd->getQueryExtents();
		dtPolyRef start_ref;
		float c_pos[3];
		c_pos[0] = center.x;
		c_pos[1] = center.y;
		c_pos[2] = center.z;

		float q_pos[3];
		navquery->findNearestPoly(c_pos, ext, filter, &start_ref, q_pos);
		if(start_ref)
		{
			dtStatus status = DT_FAILURE;
			dtPolyRef end_ref;
			float end_pos[3];
			status = navquery->findRandomPointAroundCircle(start_ref, q_pos, radius, filter, frand, &end_ref, end_pos);
			if (dtStatusSucceed(status))
			{
				pos.Set(end_pos[0],end_pos[1],end_pos[2]);
				return true;
			}
		}
		return false;
	}


	void DetourCrowdComponent::CircularScattering(const std::vector<std::string>   &selection, int num_vehicles, const GASS::Vec3 &start_pos, double radius)
	{
		//release all previous allocated characters
		ReleaseAllChildren();

		RecastNavigationMeshComponentPtr mesh = FindNavMesh(m_NavMeshName);

		//Load character
		int vehicle_index= 0;
		int vehicle_counter =0;
		int crowd_terrain_id = 0;
		if(mesh)
		{
			crowd_terrain_id = 0;//mesh->GetTerrainID(start_pos);
		}

		if(selection.size() > 0)
		{
			while(vehicle_counter < num_vehicles)
			{
				//Loop the character list
				const std::string vehicle_name = selection[vehicle_index++];
				if(vehicle_index > selection.size()-1)
					vehicle_index = 0;

				Vec3 pos;
				Float v1,v2;
				do
				{
					v1 = (((Float ) rand ()) / ((Float ) RAND_MAX));
					v1 = (v1*2-1)*radius;
					v2 = (((Float ) rand ()) / ((Float ) RAND_MAX));
					v2 = (v2*2-1)*radius;

				} while((v1*v1 + v2*v2) > radius*radius);

				pos = Vec3(v1,0,v2);
				pos = start_pos + pos;

				int id = crowd_terrain_id;
				if(mesh)
				{
					id = 0;//mesh->GetTerrainID(pos);
					GASS::Vec3 isec_pos;
					GASS::Vec3 isec_norm;
					float dist = -1;
					bool hit = false;//mesh->FindDistanceToWall(pos, 2.5, isec_pos, isec_norm, dist);
					if(hit) 
						id = 0;
				}

				if(id == crowd_terrain_id) //match terrain id
				{
					//CreateFromTemplate
					SceneObjectPtr vehicle = GetSceneObject()->GetScene()->LoadObjectFromTemplate(vehicle_name,GetSceneObject());
					if(vehicle)
					{
						vehicle->PostMessage(MessagePtr(new WorldPositionMessage(pos)));
						vehicle_counter++;
					}
				}
			}
		}

		//initlize script and navmesh for all created new vehicles
		SetNavigationMesh(m_NavMeshName);
		//SetWaypointList(m_WaypointListName);
		SetScript(m_Script);
		//std::cout << "Number of characters:" << m_Characters.size() << std::endl;
	}


	void DetourCrowdComponent::NavMeshScattering(const std::vector<std::string>   &selection, int num_vehicles)
	{
		//release all previous allocated characters
		ReleaseAllChildren();
		RecastNavigationMeshComponentPtr mesh = FindNavMesh(m_NavMeshName);

		if(!mesh)
			return;

		//Load character
		int vehicle_index= 0;
		int vehicle_counter =0;
		int crowd_terrain_id = 0;
		
		if(selection.size() > 0)
		{
			while(vehicle_counter < num_vehicles)
			{
				//Loop the character list
				const std::string vehicle_name = selection[vehicle_index++];
				if(vehicle_index > selection.size()-1)
					vehicle_index = 0;


				Vec3 center = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
				Vec3 pos;
				GetRandomPointInCircle(pos,center,m_ScatteringRadius);
				//Vec3 pos = mesh->GetRandomPoint();
				//CreateFromTemplate
				SceneObjectPtr vehicle = GetSceneObject()->GetScene()->LoadObjectFromTemplate(vehicle_name,GetSceneObject());
				if(vehicle)
				{
					vehicle->PostMessage(MessagePtr(new WorldPositionMessage(pos)));
					vehicle_counter++;
				}
				
			}
		}
		//initlize script and navmesh for all created new vehicles
		SetNavigationMesh(m_NavMeshName);
		SetScript(m_Script);
	}



	void DetourCrowdComponent::SetCircularScatteringRadius(float value)
	{
		m_ScatteringRadius = value;
	}

	float DetourCrowdComponent::GetCircularScatteringRadius() const
	{
		return m_ScatteringRadius;
	}

	bool DetourCrowdComponent::GetShowDebug() const 
	{
		return m_ShowDebug;
	}

	void DetourCrowdComponent::SetShowDebug(bool value)
	{
		m_ShowDebug = value;
	}

	void DetourCrowdComponent::SetDefaultAgentRadius(float value)
	{
		m_DefaultAgentRadius = value;
		if(m_Crowd)
		{
			IComponentContainer::ComponentVector components;
			GetSceneObject()->GetComponentsByClass<DetourCrowdAgentComponent>(components);
			for(int i = 0; i < components.size(); i++)
			{
				DetourCrowdAgentComponentPtr comp = boost::shared_dynamic_cast<DetourCrowdAgentComponent>(components[i]);
				comp->SetRadius(value);
			}
		}
	}


	void DetourCrowdComponent::SetSeparationWeight(float value)
	{
		m_SeparationWeight = value;
		if(m_Crowd)
		{
			IComponentContainer::ComponentVector components;
			GetSceneObject()->GetComponentsByClass<DetourCrowdAgentComponent>(components);
			for(int i = 0; i < components.size(); i++)
			{
				DetourCrowdAgentComponentPtr comp = boost::shared_dynamic_cast<DetourCrowdAgentComponent>(components[i]);
				comp->SetSeparationWeight(m_SeparationWeight);
			}
		}
	}

	float DetourCrowdComponent::GetDefaultAgentRadius() const
	{
		return m_DefaultAgentRadius;
	}


	void DetourCrowdComponent::SetDefaultAgentHeight(float value)
	{
		m_DefaultAgentHeight = value;
		if(m_Crowd)
		{
			IComponentContainer::ComponentVector components;
			GetSceneObject()->GetComponentsByClass<DetourCrowdAgentComponent>(components);
			for(int i = 0; i < components.size(); i++)
			{
				DetourCrowdAgentComponentPtr comp = boost::shared_dynamic_cast<DetourCrowdAgentComponent>(components[i]);
				comp->SetHeight(value);
			}
		}
	}

	float DetourCrowdComponent::GetDefaultAgentHeight() const
	{
		return m_DefaultAgentHeight;
	}
}



