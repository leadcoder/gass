#include "AIRoadNetwork.h"
#include "AISceneManager.h"
#include "AIRoadLaneSectionComponent.h"
#include "AIRoadLaneComponent.h"
#include "AIRoadIntersectionComponent.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Game/GameMessages.h"
#include "RoadIntersectionComponent.h"
#include "Sim/Interface/GASSIWaypointListComponent.h"
#include <algorithm>


namespace GASS
{
	AIRoadNetwork::AIRoadNetwork(void) 
	{
		
	}	

	AIRoadNetwork::~AIRoadNetwork(void)
	{

	}

	void AIRoadNetwork::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("AIRoadNetwork",new Creator<AIRoadNetwork, IComponent>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("AIRoadNetwork", OF_VISIBLE)));
	}

	void AIRoadNetwork::OnInitialize()
	{
		
	}

	void AIRoadNetwork::Rebuild()
	{
		//Get all roads and build search graph!
		IComponentContainer::ComponentVector components;
		GetSceneObject()->GetComponentsByClass<AIRoadComponent>(components);
		for(size_t i = 0 ;  i < components.size(); i++)
		{
			AIRoadComponentPtr road_comp = DYNAMIC_PTR_CAST<AIRoadComponent>(components[i]);
			std::vector<AIRoadLaneSectionComponentPtr> ls = road_comp->GetLaneSections();
			for(size_t j = 0; j < ls.size(); j++)
			{
				std::vector<AIRoadLaneComponentPtr>  lanes = ls[j]->GetLanes();
				for(size_t k = 0; k < lanes.size(); k++)
				{
					lanes[k]->m_Edge = 0;
				}
			}
		}


		for(size_t i = 0 ;  i < components.size(); i++)
		{
			AIRoadComponentPtr road_comp = DYNAMIC_PTR_CAST<AIRoadComponent>(components[i]);
			std::vector<AIRoadLaneSectionComponentPtr> ls = road_comp->GetLaneSections();
			for(size_t j = 0; j < ls.size(); j++)
			{
				std::vector<AIRoadLaneComponentPtr>  lanes = ls[j]->GetLanes();
				for(size_t k = 0; k < lanes.size(); k++)
				{
					if(lanes[k]->m_Edge == NULL)
						AddLane(lanes[k], NULL);
				}
			}
		}
	}

	void AIRoadNetwork::AddLane(AIRoadLaneComponentPtr lane, RoadEdge* prev_edge)
	{
		std::vector<Vec3> points = lane->GetWaypoints();
		RoadNode* start_node;
		if(prev_edge)
		{
			start_node = prev_edge->EndNode;
		}
		else
			start_node = new RoadNode();

		RoadNode* end_node = new RoadNode();
		RoadEdge* edge = new RoadEdge();
		edge->StartNode = start_node;
		edge->EndNode = end_node;
		start_node->Edges.push_back(edge);
		lane->m_Edge =edge;
		std::vector<AIRoadLaneComponentPtr>* connections = lane->GetNextLanesPtr();
		if(connections->size() > 0)
		{
			for(size_t l = 0; l < connections->size() ; l++)
			{
				AIRoadLaneComponentPtr next_lane = connections->at(l);
				if(next_lane->m_Edge == NULL)
				{
					AddLane(next_lane,edge);
				}
			}
		}
		else //?
		{

		}
	}
	
}
