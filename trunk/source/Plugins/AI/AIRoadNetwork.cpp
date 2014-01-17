#include "AIRoadNetwork.h"
#include "AISceneManager.h"
#include "AIRoadLaneSectionComponent.h"
#include "AIRoadLaneComponent.h"
#include "AIRoadIntersectionComponent.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Game/GameMessages.h"
#include "RoadIntersectionComponent.h"
#include "Sim/Interface/GASSIWaypointListComponent.h"
#include <algorithm>


namespace GASS
{
	AIRoadNetwork::AIRoadNetwork(void) 
	{
		
	}	

	AIRoadNetwork::~AIRoadNetwork(void)
	{

	}

	void AIRoadNetwork::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("AIRoadNetwork",new Creator<AIRoadNetwork, IComponent>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("AIRoadNetwork", OF_VISIBLE)));
	}
}
