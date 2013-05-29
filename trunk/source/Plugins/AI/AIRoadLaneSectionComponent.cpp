#include "AIRoadLaneSectionComponent.h"
#include "AISceneManager.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Game/GameMessages.h"
#include "RoadIntersectionComponent.h"


namespace GASS
{
	AIRoadLaneSectionComponent::AIRoadLaneSectionComponent(void) :
		m_Initialized(false),
			m_Distance(0)
	{
		
	}	

	AIRoadLaneSectionComponent::~AIRoadLaneSectionComponent(void)
	{

	}

	void AIRoadLaneSectionComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("AIRoadLaneSectionComponent",new Creator<AIRoadLaneSectionComponent, IComponent>);
		GetClassRTTI()->SetMetaData(ObjectMetaDataPtr(new ObjectMetaData("AIRoadLaneSectionComponent", OF_VISIBLE)));
		
		RegisterProperty<Float>("Distance", &AIRoadLaneSectionComponent::GetDistance, &AIRoadLaneSectionComponent::SetDistance,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
	}

	void AIRoadLaneSectionComponent::SetDistance(Float distance)
	{
		m_Distance = distance;
	}

	Float AIRoadLaneSectionComponent::GetDistance() const
	{
		return m_Distance;
	}

	void AIRoadLaneSectionComponent::OnInitialize()
	{
		m_Initialized = true;
	}
}
