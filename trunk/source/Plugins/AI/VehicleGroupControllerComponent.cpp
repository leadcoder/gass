#include "VehicleGroupControllerComponent.h"
#include "VehicleControllerComponent.h"
#include "VehicleBehaviorComponent.h"
#include "AISceneManager.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Game/GameMessages.h"



namespace GASS
{
	VehicleGroupControllerComponent::VehicleGroupControllerComponent(void) : m_Initialized(false), m_Update(false)
	{
		
	}	

	VehicleGroupControllerComponent::~VehicleGroupControllerComponent(void)
	{

	}

	void VehicleGroupControllerComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("VehicleGroupControllerComponent",new Creator<VehicleGroupControllerComponent, IComponent>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("VehicleGroupControllerComponent", OF_VISIBLE)));
		RegisterVectorProperty<std::string>("Templates", &GASS::VehicleGroupControllerComponent::GetTemplates, &GASS::VehicleGroupControllerComponent::SetTemplates);
		RegisterProperty<SceneObjectRef>("WaypointList", &VehicleGroupControllerComponent::GetWaypointList, &VehicleGroupControllerComponent::SetWaypointList);
		//SceneObjectEnumerationProxyPropertyMetaDataPtr(new SceneObjectEnumerationProxyPropertyMetaData("Waypoint list that we should follow",PF_VISIBLE,WaypointListEnumeration)));
	}

	void VehicleGroupControllerComponent::OnTransformation(TransformationNotifyMessagePtr message)
	{
		m_Pos = message->GetPosition();
		m_Rot = message->GetRotation();
	}

	void VehicleGroupControllerComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(VehicleGroupControllerComponent::OnTransformation,TransformationNotifyMessage,0));
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(VehicleGroupControllerComponent::OnScenarioEvent,ScenarioStateRequest,0));

		SceneManagerListenerPtr listener = shared_from_this();
		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<AISceneManager>()->Register(listener);

		m_Initialized = true;
	}

	void VehicleGroupControllerComponent::OnScenarioEvent(ScenarioStateRequestPtr message)
	{
		//
		if(message->GetState() == SS_PLAY)
		{
			OnPlay();
			m_Update = true;
		}
		else if(message->GetState() == SS_STOP)
		{
			//OnStop();
			m_Update = false;
		}

	}

	Vec3 VehicleGroupControllerComponent::GetOffset() const 
	{
		size_t num_vehicles  = GetSceneObject()->GetNumChildren()-1;
		Vec3 new_pos = m_Pos;
		new_pos.x += 5*num_vehicles;
		return new_pos;
	}

	void VehicleGroupControllerComponent::OnPlay() 
	{
		if(m_WaypointList.IsValid())
		{
			IComponentContainer::ComponentVector comps;
			m_WaypointList->GetComponentsByClass<VehicleBehaviorComponent>(comps);
			m_BehaviorWaypoints.clear();
			for(int i = 0 ;  i < comps.size(); i++)
			{
				VehicleBehaviorComponentPtr vbc = DYNAMIC_PTR_CAST<VehicleBehaviorComponent>(comps[i]);
				m_BehaviorWaypoints.push_back(vbc);
			}
			//SPTR<IWaypointListComponent> wpl_comp = m_WaypointList->GetFirstComponentByClass<IWaypointListComponent>();
			//std::vector<Vec3> points =  wpl_comp->GetWaypoints(false);
			VehicleControllerComponentPtr vcc = GetSceneObject()->GetFirstComponentByClass<VehicleControllerComponent>(true);
			//float target_radius = 10;
			//vcc->FollowPath(points,target_radius);
			m_Leader = vcc;
			vcc->Apply(m_BehaviorWaypoints.front());
		}
	}

	VehicleControllerComponentPtr  VehicleGroupControllerComponent::GetLeader() const
	{
		return VehicleControllerComponentPtr(m_Leader,boost::detail::sp_nothrow_tag());
	}

	void VehicleGroupControllerComponent::SceneManagerTick(double delta_time)
	{
		if(m_Update)
			OnUpdate(delta_time);
	}

	void VehicleGroupControllerComponent::OnUpdate(double delta_time)
	{
		VehicleControllerComponentPtr leader = GetLeader();
		if(leader)
		{
			//Vec3 pos = leader->GetCurrentLocation();
			if(leader->GetTargetReached())
			{
				//remove waypoints
				if(m_BehaviorWaypoints.size() > 0)
				{
					//apply behavior!
					m_BehaviorWaypoints.erase(m_BehaviorWaypoints.begin());
					leader->Apply(m_BehaviorWaypoints.front());
				}
				//Check waypoint behavior
			}
			leader->OnUpdate(delta_time);
		}
	}
	
}
