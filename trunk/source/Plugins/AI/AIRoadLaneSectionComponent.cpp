#include "AIRoadLaneSectionComponent.h"
#include "AISceneManager.h"
#include "AIRoadComponent.h"
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
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("AIRoadLaneSectionComponent", OF_VISIBLE)));
		
		RegisterProperty<Float>("Distance", &AIRoadLaneSectionComponent::GetDistance, &AIRoadLaneSectionComponent::SetDistance,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));
	}

	void AIRoadLaneSectionComponent::SetDistance(Float distance)
	{
		m_Distance = distance;
		if(m_Initialized)
		{
		}
	}

	Float AIRoadLaneSectionComponent::GetDistance() const
	{
		return m_Distance;
	}

	void AIRoadLaneSectionComponent::OnInitialize()
	{
		AIRoadComponentPtr road = GetSceneObject()->GetFirstParentComponentByClass<AIRoadComponent>();
		SceneObjectRef wp_object = road->GetWaypointsObject();
		wp_object->RegisterForMessage(REG_TMESS(AIRoadLaneSectionComponent::OnWaypointsChanged,UpdateWaypointListMessage,0));
		m_Initialized = true;
	}

	void AIRoadLaneSectionComponent::OnWaypointsChanged(UpdateWaypointListMessagePtr message)
	{
		UpdateGeom();
	}

	void AIRoadLaneSectionComponent::UpdateGeom()
	{
		AIRoadComponentPtr road = GetSceneObject()->GetFirstParentComponentByClass<AIRoadComponent>();
		if(!road)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to find AIRoadComponent", "AIRoadLaneSectionComponent::UpdateGeom()");

		SceneObjectRef wp_object = road->GetWaypointsObject();
		std::vector<Vec3> road_wps = wp_object->GetFirstComponentByClass<IWaypointListComponent>()->GetWaypoints();
		int index;
		if(road_wps.size() >0)
		{
			Vec3 pos = Math::GetPointOnPath(m_Distance, road_wps, false, index);
			GetSceneObject()->PostMessage(MessagePtr(new WorldPositionMessage(pos)));
		}
	}

	std::vector<AIRoadLaneComponentPtr> AIRoadLaneSectionComponent::GetLanesByID(int id)
	{
		std::vector<AIRoadLaneComponentPtr> lanes;
		
		IComponentContainer::ComponentVector comps;
		GetSceneObject()->GetComponentsByClass<AIRoadLaneComponent>(comps);
		for(size_t i =  0; i < comps.size(); i++)
		{
			AIRoadLaneComponentPtr  lane = DYNAMIC_PTR_CAST<AIRoadLaneComponent>(comps[i]);
			if(lane->GetLaneID() == id)
			{
				lanes.push_back(lane);
			}
		}
		return lanes;
	}

	std::vector<AIRoadLaneComponentPtr> AIRoadLaneSectionComponent::GetLanes()
	{
		std::vector<AIRoadLaneComponentPtr> lanes;
		
		IComponentContainer::ComponentVector comps;
		GetSceneObject()->GetComponentsByClass<AIRoadLaneComponent>(comps);
		for(size_t i =  0; i < comps.size(); i++)
		{
			AIRoadLaneComponentPtr  lane = DYNAMIC_PTR_CAST<AIRoadLaneComponent>(comps[i]);
			lanes.push_back(lane);
		}
		return lanes;
	}
}
