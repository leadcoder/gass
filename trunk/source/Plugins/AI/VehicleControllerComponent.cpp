#include "VehicleControllerComponent.h"
#include "VehicleBehaviorComponent.h"
#include "AISceneManager.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Game/GameMessages.h"
#include "Sim/Interface/GASSINavigationComponent.h"



namespace GASS
{
	VehicleControllerComponent::VehicleControllerComponent(void) : m_Initialized(false),
		m_TargetReached(false),
		m_TargetRadius(0),
		m_ScenarioState(SS_STOP),
		m_CurrentPathDist(0),
		m_PreviousDist(0),
		m_TargetSpeed(0),
		m_HasTargetDist(false),
		m_TargetDist(0),
		m_PathOffset(0),
		m_GroupID(0),
		m_CurrentFormation(FT_LINE)
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

		//std::vector<Vec3> final_path;
		SceneObjectPtr obj = GetSceneObject()->GetScene()->GetRootSceneObject()->GetChildByID("AI_NAVIGATION_MESH");
		if(obj)
			m_Navigation = obj->GetFirstComponentByClass<INavigationComponent>();

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

	void VehicleControllerComponent::_Apply(VehicleBehaviorComponentPtr comp, bool first_behavior)
	{

		//TODO: Get path to this waypoint! , Just generate straight path for now!
		SceneObjectPtr vehicle = GetVehicle();
		if(vehicle)
		{
			if(m_GroupID == 0) //leader
			{
				m_TargetRadius = comp->GetWaypointRadius();
				if(first_behavior || comp->GetSpeedMode().GetValue() != ST_UNCHANGED)
				{
					m_TargetSpeed = comp->GetSpeed();
					vehicle->PostMessage(MessagePtr(new DesiredSpeedMessage(m_TargetSpeed)));
				}

				if(comp->GetFormation().GetValue() != FT_UNCHANGED)
				{
					m_CurrentFormation = comp->GetFormation().GetValue();
				}
			}
			//update path 
			LocationComponentPtr wp_location = comp->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
			
			if(wp_location)
			{
				Vec3 start_pos;
				Vec3 wp_pos = wp_location->GetWorldPosition();
				m_TargetReached = false;
				//TODO: nav_mesh search!
				NavigationComponentPtr nav = _GetNavigation();
				if(first_behavior) //first wp add current vehicle position as start position
				{
					start_pos = vehicle->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
					m_Path.clear();
					m_FullPath.clear();
					m_PreviousDist = 0;
				}
				else 
				{
					start_pos = m_Path.back(); 
					//add previous path distance
					Float path_dist = 0; 
					for(size_t i = 1; i <m_Path.size() ; i++ )
					{
						m_PreviousDist += (m_Path[i-1] - m_Path[i]).Length();
					}
					m_Path.clear();
				}

				if(nav)
				{
					//search navigation mesh for path
					bool path_found = nav->GetShortestPath(start_pos,wp_pos,m_Path);
					if(path_found)
					{
						size_t i = 0;
						if(first_behavior)
							i = 1;
						for(size_t i = 0 ; i < m_Path.size(); i++)
						{
							m_FullPath.push_back(m_Path[i]);
						}
					}
				}
				else
				{
					if(first_behavior)
					{
						m_Path.push_back(start_pos);
						m_FullPath.push_back(start_pos);
					}
					else
					{
						m_Path.push_back(start_pos);
					}

					m_Path.push_back(wp_pos); //add next waypoint!
					m_FullPath.push_back(wp_pos);
				}
				
				m_CurrentPathDist = 0;
			}
		}
	}

	void VehicleControllerComponent::OnUpdate(double time)
	{
		SceneObjectPtr vehicle = GetVehicle();
		if(vehicle)
		{
			if(m_GroupID == 0) //leader
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
								_Apply(m_BehaviorWaypoints.front(),false);
						}
					}
				}
				OnLeaderPathfollow(time);
			}
			else
				OnFormationPathfollow(time);
		}
	}

	bool VehicleControllerComponent::GetFormationPosition(int id, Vec3 &target_pos, Float &path_distance)
	{
		Float total_dist = m_PreviousDist + m_CurrentPathDist;
		switch(m_CurrentFormation)
		{
		case FT_LINE:
			{
				//TODO we need to use real vehicle speed here!
				Float dist_behinde = 3 + (id)*m_TargetSpeed*3; //3 second rule
				dist_behinde -= m_TargetSpeed; //compensate for look ahead
				path_distance = total_dist - dist_behinde;
				
				
				if(path_distance >= 0)
				{
					int wp_index;
					target_pos = Math::GetPointOnPath(path_distance, m_FullPath, false, wp_index);
				}
				else //just align behinde
				{
					SceneObjectPtr vehicle = GetVehicle();
					if(vehicle)
					{
						Quaternion vehicle_rot = vehicle->GetFirstComponentByClass<ILocationComponent>()->GetRotation();
						Vec3  vehicle_pos = vehicle->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
						Mat4 mat;
						mat.Identity();
						vehicle_rot.ToRotationMatrix(mat);
						Vec3 dir = mat.GetViewDirVector();
						dir.Normalize();
						dir = dir*dist_behinde;
						target_pos = vehicle_pos + dir;
					}
				}
			}
			break;
		case FT_WALL:
			{
				int wp_index;
				
				Float offset = 10; 
				
				if(id & 1) //is odd
				{
					
					offset = -offset* (1 + (id-1)/2);
				}
				else //even
				{
					offset = offset*id/2;
				}

				SceneObjectPtr vehicle = GetVehicle();
				if(vehicle)
				{
					Quaternion vehicle_rot = vehicle->GetFirstComponentByClass<ILocationComponent>()->GetRotation();
					Mat4 mat;
					mat.Identity();
					vehicle_rot.ToRotationMatrix(mat);
					Vec3 dir = mat.GetViewDirVector();
					Float x = -dir.z;
					dir.z = dir.x;
					dir.x = x;
					dir.y = 0;
					dir.Normalize();


					path_distance = total_dist + m_TargetSpeed; //compensate for look ahead

					target_pos = Math::GetPointOnPath(path_distance, m_FullPath, false, wp_index);
					target_pos = target_pos + dir * offset;
				}
			}
			break;
		}
		return true;
	}


	void VehicleControllerComponent::OnFormationPathfollow(double time)
	{
		SceneObjectPtr vehicle = GetVehicle();
		if(vehicle)
		{
			Vec3 vehicle_pos = vehicle->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
			Vec3 point_on_path;
			Float leader_path_distance;
			Vec3 target_pos;
			GetLeader()->GetFormationPosition(m_GroupID, target_pos, leader_path_distance);
			Float max_speed = GetLeader()->GetTargetSpeed() * 2;
			vehicle->PostMessage(MessagePtr(new DesiredSpeedMessage(max_speed)));

			//m_CurrentPathDist = Math::GetPathDistance(vehicle_pos ,m_Path,wp_index, ditance_to_path_dist);
			//Float target_dist = Math::GetPathDistance(target_pos ,m_Path,wp_index,ditance_to_path_dist);
			//check distance 
			//if(m_CurrentPathDist < target_dist)
			{
				vehicle->PostMessage(MessagePtr(new GotoPositionMessage(target_pos)));
				GetSceneObject()->GetChildByID("TARGET")->PostMessage(MessagePtr(new PositionMessage(target_pos)));
				//Float new_speed = (target_dist - m_CurrentPathDist)*0.7;
				//if(new_speed > 20)
				
			}
			//else
			//	vehicle->PostMessage(MessagePtr(new DesiredSpeedMessage(0)));
		}
		else
		{
			m_TargetSpeed = 0;
			vehicle->PostMessage(MessagePtr(new DesiredSpeedMessage(m_TargetSpeed)));
		}
	}

	void VehicleControllerComponent::OnLeaderPathfollow(double time)
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

			//Move target point in front of vehicle, use 2 x speed as distance 
			//because autopilot try to slow down if distance_to_target < speed and we dont want to accelerate/deaccelerate 
			//while moving along path between waypoints

			//double look_ahead = m_TargetSpeed*2;
			double look_ahead = m_TargetSpeed;
			//do some clamping
			if(look_ahead < 3) // we need to be at least outside autopilot target radius
				look_ahead = 3;
			if(look_ahead > 100)
				look_ahead = 100;

			if(wp_index == num_waypoints-2)
			{
				Vec3 last_wp = m_Path[num_waypoints-1];
				//Check distance to last wp
				if((last_wp - vehicle_pos).FastLength() < m_TargetRadius)
				{
					m_TargetReached = true;
				}
			}
			Float new_distance = m_CurrentPathDist + look_ahead;
			Vec3 target_point = Math::GetPointOnPath(new_distance, m_Path, false, wp_index);
			vehicle->PostMessage(MessagePtr(new GotoPositionMessage(target_point)));
			GetSceneObject()->GetChildByID("TARGET")->PostMessage(MessagePtr(new PositionMessage(target_point)));
		}
		else
		{ 
			//end of path
			//m_TargetSpeed = 0;
			//vehicle->PostMessage(MessagePtr(new DesiredSpeedMessage(m_TargetSpeed)));
		}
	}

	void VehicleControllerComponent::SetBehaviorList(std::vector<VehicleBehaviorComponentPtr> behaviors)
	{
		m_BehaviorWaypoints = behaviors;
		if(behaviors.size() > 0)
			_Apply(behaviors.front(), true);

		//generate complete path!
		//std::vector<Vec3> path;

		/*SceneObjectPtr vehicle = GetVehicle();
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
		}*/


		//const double target_radius = 5;
		//m_Normals = Math::GenerateNormals(path);
		//FollowPath(path, target_radius);
		//Check waypoint behavior

		//m_OrgPath = path;
		//if(m_PathOffset != 0)
		//	m_Path = Math::GenerateOffset(m_OrgPath,m_PathOffset);
		//}
	}

	/*void VehicleControllerComponent::OffsetPath(Float offset)
	{
	m_Path = Math::GenerateOffset(m_OrgPath,offset);
	}*/

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


	bool VehicleControllerComponent::GetPathDistance(const Vec3 &point, Float &distance)
	{
		SceneObjectPtr vehicle = GetVehicle();
		if(vehicle && m_Path.size() > 0)
		{
			int wp_index;
			Float ditance_to_path_dist;
			distance = Math::GetPathDistance(point,m_Path,wp_index,ditance_to_path_dist);
		}
		return false;
	}

	void VehicleControllerComponent::SetTargetDistance(Float dist)
	{
		m_TargetDist = dist;
		m_HasTargetDist = true;
	}
}

