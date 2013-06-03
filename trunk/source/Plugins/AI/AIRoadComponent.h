
#ifndef AI_ROAD_COMPONENT
#define AI_ROAD_COMPONENT

#include "Sim/GASS.h"
#include "Core/Utils/GASSFilePath.h"
#include "Plugins/Game/PlatformType.h"
#include "Plugins/Base/CoreMessages.h"
#include "tbb/spin_mutex.h"
#include <set>
#include "tbb/atomic.h"

namespace GASS
{
	class AIRoadLaneSectionComponent;
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
		ADD_ATTRIBUTE(SceneObjectRef ,WaypointsObject);
		ADD_ATTRIBUTE(SceneObjectRef ,LaneSectionsObject);
		ADD_ATTRIBUTE(SceneObjectRef ,LaneDebugObject);
		
	private:
		void OnTransformationChanged(TransformationNotifyMessagePtr message);
		void OnWaypointsChanged(UpdateWaypointListMessagePtr message);
		SceneObjectRef m_StartNode;
		SceneObjectRef m_EndNode;
		std::vector<AIRoadLaneSectionComponentPtr> m_LaneSections;
		bool m_Initialized;
	};

	typedef SPTR<AIRoadComponent> AIRoadComponentPtr;
}
#endif