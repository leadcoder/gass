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
		//RegisterVectorProperty<SceneObjectRef>("RoadSegments", &RoadIntersectionComponent::GetRoadSegments, &RoadIntersectionComponent::SetRoadSegments);
	}

	void RoadIntersectionComponent::OnInitialize()
	{
		//GetSceneObject()->RegisterForMessage(REG_TMESS(RoadIntersectionComponent::OnTriggerEnter,TriggerEnterMessage,0));
		
		m_Initialized = true;
	}

	/*void RoadIntersectionComponent::SetRoadSegments(const std::vector<SceneObjectRef> &connections)
	{
	//	m_Connections = connections;
		UpdateRoads();
	}*/
}
