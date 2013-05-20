#include "RoadIntersectionComponent.h"
#include "AISceneManager.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Game/GameMessages.h"



namespace GASS
{
	RoadIntersectionComponent::RoadIntersectionComponent(void) :
		m_Initialized(false)
	{
		
	}	

	RoadIntersectionComponent::~RoadIntersectionComponent(void)
	{

	}

	void RoadIntersectionComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("RoadIntersectionComponent",new Creator<RoadIntersectionComponent, IComponent>);
		//REG_ATTRIBUTE(Vec2,RandomVelocity,RoadIntersectionComponent)
	}

	void RoadIntersectionComponent::OnInitialize()
	{
		//GetSceneObject()->RegisterForMessage(REG_TMESS(RoadIntersectionComponent::OnTriggerEnter,TriggerEnterMessage,0));
		m_Initialized = true;
	}
	
}
