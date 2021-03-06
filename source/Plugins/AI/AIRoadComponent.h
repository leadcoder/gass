
#ifndef AI_ROAD_COMPONENT
#define AI_ROAD_COMPONENT

#include "Sim/GASS.h"
#include "Core/Utils/GASSFilePath.h"
#include "Plugins/Game/PlatformType.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/AI/AIRoadLaneComponent.h"
#include <tbb/spin_mutex.h>
#include <set>
#include <tbb/atomic.h>

namespace GASS
{
	class AIRoadLaneSectionComponent;

	/*struct NodeLink
	{
		Vec3 Pos;
		AIRoadComponent *Road;
	};*/
	
	typedef SPTR<AIRoadLaneSectionComponent> AIRoadLaneSectionComponentPtr;
	
	class AIRoadComponent :  public Reflection<AIRoadComponent,BaseSceneComponent> 
	{
	public:
		AIRoadComponent(void);
		~AIRoadComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		void SetStartNode(SceneObjectRef node);
		SceneObjectRef GetStartNode() const;
		void SetEndNode(SceneObjectRef node);
		SceneObjectRef GetEndNode() const;
		void UpdateMesh();
		ADD_PROPERTY(SceneObjectRef ,WaypointsObject);
		ADD_PROPERTY(SceneObjectRef ,LaneSectionsObject);
		ADD_PROPERTY(SceneObjectRef ,LaneDebugObject);
		std::vector<SceneObjectPtr>  GetConnectionSelection() const;
		Vec3 GetStartPoint() const;
		Vec3 GetEndPoint() const;
		bool StartIn(SceneObjectPtr obj) const;
		std::vector<AIRoadLaneComponentPtr> GetStartLanes() const;
		std::vector<AIRoadLaneComponentPtr> GetEndLanes() const;
		std::vector<AIRoadLaneComponentPtr> GetStartLanes(LaneDirection lane_dir) const;
		std::vector<AIRoadLaneComponentPtr> GetEndLanes(LaneDirection lane_dir) const;

		std::vector<AIRoadLaneComponentPtr> GetIncommingLanes(SceneObjectPtr connection) const;
		std::vector<AIRoadLaneComponentPtr> GetOutgoingLanes(SceneObjectPtr connection) const;
		std::vector<AIRoadLaneSectionComponentPtr> GetLaneSections() const {return m_LaneSections;}
	private:
		void UpdateLanes();
		void AutoConnectToRoads();
		void AutoConnectToIntersection();
		void OnTransformationChanged(TransformationChangedEventPtr message);
		void OnWaypointsChanged(UpdateWaypointListMessagePtr message);
		SceneObjectRef m_StartNode;
		SceneObjectRef m_EndNode;
		std::vector<AIRoadLaneSectionComponentPtr> m_LaneSections;
		bool m_Initialized;

		//NodeLink* m_EndLink;
		//NodeLink* m_StartLink;
		
	};

	typedef SPTR<AIRoadComponent> AIRoadComponentPtr;
}
#endif