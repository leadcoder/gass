
#ifndef AI_ROAD_TRAFFIC_LIGHT_COMPONENT
#define AI_ROAD_TRAFFIC_LIGHT_COMPONENT

#include "Sim/GASS.h"
#include "Plugins/AI/AIMessages.h"
#include "Plugins/Game/PlatformType.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Game/GameMessages.h"
#include "Sim/Interface/GASSIWaypointListComponent.h"


namespace GASS
{
	class LaneObject;
	class AIRoadLaneComponent;
	typedef SPTR<AIRoadLaneComponent> AIRoadLaneComponentPtr;
	class AIRoadSignalComponent :  public Reflection<AIRoadSignalComponent,BaseSceneComponent> 
	{
	public:
		AIRoadSignalComponent(void);
		~AIRoadSignalComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
	private:
		ADD_PROPERTY(SceneObjectRef,RedLight);
		ADD_PROPERTY(SceneObjectRef,GreenLight);
		ADD_PROPERTY(SceneObjectRef,YellowLight);
		void OnTrafficLightState(TrafficLightStateMessagePtr message);
		void UpdateGeom();
		Float m_Distance;
		bool m_Initialized;
		LightState m_State;
	};
	typedef SPTR<AIRoadSignalComponent> AIRoadSignalComponentPtr;
}
#endif