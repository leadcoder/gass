#include "AIRoadTrafficLightComponent.h"
#include "AISceneManager.h"
#include "AIRoadComponent.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Game/GameMessages.h"
#include "RoadIntersectionComponent.h"


namespace GASS
{
	AIRoadTrafficLightComponent::AIRoadTrafficLightComponent(void) :
		m_Initialized(false),
			m_Distance(0)
	{
		
	}	

	AIRoadTrafficLightComponent::~AIRoadTrafficLightComponent(void)
	{

	}

	void AIRoadTrafficLightComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("AIRoadTrafficLightComponent",new Creator<AIRoadTrafficLightComponent, IComponent>);
		GetClassRTTI()->SetMetaData(ObjectMetaDataPtr(new ObjectMetaData("AIRoadTrafficLightComponent", OF_VISIBLE)));
		
		RegisterProperty<Float>("Distance", &AIRoadTrafficLightComponent::GetDistance, &AIRoadTrafficLightComponent::SetDistance,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
	}

	void AIRoadTrafficLightComponent::SetDistance(Float distance)
	{
		m_Distance = distance;
		
	}

	Float AIRoadTrafficLightComponent::GetDistance() const
	{
		return m_Distance;
	}

	void AIRoadTrafficLightComponent::OnInitialize()
	{
		AIRoadLaneComponentPtr lane = GetSceneObject()->GetFirstParentComponentByClass<AIRoadLaneComponent>();
		//SceneObjectRef wp_object = road->GetWaypointsObject();
		//wp_object->RegisterForMessage(REG_TMESS(AIRoadTrafficLightComponent::OnWaypointsChanged,UpdateWaypointListMessage,0));
		m_Initialized = true;
	}

	void AIRoadTrafficLightComponent::OnWaypointsChanged(UpdateWaypointListMessagePtr message)
	{
		UpdateGeom();
	}

	void AIRoadTrafficLightComponent::UpdateGeom()
	{
		AIRoadComponentPtr road = GetSceneObject()->GetFirstParentComponentByClass<AIRoadComponent>();
		if(!road)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to find AIRoadComponent", "AIRoadTrafficLightComponent::UpdateGeom()");

		SceneObjectRef wp_object = road->GetWaypointsObject();
		std::vector<Vec3> road_wps = wp_object->GetFirstComponentByClass<IWaypointListComponent>()->GetWaypoints();
		int index;
		if(road_wps.size() >0)
		{
			Vec3 pos = Math::GetPointOnPath(m_Distance, road_wps, false, index);
			GetSceneObject()->PostMessage(MessagePtr(new WorldPositionMessage(pos)));
		}
	}

	
}
