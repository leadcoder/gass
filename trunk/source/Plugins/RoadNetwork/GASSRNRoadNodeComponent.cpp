#include <limits>
#include "GASSRNRoadNodeComponent.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Game/GameMessages.h"
#include "Sim/Interface/GASSIWaypointListComponent.h"
#include "Sim/Interface/GASSIGraphComponent.h"

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
		ComponentFactory::GetPtr()->Register("RNRoadNodeComponent",new Creator<RNRoadNodeComponent, IComponent>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("RNRoadNodeComponent", OF_VISIBLE)));
	}

	void RNRoadNodeComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(RNRoadNodeComponent::OnTransformation,TransformationNotifyMessage,0));
	}

	void RNRoadNodeComponent::OnTransformation(TransformationNotifyMessagePtr message)
	{
		//update graph
		GraphComponentPtr graph = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<IGraphComponent>();
		GASSAssert(graph,"Failed to find IGraphComponent in RNRoadNodeComponent::OnTransformation");
		graph->RebuildGraph();
	}

	void RNRoadNodeComponent::OnDelete()
	{
		//Delete all edges
		std::vector<GraphEdgeComponentWeakPtr> edges =  m_Edges;
		for (size_t i= 0; i < edges.size(); i++)
		{
			GraphEdgeComponentPtr edge(edges[i],NO_THROW);
			if(edge)
			{
				//Delete edge object
				BaseSceneComponentPtr bso = DYNAMIC_PTR_CAST<BaseSceneComponent>(edge);
				//GetSceneObject()->GetParentSceneObject()->RemoveChildSceneObject(bso->GetSceneObject());
				GetSceneObject()->GetScene()->PostMessage(SceneMessagePtr(new RemoveSceneObjectRequest(bso->GetSceneObject())));
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
			GraphEdgeComponentPtr edge_ptr(*iter,NO_THROW);
			if(edge_ptr == edge)
			{
				iter = m_Edges.erase(iter);
			}
			else
				iter++;
		}
	}
}