
#ifndef AI_ROAD_TRAFFIC_LIGHT_COMPONENT
#define AI_ROAD_TRAFFIC_LIGHT_COMPONENT

#include "Sim/GASS.h"
#include "Plugins/Game/PlatformType.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Game/GameMessages.h"
#include "Sim/Interface/GASSIWaypointListComponent.h"


namespace GASS
{
	class AIRoadLaneComponent;
	typedef SPTR<AIRoadLaneComponent> AIRoadLaneComponentPtr;
	class AIRoadTrafficLightComponent :  public Reflection<AIRoadTrafficLightComponent,BaseSceneComponent> 
	{
	public:
		AIRoadTrafficLightComponent(void);
		~AIRoadTrafficLightComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
	private:
		void SetDistance(Float distance);
		Float GetDistance() const;
	
		void OnWaypointsChanged(UpdateWaypointListMessagePtr message);
		void UpdateGeom();
		Float m_Distance;
		bool m_Initialized;
	};

	typedef SPTR<AIRoadTrafficLightComponent> AIRoadTrafficLightComponentPtr;
}
#endif