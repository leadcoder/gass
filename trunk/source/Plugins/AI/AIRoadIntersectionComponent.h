
#ifndef AI_ROAD_INTERSECTION_COMPONENT
#define AI_ROAD_INTERSECTION_COMPONENT

#include "Sim/GASS.h"
#include "Core/Utils/GASSFilePath.h"
#include "Plugins/Game/PlatformType.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/AI/AIMessages.h"
#include "Plugins/AI/AIRoadComponent.h"

#include <set>

namespace GASS
{
	class AIRoadIntersectionComponent :  public Reflection<AIRoadIntersectionComponent,BaseSceneComponent> 
	{
	public:
		AIRoadIntersectionComponent(void);
		~AIRoadIntersectionComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		void AddRoad(AIRoadComponentPtr road);
		void RemoveRoad(AIRoadComponentPtr road);
		void UpdateConnectionLines();
		void AutoLineConnection();
	private:
		ADD_ATTRIBUTE(SceneObjectRef ,ConnectionDebugObject);
		
		bool m_AllowLeftTurn;
		bool m_DisableTrafficLight;
		bool m_Initialized;
		std::vector<AIRoadComponentPtr> m_Connections;
		std::vector<AIRoadComponentPtr> m_Links;
	};
	typedef SPTR<AIRoadIntersectionComponent> AIRoadIntersectionComponentPtr;
}
#endif