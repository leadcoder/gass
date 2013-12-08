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
		m_GroupID(0),
		m_CurrentFormation(FT_LINE),
		m_StartPos(0,0,0),
		m_VehiclePos(0,0,0),
		m_VehicleSpeed(0),
		m_DelayAtWaypoint(0)
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
			GetVehicle()->PostMessage(MessagePtr(new WorldPositionMessage(m_StartPos)));
			GetVehicle()->PostMessage(MessagePtr(new WorldRotationMessage(m_StartRot)));
		}
	}


	void VehicleControllerComponent::OnTransformation(TransformationNotifyMessagePtr message)
	{
		m_StartPos = message->GetPosition();
		m_StartRot = message->GetRotation();
		//if paused!
		if(GetVehicle() && m_ScenarioState != SS_PLAY)
		{
			GetVehicle()->PostMessage(MessagePtr(new WorldPositionMessage(m_StartPos)));
			GetVehicle()->PostMessage(MessagePtr(new WorldRotationMessage(m_StartRot)));
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
				vehicle->RegisterForMessage(REG_TMESS(VehicleControllerComponent::OnVehicleVelocity,VelocityNotifyMessage,0));
				vehicle->RegisterForMessage(REG_TMESS(VehicleControllerComponent::OnVehicleTransformation,TransformationNotifyMessage,0));
				//Set to start location?
			}
		}
	}

	void VehicleControllerComponent::OnVehicleVelocity(VelocityNotifyMessagePtr message)
	{
		m_VehicleSpeed  = -message->GetLinearVelocity().z;
	}

	void VehicleControllerComponent::OnVehicleTransformation(TransformationNotifyMessagePtr message)
	{
		m_VehiclePos = message->GetPosition();
		m_VehicleRot = message->GetRotation();
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
				m_DelayAtWaypoint = comp->GenerateRandomDelay();
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
						
						if((pos - m_VehiclePos).Length() < m_TargetRadius)
						{
							m_TargetReached = true;
							//waypoint reached start count down
							
						}
					}
					if(m_TargetReached)
					{
						if(m_DelayAtWaypoint > 0)
							m_DelayAtWaypoint -= time;
						else
						{
							m_BehaviorWaypoints.front()->SetComplete(true);
							if(m_BehaviorWaypoints.front()->IsSyncronized())
							{
								m_BehaviorWaypoints.erase(m_BehaviorWaypoints.begin());
								if(m_BehaviorWaypoints.size() > 0)
									_Apply(m_BehaviorWaypoints.front(),false);
							}
						}
					}
				}
				_UpdateLeaderPathfollow(time);
			}
			else
				_UpdateFormationPathfollow(time);
		}
	}

	bool VehicleControllerComponent::GetFormationPosition(VehicleControllerComponentPtr slave, Vec3 &target_pos, Float &path_distance)
	{
		int id = slave->GetGroupID();
		//TODO: Get bounding box from vehicle instead?  
		const Float max_vehicle_lenght = 5; //5 meters 
		switch(m_CurrentFormation)
		{
		case FT_LINE:
			{
				Float total_dist = m_PreviousDist + m_CurrentPathDist;

				//3 second rule converted to one second rule to get tighter line :)
				Float dist_behinde = max_vehicle_lenght*2*id;
				if(m_TargetSpeed > max_vehicle_lenght*0.5)
					dist_behinde = id*(m_TargetSpeed*2.0 + max_vehicle_lenght); 
				dist_behinde -= m_TargetSpeed; //compensate for look ahead
				path_distance = total_dist - dist_behinde;
				if(path_distance >= 0)
				{
					//Get position ahead of current position and check that distance is less than the formation distance
					int wp_index;
					Vec3 point_on_path;
					Float dist_to_path_dist;
					Float slave_dist = Math::GetPathDistance(slave->GetVehiclePos(), m_FullPath, wp_index,dist_to_path_dist);
					if(slave_dist > slave->GetCurrentDistance())
						slave->SetCurrentDistance(slave_dist);
					else
						slave_dist = slave->GetCurrentDistance();
					//Move target point in front of vehicle, use 2 x speed as distance 
					//because autopilot try to slow down if distance_to_target < speed and we dont want to accelerate/deaccelerate 
					//while moving along path between waypoints
					double look_ahead = m_TargetSpeed;
					//do some clamping
					if(look_ahead < 3) // we need to be at least outside autopilot target radius
						look_ahead = 3;
					if(look_ahead > 100)
						look_ahead = 100;

					Float new_distance = slave_dist + look_ahead;
					if(new_distance < path_distance)
						target_pos = Math::GetPointOnPath(new_distance, m_FullPath, false, wp_index);
					else
						target_pos = Math::GetPointOnPath(path_distance, m_FullPath, false, wp_index);
				}
				else //return false
				{
					return false;
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
					Vec3 dir = m_VehicleRot.GetRotationMatrix().GetViewDirVector();
					Float x = -dir.z;
					dir.z = dir.x;
					dir.x = x;
					dir.y = 0;
					dir.Normalize();
					path_distance = m_CurrentPathDist + m_TargetSpeed; //compensate for look ahead
					target_pos = Math::GetPointOnPath(path_distance, m_Path, false, wp_index);
					target_pos = target_pos + dir * offset;
				}
			}
			break;
		}
		return true;

		/*Float total_dist = m_PreviousDist + m_CurrentPathDist;
		int id = slave->GetGroupID();
		//TODO: Get bouding box from vehicle instead?  
		const Float max_vehicle_lenght = 5; //5 meters 
		switch(m_CurrentFormation)
		{
		case FT_LINE:
			{
				//3 second rule converted to one second rule to get tighter line,:)
				//we also add 10 behinde first vehicle to 
				Float dist_behinde = max_vehicle_lenght*2*id;
				if(m_TargetSpeed > max_vehicle_lenght*0.5)
					dist_behinde = id*(m_TargetSpeed*2.0 + max_vehicle_lenght); 
				dist_behinde -= m_TargetSpeed; //compensate for look ahead
				path_distance = total_dist - dist_behinde;
				if(path_distance >= 0)
				{
					//Get position ahead of current position and check that distance is less than the formation distance
					int wp_index;
					Vec3 point_on_path;
					Float dist_to_path_dist;
					
					Float slave_dist = Math::GetPathDistance(slave->GetVehiclePos(), m_FullPath, wp_index,dist_to_path_dist);
					//Move target point in front of vehicle, use 2 x speed as distance 
					//because autopilot try to slow down if distance_to_target < speed and we dont want to accelerate/deaccelerate 
					//while moving along path between waypoints
					double look_ahead = m_TargetSpeed;
					//do some clamping
					if(look_ahead < 3) // we need to be at least outside autopilot target radius
						look_ahead = 3;
					if(look_ahead > 100)
						look_ahead = 100;

					Float new_distance = slave_dist + look_ahead;
					if(new_distance < path_distance)
						target_pos = Math::GetPointOnPath(new_distance, m_FullPath, false, wp_index);
					else
						target_pos = Math::GetPointOnPath(path_distance, m_FullPath, false, wp_index);
				}
				else //just align behinde
				{
					SceneObjectPtr vehicle = GetVehicle();
					if(vehicle)
					{
						Vec3 dir = m_VehicleRot.GetRotationMatrix().GetViewDirVector();
						dir.Normalize();
						dir = dir*dist_behinde;
						target_pos = m_VehiclePos + dir;
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
					Vec3 dir = m_VehicleRot.GetRotationMatrix().GetViewDirVector();
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
		return true;*/
	}


	void VehicleControllerComponent::_UpdateFormationPathfollow(double time)
	{
		SceneObjectPtr vehicle = GetVehicle();
		if(vehicle)
		{
			//Vec3 vehicle_pos = vehicle->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
			Vec3 point_on_path;
			Float leader_path_distance;
			Vec3 target_pos;
			VehicleControllerComponentPtr this_ptr = DYNAMIC_PTR_CAST<VehicleControllerComponent>(shared_from_this());
			bool target_found = GetLeader()->GetFormationPosition(this_ptr, target_pos, leader_path_distance);

			if(target_found)
			{
				Float max_speed = GetLeader()->GetTargetSpeed() * 2; //we apply behavior speed x2 to reach target
				vehicle->PostMessage(MessagePtr(new DesiredSpeedMessage(max_speed)));
				vehicle->PostMessage(MessagePtr(new GotoPositionMessage(target_pos)));
				GetSceneObject()->GetChildByID("TARGET")->PostMessage(MessagePtr(new PositionMessage(target_pos)));
			}
			else //we have no target!
			{
				vehicle->PostMessage(MessagePtr(new DesiredSpeedMessage(0)));
			}
		}
	}

	void VehicleControllerComponent::_UpdateLeaderPathfollow(double time)
	{
		SceneObjectPtr vehicle = GetVehicle();
		if(vehicle && !m_TargetReached && m_Path.size() > 0)
		{
			//follow path!
			int num_waypoints = (int) m_Path.size();
			int wp_index;
			Vec3 point_on_path;
			Float dist_to_path_dist;
			m_CurrentPathDist = Math::GetPathDistance(m_VehiclePos, m_Path, wp_index,dist_to_path_dist);

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

			Float new_distance = m_CurrentPathDist + look_ahead;
			Vec3 target_point = Math::GetPointOnPath(new_distance, m_Path, false, wp_index);
			
			vehicle->PostMessage(MessagePtr(new GotoPositionMessage(target_point)));
			
			/*if(wp_index == num_waypoints-2)
			{
				Vec3 last_wp = m_Path[num_waypoints-1];
				//Check distance to last wp
				if((last_wp - m_VehiclePos).FastLength() < m_TargetRadius)
				{
					m_TargetReached = true;
				}
			}*/
			
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
	}
	
}

