
#ifndef AI_ROAD_INTER_COMPONENT
#define AI_ROAD_INTER_COMPONENT

#include "Core/Utils/GASSFilePath.h"
#include "Sim/GASS.h"
#include "Plugins/Game/PlatformType.h"
#include "Plugins/Base/CoreMessages.h"
#include <set>

namespace GASS
{
	class RoadSegmentComponent;
	typedef SPTR<RoadSegmentComponent>  RoadSegmentComponentPtr;
 	class RoadIntersectionComponent :  public Reflection<RoadIntersectionComponent,BaseSceneComponent> 
	{
	public:
		RoadIntersectionComponent(void);
		~RoadIntersectionComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		void AddRoad(RoadSegmentComponentPtr road);
		void RemoveRoad(RoadSegmentComponentPtr road);
		RoadSegmentComponentPtr GetRandomRoad(RoadSegmentComponentPtr road);
	private:
		bool m_Initialized;
		std::vector<RoadSegmentComponentPtr> m_Connections;
		//std::vector<SceneObjectRef> m_Connections;
	};
	typedef SPTR<RoadIntersectionComponent> RoadIntersectionComponentPtr;
}
#endif