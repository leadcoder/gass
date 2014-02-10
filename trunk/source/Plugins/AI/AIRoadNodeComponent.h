
#ifndef AI_ROAD_NODE_COMPONENT
#define AI_ROAD_NODE_COMPONENT

#include "Sim/GASS.h"
#include "Core/Utils/GASSFilePath.h"
#include "Plugins/Game/PlatformType.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/AI/AIRoadLaneComponent.h"

#include <tbb/spin_mutex.h>
#include <set>
#include <tbb/atomic.h>
#include "Plugins/AI/micropather.h"
#include "Plugins/Game/GameMessages.h"
#include "Sim/Interface/GASSIGraphNodeComponent.h"
#include "Sim/Interface/GASSIGraphEdgeComponent.h"

namespace GASS
{

	class AIRoadNodeComponent :  public Reflection<AIRoadNodeComponent,BaseSceneComponent> , public IGraphNodeComponent
	{
	public:
		AIRoadNodeComponent(void);
		~AIRoadNodeComponent(void);
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
	typedef SPTR<AIRoadNodeComponent> AIRoadNodeComponentPtr;
	typedef WPTR<AIRoadNodeComponent> AIRoadNodeComponentWeakPtr;
}
#endif