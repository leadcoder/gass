
#ifndef AI_ROAD_SEG_COMPONENT
#define AI_ROAD_SEG_COMPONENT

#include "Sim/GASS.h"
#include "Plugins/Game/PlatformType.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Game/GameMessages.h"
#include "Sim/Interface/GASSIWaypointListComponent.h"


namespace GASS
{
	class AIRoadLaneSectionComponent :  public Reflection<AIRoadLaneSectionComponent,BaseSceneComponent> 
	{
	public:
		AIRoadLaneSectionComponent(void);
		~AIRoadLaneSectionComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		void SetDistance(Float dist);
		Float GetDistance() const;
	private:
		void OnWaypointsChanged(UpdateWaypointListMessagePtr message);
		void UpdateGeom();
		Float m_Distance;
		bool m_Initialized;
	};

	typedef SPTR<AIRoadLaneSectionComponent> AIRoadLaneSectionComponentPtr;
}
#endif