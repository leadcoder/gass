#include "AIRoadSignalComponent.h"
#include "AISceneManager.h"
#include "AIRoadComponent.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Game/GameMessages.h"
#include "RoadIntersectionComponent.h"


namespace GASS
{
	AIRoadSignalComponent::AIRoadSignalComponent(void) : m_Initialized(false),
			m_State(LS_GREEN)
	{
		
	}	

	AIRoadSignalComponent::~AIRoadSignalComponent(void)
	{

	}

	void AIRoadSignalComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("AIRoadSignalComponent",new Creator<AIRoadSignalComponent, IComponent>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("AIRoadSignalComponent", OF_VISIBLE)));
		
		RegisterProperty<SceneObjectRef>("RedLight", &AIRoadSignalComponent::GetRedLight, &AIRoadSignalComponent::SetRedLight);
		RegisterProperty<SceneObjectRef>("YellowLight", &AIRoadSignalComponent::GetYellowLight, &AIRoadSignalComponent::SetYellowLight);
		RegisterProperty<SceneObjectRef>("GreenLight", &AIRoadSignalComponent::GetGreenLight, &AIRoadSignalComponent::SetGreenLight);
	}


	void AIRoadSignalComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(AIRoadSignalComponent::OnTrafficLightState,TrafficLightStateMessage,0));
		m_Initialized = true;
	}

	void AIRoadSignalComponent::OnTrafficLightState(TrafficLightStateMessagePtr message)
	{
		m_State = message->GetState();
		UpdateGeom();
	}

	void AIRoadSignalComponent::UpdateGeom()
	{
		Vec4 yellow_color(0.18,0.15,0,1);
		Vec4 green_color(0,0.15,0,1);
		Vec4 red_color(0.15,0,0,1);
		if(m_State == LS_GREEN)
		{
			green_color.Set(0,1,0,1);
		}
		else if (m_State == LS_YELLOW)
		{
			yellow_color.Set(1,0.8,0,1);
		}
		
		else if (m_State == LS_RED)
		{
			red_color.Set(1,0,0,1);
		}

		if(m_GreenLight.IsValid())
		{
			/*MessagePtr mat_mess(new MaterialMessage(green_color,
				Vec3(0,0,0),
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
				Vec3(0,0,0),
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
				Vec3(0,0,0),
				Vec3(0,0,0),
				Vec3(red_color.x,red_color.y,red_color.z),
				0,
				true));*/
			MessagePtr mat_mess(new ReplaceMaterialRequest("RedLight"));
			m_RedLight->PostMessage(mat_mess);
		}
	}
}
