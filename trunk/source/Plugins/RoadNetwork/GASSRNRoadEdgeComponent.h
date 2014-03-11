
#ifndef GASS_RN_ROAD_EDGE_COMPONENT
#define GASS_RN_ROAD_EDGE_COMPONENT

#include "Sim/GASS.h"
#include "Core/Utils/GASSFilePath.h"
#include "Sim/Interface/GASSIGraphEdgeComponent.h"
#include "Sim/Interface/GASSIGraphNodeComponent.h"
#include "Sim/Interface/GASSIWaypointListComponent.h"

namespace GASS
{
	class RNRoadEdgeComponent :  public Reflection<RNRoadEdgeComponent,BaseSceneComponent> , public IGraphEdgeComponent
	{
	public:
		RNRoadEdgeComponent(void);
		~RNRoadEdgeComponent(void);
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
		void OnTransformation(TransformationChangedEventPtr message);
		Float m_LaneWidth;
		GraphNodeComponentWeakPtr m_StartNode;
		GraphNodeComponentWeakPtr  m_EndNode;
	};
	typedef SPTR<RNRoadEdgeComponent> RNRoadEdgeComponentPtr;
}
#endif