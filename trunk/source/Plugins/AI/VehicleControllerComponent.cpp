#include "VehicleControllerComponent.h"
#include "VehicleBehaviorComponent.h"
#include "AISceneManager.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Game/GameMessages.h"



namespace GASS
{
	VehicleControllerComponent::VehicleControllerComponent(void) : m_Initialized(false),
		m_TargetReached(false),
		m_TargetRadius(0),
		m_ScenarioState(SS_STOP),
		m_CurrentPathDist(0)
	{

	}	

	VehicleControllerComponent::~VehicleControllerComponent(void)
	{

	}

	void VehicleControllerComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("VehicleControllerComponent",new Creator<VehicleControllerComponent, IComponent>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("VehicleControllerComponent", OF_VISIBLE)));
		RegisterProperty<std::string>("VehicleTemplate", &VehicleControllerComponent::GetVehicleTemplate, &VehicleControllerComponent::SetVehicleTemplate,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Vehicle Template",PF_VISIBLE)));
	}

	void VehicleControllerComponent::OnInitialize()
	{
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(VehicleControllerComponent::OnScenarioEvent,ScenarioStateRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(VehicleControllerComponent::OnTransformation,TransformationNotifyMessage,0));
		m_Initialized = true;
		SetVehicleTemplate(m_VehicleTemplate);
	}

	void VehicleControllerComponent::OnScenarioEvent(ScenarioStateRequestPtr message)
	{
		m_ScenarioState = message->GetState();
		if(message->GetState() == SS_PLAY)
		{
			//OnPlay();
			//m_Update = true;
			
		}
		else if(message->GetState() == SS_STOP)
		{
			//reset start position
			GetVehicle()->PostMessage(MessagePtr(new WorldPositionMessage(m_Pos)));
			GetVehicle()->PostMessage(MessagePtr(new WorldRotationMessage(m_Rot)));
		}
	}


	void VehicleControllerComponent::OnTransformation(TransformationNotifyMessagePtr message)
	{
		m_Pos = message->GetPosition();
		m_Rot = message->GetRotation();
		//if paused!
		if(GetVehicle() && m_ScenarioState != SS_PLAY)
		{
			//Vec3 pos = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
			GetVehicle()->PostMessage(MessagePtr(new WorldPositionMessage(m_Pos)));
			GetVehicle()->PostMessage(MessagePtr(new WorldRotationMessage(m_Rot)));
		}
	}

	std::string VehicleControllerComponent::GetVehicleTemplate() const
	{
		return m_VehicleTemplate;
	}

	void VehicleControllerComponent::SetVehicleTemplate(const std::string &template_name)
	{
		m_VehicleTemplate = template_name;
		if(m_Initialized)
		{
			//TODO: Remove previous vehicle
			SceneObjectPtr vehicle = SimEngine::Get().CreateObjectFromTemplate(template_name);
			if(vehicle)
			{
				m_Vehicle = vehicle;
				GetSceneObject()->AddChildSceneObject(vehicle,false);
				//Set to start location?
			}
		}
	}

	void VehicleControllerComponent::FollowPath(const std::vector<Vec3> path, double target_radius)
	{
		m_TargetReached = false;
		m_Path = path;
		m_TargetRadius = target_radius;
		m_CurrentPathDist = 0;
	}

	void VehicleControllerComponent::Apply(VehicleBehaviorComponentPtr comp)
	{
		//Always goto to waypoint
		//LocationComponentPtr location = comp->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
	//	if(location)
		{
		//	Vec3 pos = location->GetWorldPosition();
			//TODO: Get path to this waypoint! ,Just generate straight path for now!
			SceneObjectPtr vehicle = GetVehicle();
			if(vehicle)
			{
				/*Vec3 vehicle_pos = vehicle->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
				std::vector<Vec3> path;
				path.push_back(vehicle_pos);
				path.push_back(pos);
				const double target_radius = 5;
				FollowPath(path, target_radius);*/
				//TODO: Get speed from behavior!
				vehicle->PostMessage(MessagePtr(new DesiredSpeedMessage(9)));
			}
		}
	}


	void VehicleControllerComponent::OnUpdate(double time)
	{
		SceneObjectPtr vehicle = GetVehicle();
		if(vehicle)
		{
			if(m_BehaviorWaypoints.size() > 0)
			{

				VehicleBehaviorComponentPtr  behavior = m_BehaviorWaypoints.front();
				LocationComponentPtr location = behavior->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
				if(location)
				{
					Vec3 pos = location->GetWorldPosition();
					Vec3 vehicle_pos = vehicle->GetFirstComponentByClass<ILocationComponent>()->GetPosition();

					if((pos - vehicle_pos).Length() < m_TargetRadius)
					{
						m_BehaviorWaypoints.erase(m_BehaviorWaypoints.begin());
						if(m_BehaviorWaypoints.size() > 0)
							Apply(m_BehaviorWaypoints.front());
					}
				}
			}
			OnPathfollow(time);
		}
	}
	
	void VehicleControllerComponent::OnPathfollow(double time)
	{
		SceneObjectPtr vehicle = GetVehicle();
		if(vehicle && !m_TargetReached && m_Path.size() > 0)
		{
			Vec3 vehicle_pos = vehicle->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
			//follow path!
			int num_waypoints = (int) m_Path.size();
			int wp_index;
			Vec3 point_on_path;
			Float ditance_to_path_dist;
			m_CurrentPathDist = Math::GetPathDistance(vehicle_pos,m_Path,wp_index,ditance_to_path_dist);
			
			double look_ahead = 10;
			if(look_ahead < 3)
				look_ahead = 3;
			if(look_ahead > 10)
				look_ahead = 10;
			
			if(wp_index == num_waypoints-2)
			{
				Vec3 last_wp = m_Path[num_waypoints-1];
				//Check distance to last wp
				if((last_wp - vehicle_pos).FastLength() < m_TargetRadius)
				{
					//GetSceneObject()->PostMessage(MessagePtr(new DesiredSpeedMessage(0)));
					m_TargetReached = true;
				}
			}
			Float new_distance = m_CurrentPathDist + look_ahead;
			Vec3 target_point = Math::GetPointOnPath(new_distance, m_Path, false, wp_index);
			vehicle->PostMessage(MessagePtr(new GotoPositionMessage(target_point)));
		}
		else
		{
			vehicle->PostMessage(MessagePtr(new DesiredSpeedMessage(0)));
		}
	}

	void VehicleControllerComponent::SetBehaviorList(std::vector<VehicleBehaviorComponentPtr> behaviors)
	{
		//generate complete path!
		std::vector<Vec3> path;
		SceneObjectPtr vehicle = GetVehicle();
		if(vehicle)
		{
			Vec3 vehicle_pos = vehicle->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
			path.push_back(vehicle_pos);
			for(size_t i = 0;  i < behaviors.size(); i++)
			{
				VehicleBehaviorComponentPtr behavior = behaviors[i];
				LocationComponentPtr location = behavior->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
				if(location)
				{
					Vec3 pos = location->GetWorldPosition();
					//TODO: Get path to this waypoint! ,Just generate straight path for now!
					path.push_back(pos);
				}
			}

			if(m_BehaviorWaypoints.size() > 0)
				Apply(m_BehaviorWaypoints.front());
			const double target_radius = 5;
			FollowPath(path, target_radius);
			//Check waypoint behavior
			m_BehaviorWaypoints = behaviors;
		}
	}

	bool VehicleControllerComponent::GetRelativePosition(Float behinde_dist, Vec3 &target_position)
	{
		SceneObjectPtr vehicle = GetVehicle();
		if(vehicle && m_Path.size() > 0)
		{
			if(m_CurrentPathDist > behinde_dist)
			{
				Float new_distance = m_CurrentPathDist - behinde_dist;
				int wp_index;
				target_position = Math::GetPointOnPath(new_distance, m_Path, false, wp_index);
				return true;
			}
		}
		return false;
	}
}

