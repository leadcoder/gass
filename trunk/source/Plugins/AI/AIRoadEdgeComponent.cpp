#include <limits>
#include "AIRoadEdgeComponent.h"
#include "AIRoadNodeComponent.h"

#include "AISceneManager.h"
#include "AIRoadLaneSectionComponent.h"
#include "AIRoadLaneComponent.h"

#include "AIRoadIntersectionComponent.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Game/GameMessages.h"
#include "RoadIntersectionComponent.h"
#include "Sim/Interface/GASSIWaypointListComponent.h"
#include "Sim/Interface/GASSIGraphComponent.h"

#include <algorithm>


namespace GASS
{
	AIRoadEdgeComponent::AIRoadEdgeComponent(void) : m_LaneWidth(2)
	{

	}	

	AIRoadEdgeComponent::~AIRoadEdgeComponent(void)
	{

	}

	void AIRoadEdgeComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("AIRoadEdgeComponent",new Creator<AIRoadEdgeComponent, IComponent>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("AIRoadEdgeComponent", OF_VISIBLE)));
		RegisterProperty<Float>("LaneWidth", &AIRoadEdgeComponent::GetLaneWidth, &AIRoadEdgeComponent::SetLaneWidth,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Lane width",PF_VISIBLE | PF_EDITABLE)));
	}

	void AIRoadEdgeComponent::OnInitialize()
	{

	}

	void AIRoadEdgeComponent::OnDelete()
	{
		GraphNodeComponentPtr start_node = DYNAMIC_PTR_CAST<IGraphNodeComponent>(GetStartNode());
		GraphNodeComponentPtr end_node = DYNAMIC_PTR_CAST<IGraphNodeComponent>(GetEndNode());
		GraphEdgeComponentPtr edge = DYNAMIC_PTR_CAST<IGraphEdgeComponent>(shared_from_this());
		if(start_node)
			start_node->RemoveEdge(edge);
		if(end_node)
			end_node->RemoveEdge(edge);
	}

	WaypointListComponentPtr AIRoadEdgeComponent::GetWaypointList() const
	{
		return GetSceneObject()->GetChildByID("EDGE_WAYPOINTS")->GetFirstComponentByClass<IWaypointListComponent>();
	}

	std::vector<Vec3> AIRoadEdgeComponent::GetWaypoints() const 
	{
		std::vector<Vec3> road_wps;
		if(GetStartNode())
		{
			AIRoadNodeComponentPtr start_rn = DYNAMIC_PTR_CAST<AIRoadNodeComponent>(GetStartNode());
			Vec3 start_pos  = start_rn->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
			road_wps.push_back(start_pos);
		}
		std::vector<SceneObjectPtr> objects;
		GetSceneObject()->GetChildrenByID(objects,"ROAD_WP",true,true);
		
		for(size_t i = 0; i < objects.size(); i++)
		{
			LocationComponentPtr wp_location = objects[i]->GetFirstComponentByClass<ILocationComponent>();
			road_wps.push_back(wp_location->GetPosition());
		}
		
		if(GetEndNode())
		{
			AIRoadNodeComponentPtr end_rn = DYNAMIC_PTR_CAST<AIRoadNodeComponent>(GetEndNode());
			Vec3 end_pos  = end_rn->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
			road_wps.push_back(end_pos);
		}
		return road_wps;
	}

	SceneObjectPtr AIRoadEdgeComponent::GetWaypointListObject() const
	{
		return GetSceneObject()->GetChildByID("EDGE_WAYPOINTS");
	}

	void AIRoadEdgeComponent::SetWaypoints(const std::vector<Vec3> &wps)
	{
		//Add waypoints
		if(wps.size() > 2)
		{
			for(size_t i = 1; i < wps.size()-1; i++)
			{
				SceneObjectPtr node_obj = GetSceneObject()->GetScene()->LoadObjectFromTemplate("RoadWP",GetSceneObject());

				node_obj->RegisterForMessage(REG_TMESS(AIRoadEdgeComponent::OnTransformation,TransformationNotifyMessage,0));
				GASSAssert(node_obj,"Failed to create scene object in void AIRoadEdgeComponent::SetWaypoints");
				node_obj->SendImmediate(MessagePtr(new GASS::WorldPositionMessage(wps[i])));
			}
		}
	}

	void AIRoadEdgeComponent::OnTransformation(TransformationNotifyMessagePtr message)
	{
		//update graph
		GraphComponentPtr graph = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<IGraphComponent>();
		GASSAssert(graph,"Failed to find IGraphComponent in AIRoadEdgeComponent::OnTransformation");
		graph->RebuildGraph();
	}
}
