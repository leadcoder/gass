#include "AIRoadLaneComponent.h"
#include "AIRoadComponent.h"
#include "AISceneManager.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Game/GameMessages.h"
#include "RoadIntersectionComponent.h"
#include "Sim/Interface/GASSIWaypointListComponent.h"


namespace GASS
{
	AIRoadLaneComponent::AIRoadLaneComponent(void) :
		m_Initialized(false),
			m_Width(4)
	{
		
	}	

	AIRoadLaneComponent::~AIRoadLaneComponent(void)
	{

	}

	void AIRoadLaneComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("AIRoadLaneComponent",new Creator<AIRoadLaneComponent, IComponent>);
		RegisterProperty<Float>("Width", &AIRoadLaneComponent::GetWidth, &AIRoadLaneComponent::SetWidth);
		RegisterProperty<SceneObjectID>("NextLane", &AIRoadLaneComponent::GetNextLane, &AIRoadLaneComponent::SetNextLane);
		RegisterProperty<SceneObjectID>("PrevLane", &AIRoadLaneComponent::GetPrevLane, &AIRoadLaneComponent::SetPrevLane);
	}

	void AIRoadLaneComponent::SetWidth(Float width)
	{
		m_Width = width;
	}

	Float AIRoadLaneComponent::GetWidth() const
	{
		return m_Width;
	}

	void AIRoadLaneComponent::OnInitialize()
	{
		AIRoadComponentPtr road = GetSceneObject()->GetFirstParentComponentByClass<AIRoadComponent>();
		SceneObjectRef wp_object = road->GetWaypointsObject();
		wp_object->RegisterForMessage(REG_TMESS(AIRoadLaneComponent::OnWaypointsChanged,UpdateWaypointListMessage,0));
		m_Initialized = true;
	}
	void AIRoadLaneComponent::OnWaypointsChanged(UpdateWaypointListMessagePtr message)
	{
		//Update all lines
		
		AIRoadComponentPtr road = GetSceneObject()->GetFirstParentComponentByClass<AIRoadComponent>();
		SceneObjectRef wp_object = road->GetWaypointsObject();
		std::vector<Vec3> wps = wp_object->GetFirstComponentByClass<IWaypointListComponent>()->GetWaypoints();
		
	}


}
