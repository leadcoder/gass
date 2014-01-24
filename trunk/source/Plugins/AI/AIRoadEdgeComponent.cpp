#include <limits>
#include "AIRoadEdgeComponent.h"
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
	AIRoadEdgeComponent::AIRoadEdgeComponent(void) 
	{

	}	

	AIRoadEdgeComponent::~AIRoadEdgeComponent(void)
	{

	}

	void AIRoadEdgeComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("AIRoadEdgeComponent",new Creator<AIRoadEdgeComponent, IComponent>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("AIRoadEdgeComponent", OF_VISIBLE)));

		//RegisterProperty<bool>("Build", &AIRoadEdgeComponent::GetBuild, &AIRoadEdgeComponent::SetBuild,
		//	BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
	}

	void AIRoadEdgeComponent::OnInitialize()
	{

	}
	
}
