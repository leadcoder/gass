#include "RoadSegmentComponent.h"
#include "AISceneManager.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Game/GameMessages.h"



namespace GASS
{
	RoadSegmentComponent::RoadSegmentComponent(void) :
		m_Initialized(false)
	{
		
	}	

	RoadSegmentComponent::~RoadSegmentComponent(void)
	{

	}

	void RoadSegmentComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("RoadSegmentComponent",new Creator<RoadSegmentComponent, IComponent>);
		//REG_ATTRIBUTE(Vec2,RandomVelocity,RoadSegmentComponent)
	}

	void RoadSegmentComponent::OnInitialize()
	{
		
		m_Initialized = true;
	}
	
}
