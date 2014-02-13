
#ifndef GASS_RN_ROAD_NODE_COMPONENT
#define GASS_RN_ROAD_NODE_COMPONENT

#include "Sim/GASS.h"
#include "Core/Utils/GASSFilePath.h"
#include "Sim/Interface/GASSIGraphNodeComponent.h"
#include "Sim/Interface/GASSIGraphEdgeComponent.h"

namespace GASS
{
	class RNRoadNodeComponent :  public Reflection<RNRoadNodeComponent,BaseSceneComponent> , public IGraphNodeComponent
	{
	public:
		RNRoadNodeComponent(void);
		~RNRoadNodeComponent(void);
		void OnInitialize();
		void OnDelete();
		static void RegisterReflection();
		virtual void AddEdge(GraphEdgeComponentPtr edge) {m_Edges.push_back(edge);}
		virtual void RemoveEdge(GraphEdgeComponentPtr edge);
		std::vector<GraphEdgeComponentWeakPtr> GetEdges() {return m_Edges;}
	private:
		void OnTransformation(TransformationNotifyMessagePtr message);
		std::vector<GraphEdgeComponentWeakPtr> m_Edges;
	};
	typedef SPTR<RNRoadNodeComponent> RNRoadNodeComponentPtr;
	typedef WPTR<RNRoadNodeComponent> RNRoadNodeComponentWeakPtr;
}
#endif