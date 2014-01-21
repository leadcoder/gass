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

		RegisterProperty<bool>("Build", &AIRoadNetwork::GetBuild, &AIRoadNetwork::SetBuild,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
	}

	void AIRoadNetwork::OnInitialize()
	{

	}

	bool AIRoadNetwork::GetBuild() const
	{
		return false;
	}

	void AIRoadNetwork::SetBuild(bool value) 
	{
		if(GetSceneObject())
			Rebuild();
	}

	void AIRoadNetwork::Rebuild()
	{
		//Get all roads and build search graph!
		IComponentContainer::ComponentVector components;
		GetSceneObject()->GetScene()->GetRootSceneObject()->GetComponentsByClass<AIRoadComponent>(components);
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

		//test
		RoadNavigation nav;
		std::vector<RoadNode*> path;
		//int ret = nav.GetPath(from_node, to_node, path);

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
		{
			start_node = new RoadNode();
			start_node->Position = points.front();
		}


		RoadNode* end_node = new RoadNode();
		end_node->Position = points.back();
		RoadEdge* edge = new RoadEdge();
		edge->Waypoints = points;
		edge->Distance =  Math::GetPathLength(points);
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



	void AIRoadNetwork::GenerateGraph()
	{
		std::map<AIRoadIntersectionComponentPtr,RoadNode*> node_mapping;
		IComponentContainer::ComponentVector components;
		GetSceneObject()->GetScene()->GetRootSceneObject()->GetComponentsByClass<AIRoadIntersectionComponent>(components);
		for(size_t i = 0 ;  i < components.size(); i++)
		{
			AIRoadIntersectionComponentPtr inter_comp = DYNAMIC_PTR_CAST<AIRoadIntersectionComponent>(components[i]);
			Vec3 inter_pos = inter_comp->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
			RoadNode* road_node = new RoadNode();
			road_node->Position = inter_pos;
			node_mapping[inter_comp] = road_node; 
		}

		components.clear();
		GetSceneObject()->GetScene()->GetRootSceneObject()->GetComponentsByClass<AIRoadComponent>(components);
		for(size_t i = 0 ;  i < components.size(); i++)
		{
			AIRoadComponentPtr road_comp = DYNAMIC_PTR_CAST<AIRoadComponent>(components[i]);
			std::vector<Vec3> road_wps = road_comp->GetWaypointsObject()->GetFirstComponentByClass<IWaypointListComponent>()->GetWaypoints();
			RoadEdge* edge = new RoadEdge();
			edge->Waypoints = road_wps;
			edge->Distance =  Math::GetPathLength(road_wps);
			if(road_comp->GetStartNode().IsValid())
			{
				AIRoadIntersectionComponentPtr start_inter = road_comp->GetStartNode()->GetFirstComponentByClass<AIRoadIntersectionComponent>();
				RoadNode* start_node = node_mapping.find(start_inter)->second;
				edge->StartNode = start_node;
				start_node->Edges.push_back(edge);
			}

			if(road_comp->GetEndNode().IsValid())
			{
				AIRoadIntersectionComponentPtr end_inter = road_comp->GetEndNode()->GetFirstComponentByClass<AIRoadIntersectionComponent>();
				RoadNode* end_node = node_mapping.find(end_inter)->second;
				edge->EndNode = end_node;
				end_node->Edges.push_back(edge);
			}
		}
	}
}
