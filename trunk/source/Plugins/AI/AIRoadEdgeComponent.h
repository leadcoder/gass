
#ifndef AI_ROAD_EDGE_COMPONENT
#define AI_ROAD_EDGE_COMPONENT

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
#include "Sim/Interface/GASSIGraphEdgeComponent.h"
#include "Sim/Interface/GASSIGraphNodeComponent.h"

namespace GASS
{
	class AIRoadEdgeComponent :  public Reflection<AIRoadEdgeComponent,BaseSceneComponent> , public IGraphEdgeComponent
	{
	public:
		AIRoadEdgeComponent(void);
		~AIRoadEdgeComponent(void);
		void OnInitialize();
		void OnDelete();
		static void RegisterReflection();
		void SetStartNode(GraphNodeComponentPtr node) {m_StartNode = node;}
		void SetEndNode(GraphNodeComponentPtr node) {m_EndNode = node;}
		GraphNodeComponentPtr GetStartNode() const {return GraphNodeComponentPtr(m_StartNode,NO_THROW);}
		GraphNodeComponentPtr GetEndNode() const {return GraphNodeComponentPtr(m_EndNode,NO_THROW);}
	private:
		GraphNodeComponentWeakPtr m_StartNode;
		GraphNodeComponentWeakPtr  m_EndNode;
	};
	typedef SPTR<AIRoadEdgeComponent> AIRoadEdgeComponentPtr;
}
#endif