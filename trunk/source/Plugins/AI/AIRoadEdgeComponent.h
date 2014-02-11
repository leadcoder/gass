
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
#include "Sim/Interface/GASSIWaypointListComponent.h"


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
		void SetLaneWidth(Float value) { m_LaneWidth = value;}
		Float GetLaneWidth() const { return m_LaneWidth;}
		WaypointListComponentPtr GetWaypointList() const;
		SceneObjectPtr GetWaypointListObject() const;
		void SetWaypoints(const std::vector<Vec3> &wps);
		std::vector<Vec3> GetWaypoints() const;
	private:
		Float m_LaneWidth;
		GraphNodeComponentWeakPtr m_StartNode;
		GraphNodeComponentWeakPtr  m_EndNode;
	};
	typedef SPTR<AIRoadEdgeComponent> AIRoadEdgeComponentPtr;
}
#endif