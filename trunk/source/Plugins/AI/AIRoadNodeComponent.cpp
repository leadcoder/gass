#include <limits>
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
	AIRoadNodeComponent::AIRoadNodeComponent(void) 
	{

	}	

	AIRoadNodeComponent::~AIRoadNodeComponent(void)
	{

	}

	void AIRoadNodeComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("AIRoadNodeComponent",new Creator<AIRoadNodeComponent, IComponent>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("AIRoadNodeComponent", OF_VISIBLE)));
		
	}

	void AIRoadNodeComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(AIRoadNodeComponent::OnTransformation,TransformationNotifyMessage,0));
	}

	void AIRoadNodeComponent::OnTransformation(TransformationNotifyMessagePtr message)
	{
		//update graph
		GraphComponentPtr graph = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<IGraphComponent>();
		GASSAssert(graph,"Failed to find IGraphComponent in AIRoadEdgeComponent::OnDelete()");
		graph->RebuildGraph();
	}

	void AIRoadNodeComponent::OnDelete()
	{
		//Delete all edges
		std::vector<GraphEdgeComponentWeakPtr> edges =  m_Edges;
		for (size_t i= 0; i < edges.size(); i++)
		{
			GraphEdgeComponentPtr edge(edges[i]);
			if(edge)
			{
				//Delete edge object
				BaseSceneComponentPtr bso = DYNAMIC_PTR_CAST<BaseSceneComponent>(edge);
				GetSceneObject()->GetParentSceneObject()->RemoveChildSceneObject(bso->GetSceneObject());
			}
		}
		//update graph
		GraphComponentPtr graph = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<IGraphComponent>();
		GASSAssert(graph,"Failed to find IGraphComponent in AIRoadEdgeComponent::OnDelete()");
		graph->RebuildGraph();
	}
}
