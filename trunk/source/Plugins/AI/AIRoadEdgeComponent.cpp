#include <limits>
#include "AIRoadEdgeComponent.h"
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
	AIRoadEdgeComponent::AIRoadEdgeComponent(void) 
	{

	}	

	AIRoadEdgeComponent::~AIRoadEdgeComponent(void)
	{

	}

	void AIRoadEdgeComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("AIRoadEdgeComponent",new Creator<AIRoadEdgeComponent, IComponent>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("AIRoadEdgeComponent", OF_VISIBLE)));
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
		//update graph
		//GraphComponentPtr graph = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<IGraphComponent>();
		//GASSAssert(graph,"Failed to find IGraphComponent in AIRoadEdgeComponent::OnDelete()");
		//graph->RebuildGraph();
	}
}
