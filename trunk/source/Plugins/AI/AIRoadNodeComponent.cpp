#include <limits>
#include "AIRoadNodeComponent.h"
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
	AIRoadNodeComponent::AIRoadNodeComponent(void) 
	{

	}	

	AIRoadNodeComponent::~AIRoadNodeComponent(void)
	{

	}

	void AIRoadNodeComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("AIRoadNodeComponent",new Creator<AIRoadNodeComponent, IComponent>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("AIRoadNodeComponent", OF_VISIBLE)));

		//RegisterProperty<bool>("Build", &AIRoadNodeComponent::GetBuild, &AIRoadNodeComponent::SetBuild,
		//	BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
	}

	void AIRoadNodeComponent::OnInitialize()
	{
		
	}
}
