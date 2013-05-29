
#ifndef AI_ROAD_LANE_COMPONENT
#define AI_ROAD_LANE_COMPONENT

#include "Sim/GASS.h"
#include "Core/Utils/GASSFilePath.h"
#include "Plugins/Game/PlatformType.h"
#include "Plugins/Base/CoreMessages.h"
#include "tbb/spin_mutex.h"
#include <set>
#include "tbb/atomic.h"

namespace GASS
{
	class AIRoadLaneComponent :  public Reflection<AIRoadLaneComponent,BaseSceneComponent> 
	{
	public:
		AIRoadLaneComponent(void);
		~AIRoadLaneComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		void SetWidth(Float dist);
		Float GetWidth() const;
		void UpdateLane();
	private:
		std::vector<Vec3> GenerateOffset(std::vector<Vec3> wps, Float offset);
		ADD_ATTRIBUTE(SceneObjectID,NextLane);
		ADD_ATTRIBUTE(SceneObjectID,PrevLane);
		ADD_ATTRIBUTE(SceneObjectRef ,WaypointsObject);
		void OnWaypointsChanged(UpdateWaypointListMessagePtr message);
		Float m_Width;
		bool m_Initialized;
	};

	typedef SPTR<AIRoadLaneComponent> AIRoadLaneComponentPtr;
}
#endif