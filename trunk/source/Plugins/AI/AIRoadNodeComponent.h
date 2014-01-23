
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
#include "Sim/Interface/GASSIGraphComponent.h"

namespace GASS
{
	class AIRoadLaneSectionComponent;
	class RoadNode;
	class RoadEdge;

	typedef SPTR<AIRoadLaneSectionComponent> AIRoadLaneSectionComponentPtr;

	class AIRoadNodeComponent :  public Reflection<AIRoadNodeComponent,BaseSceneComponent> , IGraphComponent
	{
	public:
		AIRoadNodeComponent(void);
		~AIRoadNodeComponent(void);
		void OnInitialize();
		std::string GetNodeTemplate() const {return "AINODE";}
		static void RegisterReflection();
	private:
		std::vector<AIRoadNodeComponentPtr> m_Edges;
	};
	typedef SPTR<AIRoadNodeComponent> AIRoadNodeComponentPtr;
}
#endif