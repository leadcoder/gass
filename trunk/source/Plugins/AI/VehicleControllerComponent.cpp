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
		m_CurrentPathDist(0),
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
		//if(location)
		{
			//Vec3 pos = location->GetWorldPosition();
			//TODO: Get path to this waypoint! , Just generate straight path for now!
			SceneObjectPtr vehicle = GetVehicle();
			if(vehicle)
			{
				if(m_GroupID == 0) //leader
				{
					m_TargetSpeed = comp->GetSpeed();
					vehicle->PostMessage(MessagePtr(new DesiredSpeedMessage(m_TargetSpeed)));

					if(comp->GetFormation().GetValue() != FT_UNCHANGED)
						m_CurrentFormation = comp->GetFormation().GetValue();
				}
				/*else //just do formation!
				{
					m_TargetSpeed = comp->GetSpeed();
					if(comp->GetFormation().GetValue() != FT_UNCHANGED)
						m_CurrentFormation = comp->GetFormation().GetValue();

					if(comp->GetFormation().GetValue() != FT_WALL)
						OffsetPath(5*m_GroupID);
				}*/
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
								Apply(m_BehaviorWaypoints.front());
						}
					}
				}
				OnLeaderPathfollow(time);
			}
			else
				OnFormationPathfollow(time);
			
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

			//check if we have path target distance!
			if(m_GroupID != 0)
			{
				switch(m_CurrentFormation)
				{
				case FT_UNCHANGED:
					break;
				case FT_LINE:
					{
						Float dist_behinde = 10 + (m_GroupID)*m_TargetSpeed*3;
						SetTargetDistance(GetLeader()->GetCurrentDistance() - dist_behinde);
					}
					break;
				case FT_WALL:
					{
						SetTargetDistance(GetLeader()->GetCurrentDistance());
					}
					break;
				}
				//if(m_HasTargetDist)
				Vec3 tp = Math::GetPointOnPath(m_TargetDist, m_Path, false, wp_index);
				GetSceneObject()->GetChildByID("TARGET")->PostMessage(MessagePtr(new PositionMessage(tp)));

				if(m_CurrentPathDist  > m_TargetDist)
				{
					//try to sett correct speed
					vehicle->PostMessage(MessagePtr(new DesiredSpeedMessage(0)));
				}
				else
				{
					Float new_speed = (m_TargetDist - m_CurrentPathDist)*0.5;
					if(new_speed > 20)
						new_speed = 20;
					vehicle->PostMessage(MessagePtr(new DesiredSpeedMessage(new_speed)));
				}
			}
		}
		else
		{
			m_TargetSpeed = 0;
			vehicle->PostMessage(MessagePtr(new DesiredSpeedMessage(m_TargetSpeed)));
		}
	}

	bool VehicleControllerComponent::GetFormationPosition(int id, Vec3 &target_pos, Float &path_distance)
	{
		switch(m_CurrentFormation)
		{
			case FT_LINE:
			{
				Float dist_behinde = 10 + (id)*m_TargetSpeed*3;
				path_distance = m_CurrentPathDist - dist_behinde;
				int wp_index;
				target_pos = Math::GetPointOnPath(path_distance, m_Path, false, wp_index);
			}
			break;
			case FT_WALL:
			{
				int wp_index;
				Float offset = 10; 
				if(id & 1) //is even
					offset = -offset;

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

					target_pos = Math::GetPointOnPath(m_CurrentPathDist, m_Path, false, wp_index);
					target_pos = target_pos + dir * offset;
					path_distance = m_CurrentPathDist;
				}
				
			}
			break;
		}
		return true;
	}
	

	void VehicleControllerComponent::OnFormationPathfollow(double time)
	{
		SceneObjectPtr vehicle = GetVehicle();
		if(vehicle && m_Path.size() > 0)
		{
			Vec3 vehicle_pos = vehicle->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
			//follow path!
			int num_waypoints = (int) m_Path.size();
			int wp_index;
			Vec3 point_on_path;
			Float ditance_to_path_dist;
			Float leader_path_distance;
			Vec3 target_pos;
			GetLeader()->GetFormationPosition(m_GroupID, target_pos, leader_path_distance);
			m_CurrentPathDist = Math::GetPathDistance(vehicle_pos ,m_Path,wp_index, ditance_to_path_dist);
			Float target_dist = Math::GetPathDistance(target_pos ,m_Path,wp_index,ditance_to_path_dist);
			//check distance 
			if(m_CurrentPathDist < target_dist)
			{
				vehicle->PostMessage(MessagePtr(new GotoPositionMessage(target_pos)));
				GetSceneObject()->GetChildByID("TARGET")->PostMessage(MessagePtr(new PositionMessage(target_pos)));
				//Float new_speed = (target_dist - m_CurrentPathDist)*0.7;
				//if(new_speed > 20)
				Float new_speed = 20;
				vehicle->PostMessage(MessagePtr(new DesiredSpeedMessage(new_speed)));
			}
			else
				vehicle->PostMessage(MessagePtr(new DesiredSpeedMessage(0)));
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
			//end of path
			m_TargetSpeed = 0;
			vehicle->PostMessage(MessagePtr(new DesiredSpeedMessage(m_TargetSpeed)));
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

			if(behaviors.size() > 0)
				Apply(behaviors.front());
			const double target_radius = 5;
			m_Normals = Math::GenerateNormals(path);
			FollowPath(path, target_radius);
			//Check waypoint behavior
			m_BehaviorWaypoints = behaviors;
			//m_OrgPath = path;
			//if(m_PathOffset != 0)
			//	m_Path = Math::GenerateOffset(m_OrgPath,m_PathOffset);
			
		}
	}

	void VehicleControllerComponent::OffsetPath(Float offset)
	{
		m_Path = Math::GenerateOffset(m_OrgPath,offset);
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

