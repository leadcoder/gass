
#ifndef GASS_RN_ROAD_NODE_COMPONENT
#define GASS_RN_ROAD_NODE_COMPONENT

#include "Sim/GASS.h"
#include "Sim/Interface/GASSIGraphNodeComponent.h"
#include "Sim/Interface/GASSIGraphEdgeComponent.h"

namespace GASS
{
	class RNRoadNodeComponent :  public Reflection<RNRoadNodeComponent,BaseSceneComponent> , public IGraphNodeComponent
	{
	public:
		RNRoadNodeComponent(void);
		~RNRoadNodeComponent(void) override;
		void OnInitialize() override;
		void OnDelete() override;
		static void RegisterReflection();
		void AddEdge(GraphEdgeComponentPtr edge) override {m_Edges.push_back(edge);}
		void RemoveEdge(GraphEdgeComponentPtr edge) override;
		std::vector<GraphEdgeComponentWeakPtr> GetEdges() const {return m_Edges;}
	private:
		//void OnTransformation(TransformationChangedEventPtr message);
		void OnTransformation(TransformationChangedEventPtr event);
		std::vector<GraphEdgeComponentWeakPtr> m_Edges;
	};
	typedef GASS_SHARED_PTR<RNRoadNodeComponent> RNRoadNodeComponentPtr;
	typedef GASS_WEAK_PTR<RNRoadNodeComponent> RNRoadNodeComponentWeakPtr;
}
#endif