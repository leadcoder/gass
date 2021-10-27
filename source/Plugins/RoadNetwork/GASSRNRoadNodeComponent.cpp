#include "GASSRNRoadNodeComponent.h"
#include "Plugins/Base/CoreMessages.h"
#include "Sim/Interface/GASSIGraphComponent.h"
#include <limits>
#include <memory>

namespace GASS
{
	RNRoadNodeComponent::RNRoadNodeComponent(void) 
	{

	}	

	RNRoadNodeComponent::~RNRoadNodeComponent(void)
	{

	}

	void RNRoadNodeComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register<RNRoadNodeComponent>();
		GetClassRTTI()->SetMetaData(std::make_shared<ClassMetaData>("RNRoadNodeComponent", OF_VISIBLE));
	}

	void RNRoadNodeComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(RNRoadNodeComponent::OnTransformation,TransformationChangedEvent,0));
	}

	void RNRoadNodeComponent::OnTransformation(TransformationChangedEventPtr event)
	{
		//update graph
		GraphComponentPtr graph = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<IGraphComponent>();
		GASSAssert(graph,"Failed to find IGraphComponent in RNRoadNodeComponent::OnTransformation");
		graph->RebuildGraph();
	}

	/*void RNRoadNodeComponent::OnTransformation(TransformationChangedEventPtr message)
	{
		//update graph
		GraphComponentPtr graph = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<IGraphComponent>();
		GASSAssert(graph,"Failed to find IGraphComponent in RNRoadNodeComponent::OnTransformation");
		graph->RebuildGraph();
	}*/

	void RNRoadNodeComponent::OnDelete()
	{
		//Delete all edges
		std::vector<GraphEdgeComponentWeakPtr> edges =  m_Edges;
		for (size_t i= 0; i < edges.size(); i++)
		{
			GraphEdgeComponentPtr edge = edges[i].lock();
			if(edge)
			{
				//Delete edge object
				auto component = GASS_DYNAMIC_PTR_CAST<Component>(edge);
				component->GetSceneObject()->SendRemoveRequest(0);
			}
		}
		//update graph
		GraphComponentPtr graph = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<IGraphComponent>();
		GASSAssert(graph,"Failed to find IGraphComponent in RNRoadEdgeComponent::OnDelete()");
		graph->RebuildGraph();
	}

	void RNRoadNodeComponent::RemoveEdge(GraphEdgeComponentPtr edge)
	{
		auto iter = m_Edges.begin();
		while(iter !=m_Edges.end())
		{
			GraphEdgeComponentPtr edge_ptr = (*iter).lock();
			if(edge_ptr == edge)
			{
				iter = m_Edges.erase(iter);
			}
			else
				++iter;
		}
	}
}
