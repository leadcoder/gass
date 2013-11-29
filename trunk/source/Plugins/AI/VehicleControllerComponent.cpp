#include "VehicleControllerComponent.h"
#include "VehicleBehaviorComponent.h"
#include "AISceneManager.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Game/GameMessages.h"



namespace GASS
{
	VehicleControllerComponent::VehicleControllerComponent(void) : m_Initialized(false),
		m_TargetReached(false),
		m_TargetRadius(0)
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

	void VehicleControllerComponent::OnTransformation(TransformationNotifyMessagePtr message)
	{
		m_Pos = message->GetPosition();
		m_Rot = message->GetRotation();
		//if paused!
		if(GetVehicle())
		{
			//Vec3 pos = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
			GetVehicle()->PostMessage(MessagePtr(new WorldPositionMessage(m_Pos)));
			GetVehicle()->PostMessage(MessagePtr(new WorldRotationMessage(m_Rot)));
		}
	}

	void VehicleControllerComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(VehicleControllerComponent::OnTransformation,TransformationNotifyMessage,0));
		m_Initialized = true;
		SetVehicleTemplate(m_VehicleTemplate);
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
	}

	void VehicleControllerComponent::Apply(VehicleBehaviorComponentPtr comp)
	{
		//Always goto to waypoint
		LocationComponentPtr location = comp->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		if(location)
		{
			Vec3 pos = location->GetWorldPosition();
			//TODO: Get path to this waypoint! ,Just generate straight path for now!
			SceneObjectPtr vehicle = GetVehicle();
			if(vehicle)
			{
				Vec3 vehicle_pos = vehicle->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
				std::vector<Vec3> path;
				path.push_back(vehicle_pos);
				path.push_back(pos);
				const double target_radius = 5;
				FollowPath(path, target_radius);
				//Get speed!
				GetSceneObject()->PostMessage(MessagePtr(new DesiredSpeedMessage(9)));
			}
		}
	}
	
	void VehicleControllerComponent::OnUpdate(double time)
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
			Float now_distance = Math::GetPathDistance(vehicle_pos,m_Path,wp_index,ditance_to_path_dist);
			
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
			Float new_distance = now_distance + look_ahead;
			Vec3 target_point = Math::GetPointOnPath(new_distance, m_Path, false, wp_index);
			GetSceneObject()->PostMessage(MessagePtr(new GotoPositionMessage(target_point)));
		}
		else
		{
			GetSceneObject()->PostMessage(MessagePtr(new DesiredSpeedMessage(0)));
		}
	}
}

