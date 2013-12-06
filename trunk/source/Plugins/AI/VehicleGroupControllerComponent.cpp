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
		BaseSceneComponent::InitializeSceneObjectRef();
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
		new_pos.z += 10*num_vehicles;
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
			VehicleControllerComponentPtr vcc = GetSceneObject()->GetFirstComponentByClass<VehicleControllerComponent>(true);

			//Get slaves!
			m_Slaves.clear();
			IComponentContainerTemplate::ComponentVector components;
			GetSceneObject()->GetComponentsByClass<VehicleControllerComponent>(components);
			for(int i = 0;  i< components.size(); i++)
			{
			
				VehicleControllerComponentPtr comp = DYNAMIC_PTR_CAST<VehicleControllerComponent>(components[i]);
				comp->SetGroupID(i);
				if(i == 0)
				{
					if(m_BehaviorWaypoints.size() > 0)
						comp->SetBehaviorList(m_BehaviorWaypoints);
					m_Leader = comp;
				}
				else
				{
					comp->SetLeader(VehicleControllerComponentPtr(m_Leader));
					comp->SetBehaviorList(m_BehaviorWaypoints);
					m_Slaves.push_back(comp);
				}
			}
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
			/*if(leader->GetTargetReached())
			{
				//remove waypoints
				if(m_BehaviorWaypoints.size() > 0)
				{
					//apply behavior!
					m_BehaviorWaypoints.erase(m_BehaviorWaypoints.begin());
					if(m_BehaviorWaypoints.size() > 0)
						leader->Apply(m_BehaviorWaypoints.front());
				}
				//Check waypoint behavior
			}*/
			leader->OnUpdate(delta_time);
			//Float target_speed = leader->GetTargetSpeed();

			for(size_t i = 0;  i < m_Slaves.size(); i++)
			{
				//"3 second rule" and add min distance 10 m"
				//Float target_dist = 10 + (i+1)*target_speed*3;
				VehicleControllerComponentPtr slave(m_Slaves[i],NO_THROW);
				if(slave)
				{
					//target_dist = leader->GetCurrentDistance() - target_dist;
					//slave->SetTargetDistance(target_dist);
					//slave->SetOffset(5); 
					slave->OnUpdate(delta_time);
				}
			}
		}

			/*for(size_t i = 0;  i < m_Slaves.size(); i++)
			{
				VehicleControllerComponentPtr slave(m_Slaves[i],NO_THROW);
				if(slave)
				{
					SceneObjectPtr vehicle = slave->GetVehicle();
					if(vehicle)
					{
						Vec3 vehicle_pos = vehicle->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
						Float my_dist = 0;
						if(leader->GetPathDistance(vehicle_pos,my_dist))
						{
							target_dist = (i+1)*target_speed*3;
							Float dist_to_go = target_dist - my_dist;
							if(dist_to_go > 0)
							{
								Vec3 target_point;
								if(leader->GetRelativePosition(, target_point))
								{
								slave->GetSceneObject()->GetChildByID("TARGET")->PostMessage(MessagePtr(new PositionMessage(target_point)));
								vehicle->PostMessage(MessagePtr(new GotoPositionMessage(target_point)));
								
								Float distance = (vehicle_pos - target_point).Length();
								Float speed = 0;
								
								//Start break!
								if(distance < target_speed*0.5)
								{
									speed = 0;
									//Float w = distance/10.0;
									//speed = w*20 + (1.0-w)*target_speed;
								}
								if(distance < target_speed)
								{
									speed = target_speed;
									//Float w = distance/10.0;
									//speed = w*20 + (1.0-w)*target_speed;
								}
								else
									speed = target_speed*1.5;

								
								vehicle->PostMessage(MessagePtr(new DesiredSpeedMessage(speed)));
							
						}
						else
							vehicle->PostMessage(MessagePtr(new DesiredSpeedMessage(0)));
						}
					}
				}
			}
		}*/
	}
}
