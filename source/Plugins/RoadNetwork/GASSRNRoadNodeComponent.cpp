#include "GASSRNRoadNodeComponent.h"
#include "Plugins/Base/CoreMessages.h"
#include "Sim/Interface/GASSIGraphComponent.h"
#include <limits>

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
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("RNRoadNodeComponent", OF_VISIBLE)));
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
				BaseSceneComponentPtr bso = GASS_DYNAMIC_PTR_CAST<BaseSceneComponent>(edge);
				bso->GetSceneObject()->SendRemoveRequest(0);
			}
		}
		//update graph
		GraphComponentPtr graph = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<IGraphComponent>();
		GASSAssert(graph,"Failed to find IGraphComponent in RNRoadEdgeComponent::OnDelete()");
		graph->RebuildGraph();
	}

	void RNRoadNodeComponent::RemoveEdge(GraphEdgeComponentPtr edge)
	{
		std::vector<GraphEdgeComponentWeakPtr>::iterator iter = m_Edges.begin();
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
