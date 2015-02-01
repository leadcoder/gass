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
			m_Distance(15),
			m_State(LS_GREEN),
			m_LaneObject(new LaneObject())
	{
		m_LaneObject->m_Distance = m_Distance;
		m_LaneObject->m_Speed = 0;
		m_LaneObject->m_DistanceToPath = 0;
		m_LaneObject->m_Static = true;
		
	}	

	AIRoadTrafficLightComponent::~AIRoadTrafficLightComponent(void)
	{

	}

	void AIRoadTrafficLightComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("AIRoadTrafficLightComponent",new Creator<AIRoadTrafficLightComponent, IComponent>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("AIRoadTrafficLightComponent", OF_VISIBLE)));
		
		RegisterProperty<Float>("Distance", &AIRoadTrafficLightComponent::GetDistance, &AIRoadTrafficLightComponent::SetDistance,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("",PF_VISIBLE | PF_EDITABLE)));

		//RegisterProperty<SceneObjectRef>("RedLight", &AIRoadTrafficLightComponent::GetRedLight, &AIRoadTrafficLightComponent::SetRedLight);
		//RegisterProperty<SceneObjectRef>("YellowLight", &AIRoadTrafficLightComponent::GetYellowLight, &AIRoadTrafficLightComponent::SetYellowLight);
		//RegisterProperty<SceneObjectRef>("GreenLight", &AIRoadTrafficLightComponent::GetGreenLight, &AIRoadTrafficLightComponent::SetGreenLight);
	}

	void AIRoadTrafficLightComponent::SetDistance(Float distance)
	{
		m_Distance = distance;

		m_LaneObject->m_Distance = m_Distance;
		m_LaneObject->m_Speed = 0;
		m_LaneObject->m_DistanceToPath = 0;

	}

	Float AIRoadTrafficLightComponent::GetDistance() const
	{
		return m_Distance;
	}

	void AIRoadTrafficLightComponent::OnInitialize()
	{
		
		GetSceneObject()->RegisterForMessage(REG_TMESS(AIRoadTrafficLightComponent::OnTrafficLightState,TrafficLightStateMessage,0));
		
		//SceneObjectRef wp_object = road->GetWaypointsObject();
		//wp_object->RegisterForMessage(REG_TMESS(AIRoadTrafficLightComponent::OnWaypointsChanged,UpdateWaypointListMessage,0));
		m_Initialized = true;
	}

	/*void AIRoadTrafficLightComponent::OnWaypointsChanged(UpdateWaypointListMessagePtr message)
	{
		UpdateGeom();
	}*/

	void AIRoadTrafficLightComponent::OnTrafficLightState(TrafficLightStateMessagePtr message)
	{
		if(m_State != message->GetState())
		{
			//GetSceneObject()->PostMessage(GASS::MessagePtr(new GASS::TrafficLightStateMessage(m_State,-1,10)));

			AIRoadLaneComponentPtr lane = GetSceneObject()->GetFirstParentComponentByClass<AIRoadLaneComponent>();
			m_State = message->GetState();
			if(m_State == LS_GREEN)
			{
				lane->UnregisterLaneObject(m_LaneObject);
			}
			else
				lane->RegisterLaneObject(m_LaneObject);

		}
		//relay message to children
		IComponentContainer::ComponentContainerIterator children = GetSceneObject()->GetChildren();
		while(children.hasMoreElements())
		{
			SceneObjectPtr child_obj =  STATIC_PTR_CAST<SceneObject>(children.getNext());
			child_obj->SendImmediate(message);
		}

		//UpdateGeom();
	}

	void AIRoadTrafficLightComponent::UpdateGeom()
	{
		//AIRoadComponentPtr road = GetSceneObject()->GetFirstParentComponentByClass<AIRoadComponent>();
		AIRoadLaneComponentPtr lane = GetSceneObject()->GetFirstParentComponentByClass<AIRoadLaneComponent>();
		if(!lane)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to find AIRoadLaneComponent", "AIRoadTrafficLightComponent::UpdateGeom()");

		std::vector<Vec3> *lane_wps = lane->GetWaypointsPtr();
		if(lane_wps->size() >0)
		{
			int index;
			
			Vec4 yellow_color(0.3,0.3,0,1);
			Vec4 green_color(0,0.3,0,1);
			Vec4 red_color(0.3,0,0,1);
			std::vector<Vec3> offset_wps = Math::GenerateOffset(*lane_wps, 3.9);
			Vec3 pos = Math::GetPointOnPath(m_Distance, offset_wps, false, index);
			
			if(m_State == LS_GREEN)
			{
				green_color.Set(0,1,0,1);
			}

			else if (m_State == LS_YELLOW)
			{
				yellow_color.Set(1,1,0,1);
			}
			else if (m_State == LS_RED)
			{
				red_color.Set(1,0,0,1);
			}

			if(m_GreenLight.IsValid())
			{
				/*MessagePtr mat_mess(new MaterialMessage(green_color,
				Vec3(green_color.x,green_color.y,green_color.z),
				Vec3(0,0,0),
					Vec3(green_color.x,green_color.y,green_color.z),
					0,
					true));*/
				MessagePtr mat_mess(new ReplaceMaterialRequest("GreenLight"));
				m_GreenLight->PostMessage(mat_mess);
			}


			if(m_YellowLight.IsValid())
			{
				/*MessagePtr mat_mess(new MaterialMessage(yellow_color,
				Vec3(yellow_color.x,yellow_color.y,yellow_color.z),
				Vec3(0,0,0),
					Vec3(yellow_color.x,yellow_color.y,yellow_color.z),
					0,
					true));*/
				MessagePtr mat_mess(new ReplaceMaterialRequest("YellowLight"));
				m_YellowLight->PostMessage(mat_mess);
			}

			if(m_RedLight.IsValid())
			{
				/*MessagePtr mat_mess(new MaterialMessage(red_color,
				Vec3(red_color.x,red_color.y,red_color.z),
				Vec3(0,0,0),
					Vec3(red_color.x,red_color.y,red_color.z),
					0,
					true));*/
				MessagePtr mat_mess(new ReplaceMaterialRequest("RedLight"));
				m_RedLight->PostMessage(mat_mess);
			}
			
			GetSceneObject()->PostMessage(MessagePtr(new WorldPositionRequest(pos)));
		}
	}
}
