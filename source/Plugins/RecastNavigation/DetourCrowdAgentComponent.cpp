#include "DetourCommon.h"
#include "DetourCrowdAgentComponent.h"
#include "DetourCrowdComponent.h"
#include "Core/Math/GASSMath.h"


namespace GASS
{
	DetourCrowdAgentComponent::DetourCrowdAgentComponent(void) : m_MeshData(new GraphicsMesh()),
		m_Radius(0.5),
		m_Agent(NULL),
		m_Index(-1),
		m_Height(2),
		m_MaxAcceleration(8),
		m_MaxSpeed(3.5),
		m_AccTime(0),
		m_SeparationWeight(1),
		m_VelSmoother(120,Vec3(0,0,0)),
		m_TargetPos(0,0,0),
		m_LastPos(0,0,0)
	{

	}

	DetourCrowdAgentComponent::~DetourCrowdAgentComponent(void)
	{

	}

	void DetourCrowdAgentComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("DetourCrowdAgentComponent",new Creator<DetourCrowdAgentComponent, Component>);
		//RegisterProperty<std::string>("Group", &DetourCrowdAgentComponent::GetGroup, &DetourCrowdAgentComponent::SetGroup);
		RegisterProperty<Float>("Radius", &DetourCrowdAgentComponent::GetRadius, &DetourCrowdAgentComponent::SetRadius);
		RegisterProperty<Float>("Height", &DetourCrowdAgentComponent::GetHeight, &DetourCrowdAgentComponent::SetHeight);
		RegisterProperty<Float>("MaxAcceleration", &DetourCrowdAgentComponent::GetMaxAcceleration, &DetourCrowdAgentComponent::SetMaxAcceleration);
		RegisterProperty<Float>("MaxSpeed", &DetourCrowdAgentComponent::GetMaxSpeed, &DetourCrowdAgentComponent::SetMaxSpeed);
		RegisterProperty<Float>("SeparationWeight", &DetourCrowdAgentComponent::GetSeparationWeight, &DetourCrowdAgentComponent::SetSeparationWeight);
	}

	void DetourCrowdAgentComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(DetourCrowdAgentComponent::OnLoad,LocationLoadedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(DetourCrowdAgentComponent::OnWorldPosition,WorldPositionRequest,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(DetourCrowdAgentComponent::OnChangeName,SceneObjectNameMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(DetourCrowdAgentComponent::OnGoToPosition,GotoPositionRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(DetourCrowdAgentComponent::OnSetDesiredSpeed,DesiredSpeedMessage,0));
	}


	Vec3 DetourCrowdAgentComponent::GetSize() const
	{
		return Vec3(GetRadius()*2,GetHeight(),GetRadius()*2);
	}

	void DetourCrowdAgentComponent::OnSetDesiredSpeed(DesiredSpeedMessagePtr message)
	{
		SetMaxSpeed(message->GetSpeed());
	}

	void DetourCrowdAgentComponent::OnLoad(LocationLoadedEventPtr message)
	{
		UpdateGeometry();
		SceneObjectPtr obj = GetSceneObject()->GetScene()->GetRootSceneObject()->GetChildByID("DETOUR_CROWD");
		if(obj)
		{
			DetourCrowdComponentPtr crowd_comp = obj->GetFirstComponentByClass<DetourCrowdComponent>();
			m_CrowdComp = crowd_comp;
			if(crowd_comp)
			{
				crowd_comp->RegisterAgent(GASS_DYNAMIC_PTR_CAST<DetourCrowdAgentComponent>(shared_from_this()));
				dtCrowd* crowd = crowd_comp->GetCrowd();
				if(crowd)
				{
					Init(crowd);
				}
				else
				{
					LogManager::getSingleton().stream() << "WARNING: DetourCrowdComponent not initialized, forgott to assign navigation mesh to DetourCrowdComponent?";
				}
			}
		}
		else
		{
			LogManager::getSingleton().stream() << "WARNING: DetourCrowdAgentComponent need to find DetourCrowdComponent";
		}
	}

	void DetourCrowdAgentComponent::OnChangeName(GASS::MessagePtr message)
	{
		SceneObjectNameMessagePtr nm = GASS_STATIC_PTR_CAST<SceneObjectNameMessage>(message);
	}

	Float DetourCrowdAgentComponent::GetMaxSpeed() const
	{
		return m_MaxSpeed;
	}

	void DetourCrowdAgentComponent::SetMaxSpeed(Float value)
	{
		m_MaxSpeed = value;
		//UpdateAgentParams();
	}

	Float DetourCrowdAgentComponent::GetMaxAcceleration() const
	{
		return m_MaxAcceleration;
	}

	void DetourCrowdAgentComponent::SetMaxAcceleration(Float value)
	{
		m_MaxAcceleration = value;
		UpdateAgentParams();
	}

	Float DetourCrowdAgentComponent::GetRadius() const
	{
		return m_Radius;
	}

	void DetourCrowdAgentComponent::SetRadius(Float value)
	{
		m_Radius = value;
		if(m_Agent)
		{
			UpdateGeometry();
			UpdateAgentParams();
		}
	}

	Float DetourCrowdAgentComponent::GetHeight() const
	{
		return m_Height;
	}

	void DetourCrowdAgentComponent::SetHeight(Float value)
	{
		m_Height = value;
		if(m_Agent)
		{
			UpdateGeometry();
			UpdateAgentParams();
		}
	}

	void DetourCrowdAgentComponent::SetSeparationWeight(Float value)
	{
		m_SeparationWeight = value;
		if(m_Agent)
		{
			UpdateAgentParams();
		}
	}

	dtCrowdAgentParams DetourCrowdAgentComponent::GetAgentParams() const
	{
		dtCrowdAgentParams ap;
		DetourCrowdComponentPtr crowd_comp = GetCrowdComp();
		if(crowd_comp)
		{
			memset(&ap, 0, sizeof(ap));
			ap.radius = m_Radius;
			ap.height = m_Height;
			ap.maxAcceleration = m_MaxAcceleration;
			ap.maxSpeed = m_MaxSpeed;
			//ap.collisionQueryRange = ap.radius * 12.0f;
			//ap.pathOptimizationRange = ap.radius * 30.0f;

			ap.collisionQueryRange = 12.0f;
			ap.pathOptimizationRange = 30.0f;
			ap.updateFlags = 0;

			if (crowd_comp->GetAnticipateTurns())
				ap.updateFlags |= DT_CROWD_ANTICIPATE_TURNS;
			if (crowd_comp->GetOptimizeVis())
				ap.updateFlags |= DT_CROWD_OPTIMIZE_VIS;
			if (crowd_comp->GetOptimizeTopo())
				ap.updateFlags |= DT_CROWD_OPTIMIZE_TOPO;
			if (crowd_comp->GetObstacleAvoidance())
				ap.updateFlags |= DT_CROWD_OBSTACLE_AVOIDANCE;
			if (crowd_comp->GetSeparation())
				ap.updateFlags |= DT_CROWD_SEPARATION;
			ap.obstacleAvoidanceType = 0;
			ap.separationWeight = m_SeparationWeight;
		}
		return ap;
	}


	void DetourCrowdAgentComponent::UpdateAgentParams()
	{
		if(m_Agent)
		{
			DetourCrowdComponentPtr crowd_comp = GetCrowdComp();
			dtCrowd* crowd = crowd_comp->GetCrowd();
			dtCrowdAgentParams ap = GetAgentParams();
			crowd->updateAgentParameters(m_Index,&ap);
		}
	}


	void DetourCrowdAgentComponent::Init(dtCrowd* crowd)
	{
		LocationComponentPtr location = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		dtCrowdAgentParams ap = GetAgentParams();
		float dpos[3];
		Vec3 pos = location->GetPosition();
		dpos[0] = pos.x; dpos[1] = pos.y; dpos[2] = pos.z;
		m_Index = crowd->addAgent(dpos, &ap);
		if (m_Index != -1)
		{
			//save pointer
			m_Agent = crowd->getAgent(m_Index);
		}
	}

	void DetourCrowdAgentComponent::OnGoToPosition(GotoPositionRequestPtr message)
	{
		m_TargetPos = message->GetPosition();
		DetourCrowdComponentPtr crowd_comp = GetCrowdComp();
		if(crowd_comp)
		{
			RecastNavigationMeshComponentPtr nav_mesh  =  crowd_comp->GetRecastNavigationMeshComponent();
			if(m_Agent && nav_mesh)
			{
				dtNavMeshQuery* navquery = nav_mesh->GetNavMeshQuery();
				dtCrowd* crowd = crowd_comp->GetCrowd();
				const dtQueryFilter* filter = crowd->getFilter();
				const float* ext = crowd->getQueryExtents();
				dtPolyRef targetRef;
				float targetPos[3];
				float pos[3];
				float expand_ext[3];
				expand_ext[0] = ext[0];
				expand_ext[1] = 1;
				expand_ext[2] = ext[2];
				pos[0] = message->GetPosition().x;
				pos[1] = message->GetPosition().y;
				pos[2] = message->GetPosition().z;

				navquery->findNearestPoly(pos, expand_ext, filter, &targetRef, targetPos);
				if(targetRef)
				{
					m_TargetPos.x = targetPos[0];
					m_TargetPos.y = targetPos[1];
					m_TargetPos.z = targetPos[2];
					crowd->requestMoveTarget(m_Index, targetRef, targetPos);
				}
			}
		}
	}

	void DetourCrowdAgentComponent::OnDelete()
	{
		DetourCrowdComponentPtr crowd_comp = GetCrowdComp();
		if(crowd_comp)
		{
			crowd_comp->UnregisterAgent(GASS_DYNAMIC_PTR_CAST<DetourCrowdAgentComponent>(shared_from_this()));
		}
	}

	void DetourCrowdAgentComponent::UpdateLocation(double delta_time)
	{
		//transfer position
		if(m_Agent)
		{
			if(m_Agent->active)
			{
				const float* pos = m_Agent->npos;
				int id = GASS_PTR_TO_INT(this);
				Vec3 current_pos(pos[0],pos[1],pos[2]);

				GetSceneObject()->PostRequest(WorldPositionRequestPtr(new WorldPositionRequest(current_pos,id)));

				Vec3 target_dir = current_pos - m_TargetPos;
				Float dist = target_dir.Length();
				DetourCrowdComponentPtr crowd_comp = GetCrowdComp();

				const float* vel = m_Agent->nvel;
				Vec3 view_dir(vel[0],vel[1],vel[2]);
				Float speed = view_dir.Length();
				Float dist_to_stop = 0.5;
				Float dist_to_slow_down = speed + dist_to_stop;

				if(dist < dist_to_stop)
				{
					//we have arrived!
					float zero_vel[3];
					zero_vel[0] = 0;
					zero_vel[1] = 0;
					zero_vel[2] = 0;
					crowd_comp->GetCrowd()->requestMoveVelocity(m_Index, zero_vel);
				}
				else if (dist < dist_to_slow_down)
				{
					float ratio = (dist-dist_to_stop)/(dist_to_slow_down - dist_to_stop);
					/*float damp_vel[3];
					damp_vel[0] = ratio*m_Agent->vel[0];
					damp_vel[1] = ratio*m_Agent->vel[1];
					damp_vel[2] = ratio*m_Agent->vel[2];

					crowd_comp->GetCrowd()->requestMoveVelocity(m_Index, damp_vel);*/
					dtCrowdAgentParams ap = GetAgentParams();
					ap.maxSpeed = m_MaxSpeed*ratio;
					crowd_comp->GetCrowd()->updateAgentParameters(m_Index,&ap);
				}
				else
				{
					dtCrowdAgentParams ap = GetAgentParams();
					ap.maxSpeed = m_MaxSpeed;
					crowd_comp->GetCrowd()->updateAgentParameters(m_Index,&ap);
				}

				Vec3 c_velocity = (current_pos - m_LastPos)/delta_time;
				//Float speed2 = (current_pos - m_LastPos).Length();
				//speed2 = speed2/delta_time;
				m_LastPos = current_pos;
				//std::cout << speed << " S2:" << speed2 << std::endl;
				if(speed > 0.00001)
				{
					view_dir.y = 0;
					view_dir.Normalize();

					Vec3 final_dir;
					//m_CurrentDir = view_dir;
					//if(speed < 0.2)
					{
						if(m_AccTime > 0.02)
						{
							m_CurrentDir = m_VelSmoother.Update(view_dir);
							m_AccTime = 0;
							m_CurrentDir.Normalize();
						}
						m_AccTime += delta_time;
					}
					Float max_speed = 2;
					Float min_speed = 0.6;
					if(speed > min_speed && speed < max_speed)
					{
						Float ratio = (speed-min_speed)/(max_speed - min_speed);
						final_dir = view_dir*ratio + m_CurrentDir*(1.0-ratio);
						final_dir.Normalize();
					}
					else if(speed >= max_speed)
						final_dir = view_dir;
					else if(speed <= min_speed)
						final_dir = m_CurrentDir;

					Vec3 up(0,1,0);
					Vec3 right = final_dir;
					right.x = final_dir.z;
					right.z = -final_dir.x;
					GASS::Mat4 rot_mat;
					rot_mat.MakeIdentity();
					rot_mat.SetRotationByAxis(right, up, final_dir);
					Quaternion rot;
					rot.FromRotationMatrix(rot_mat);

					GetSceneObject()->PostRequest(WorldRotationRequestPtr(new WorldRotationRequest(rot,id)));
				}
				GetSceneObject()->PostEvent(PhysicsVelocityEventPtr(new PhysicsVelocityEvent(c_velocity,Vec3(0,0,0),id)));
			}
		}

	}

	void DetourCrowdAgentComponent::OnWorldPosition(WorldPositionRequestPtr message)
	{
		int id = GASS_PTR_TO_INT(this);
		if(id != message->GetSenderID())
		{
			Vec3 pos = message->GetPosition();
			DetourCrowdComponentPtr crowd_comp = GetCrowdComp();
			if(m_Agent && crowd_comp)
			{
				dtCrowdAgent* agent = const_cast<dtCrowdAgent*>(m_Agent);
				RecastNavigationMeshComponentPtr nav_mesh  =  crowd_comp->GetRecastNavigationMeshComponent();

				if(nav_mesh)
				{
					dtNavMeshQuery* navquery = nav_mesh->GetNavMeshQuery();
					dtCrowd* crowd = crowd_comp->GetCrowd();

					if(crowd && navquery)
					{
						const dtQueryFilter* filter = crowd->getFilter();
						const float* ext = crowd->getQueryExtents();
						dtPolyRef targetRef;
						float targetPos[3];
						float fpos[3];
						fpos[0] = pos.x;
						fpos[1] = pos.y;
						fpos[2] = pos.z;

						navquery->findNearestPoly(fpos, ext, filter, &targetRef, targetPos);
						if(targetRef)
						{

							agent->corridor.reset(targetRef, targetPos);
							agent->boundary.reset();

							//agent->topologyOptTime = 0;
							//agent->nneis = 0;

							dtVset(agent->dvel, 0,0,0);
							dtVset(agent->nvel, 0,0,0);
							dtVset(agent->vel, 0,0,0);
							dtVcopy(agent->npos, targetPos);
							agent->desiredSpeed = 0;
							//agent->t = 0;

							if (targetRef)
								agent->state = DT_CROWDAGENT_STATE_WALKING;
						}
					}
				}
			}
		}
	}

	void DetourCrowdAgentComponent::UpdateGeometry()
	{
		m_MeshData->SubMeshVector.clear();
		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		m_MeshData->SubMeshVector.push_back(sub_mesh_data);
		sub_mesh_data->Type = LINE_STRIP;
		sub_mesh_data->MaterialName = "WhiteTransparentNoLighting";
		ColorRGBA color(1,0,0,1);

		float samples = 24;
		float rad = 2*GASS_PI/samples;
		float x,y;
		Vec3 pos(0,0,0);
		sub_mesh_data->PositionVector.push_back(pos);
		sub_mesh_data->ColorVector.push_back(color);
		for(float i = 0 ;i <= samples; i++)
		{
			x = sin(rad*i)*m_Radius;
			y = cos(rad*i)*m_Radius;
			pos.Set(x,0,y);
			sub_mesh_data->PositionVector.push_back(pos);
			sub_mesh_data->ColorVector.push_back(color);
		}

		for(float i = 0 ;i <= samples; i++)
		{
			x = sin(rad*i)*m_Radius;
			y = cos(rad*i)*m_Radius;
			pos.Set(x,m_Height,y);
			sub_mesh_data->PositionVector.push_back(pos);
			sub_mesh_data->ColorVector.push_back(color);
		}

		sub_mesh_data->PositionVector.push_back(pos);
		sub_mesh_data->ColorVector.push_back(color);
		GetSceneObject()->PostRequest(ManualMeshDataRequestPtr (new ManualMeshDataRequest(m_MeshData)));
	}
}


