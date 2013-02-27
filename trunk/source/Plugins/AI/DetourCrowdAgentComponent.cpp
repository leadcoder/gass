

#include "Detour/DetourCommon.h"

#include "DetourCrowdAgentComponent.h"
#include "DetourCrowdComponent.h"

namespace GASS
{
	DetourCrowdAgentComponent::DetourCrowdAgentComponent(void) : m_MeshData(new ManualMeshData()), 
		m_Radius(0.5),
		m_Group("BLUE"),
		m_Agent(NULL),
		m_Index(-1),
		m_Height(2),
		m_MaxAcceleration(8),
		m_MaxSpeed(3.5),
		m_AccTime(0),
		m_SeparationWeight(1)
	{

	}	

	DetourCrowdAgentComponent::~DetourCrowdAgentComponent(void)
	{

	}

	void DetourCrowdAgentComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("DetourCrowdAgentComponent",new Creator<DetourCrowdAgentComponent, IComponent>);
		RegisterProperty<std::string>("Script", &DetourCrowdAgentComponent::GetScript, &DetourCrowdAgentComponent::SetScript);
		RegisterProperty<std::string>("Group", &DetourCrowdAgentComponent::GetGroup, &DetourCrowdAgentComponent::SetGroup);
		RegisterProperty<float>("Radius", &DetourCrowdAgentComponent::GetRadius, &DetourCrowdAgentComponent::SetRadius);
		RegisterProperty<float>("Height", &DetourCrowdAgentComponent::GetHeight, &DetourCrowdAgentComponent::SetHeight);
		RegisterProperty<float>("MaxAcceleration", &DetourCrowdAgentComponent::GetMaxAcceleration, &DetourCrowdAgentComponent::SetMaxAcceleration);
		RegisterProperty<float>("MaxSpeed", &DetourCrowdAgentComponent::GetMaxSpeed, &DetourCrowdAgentComponent::SetMaxSpeed);
		RegisterProperty<float>("SeparationWeight", &DetourCrowdAgentComponent::GetSeparationWeight, &DetourCrowdAgentComponent::SetSeparationWeight);
	}

	void DetourCrowdAgentComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(DetourCrowdAgentComponent::OnLoad,LocationLoadedMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(DetourCrowdAgentComponent::OnWorldPosition,WorldPositionMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(DetourCrowdAgentComponent::OnChangeName,SceneObjectNameMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(DetourCrowdAgentComponent::OnGoToPosition,GotoPositionMessage,0));
	}

	void DetourCrowdAgentComponent::OnChangeName(GASS::MessagePtr message)
	{
		SceneObjectNameMessagePtr nm = STATIC_PTR_CAST<SceneObjectNameMessage>(message);
	}

	float DetourCrowdAgentComponent::GetMaxSpeed() const
	{
		return m_MaxSpeed;
	}

	void DetourCrowdAgentComponent::SetMaxSpeed(float value)
	{
		m_MaxSpeed = value;
		UpdateAgentParams();
	}

	float DetourCrowdAgentComponent::GetMaxAcceleration() const
	{
		return m_MaxAcceleration;
	}

	void DetourCrowdAgentComponent::SetMaxAcceleration(float value)
	{
		m_MaxAcceleration = value;
		UpdateAgentParams();
	}

	float DetourCrowdAgentComponent::GetRadius() const
	{
		return m_Radius;
	}

	void DetourCrowdAgentComponent::SetRadius(float value)
	{
		m_Radius = value;
		if(m_Agent)
		{
			UpdateGeometry();
			UpdateAgentParams();
		}
	}

	float DetourCrowdAgentComponent::GetHeight() const
	{
		return m_Height;
	}

	void DetourCrowdAgentComponent::SetHeight(float value)
	{
		m_Height = value;
		if(m_Agent)
		{
			UpdateGeometry();
			UpdateAgentParams();
		}
	}


	void DetourCrowdAgentComponent::SetSeparationWeight(float value)
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
		DetourCrowdComponentPtr crowd_comp = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<DetourCrowdComponent>();
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
			DetourCrowdComponentPtr crowd_comp = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<DetourCrowdComponent>();
			dtCrowd* crowd = crowd_comp->GetCrowd();
			dtCrowdAgentParams ap = GetAgentParams();
			crowd->updateAgentParameters(m_Index,&ap);
		}
	}

	void DetourCrowdAgentComponent::OnLoad(LocationLoadedMessagePtr message)
	{
		//SimEngine::Get().GetRuntimeController()->Register(this);
		UpdateGeometry();
		DetourCrowdComponentPtr crowd_comp = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<DetourCrowdComponent>();

		if(crowd_comp)
		{
			crowd_comp->RegisterAgent(DYNAMIC_PTR_CAST<DetourCrowdAgentComponent>(shared_from_this()));
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
		else
		{
			LogManager::getSingleton().stream() << "WARNING: DetourCrowdAgentComponent need DetourCrowdComponent as parent";
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

	void DetourCrowdAgentComponent::OnGoToPosition(GotoPositionMessagePtr message)
	{
		DetourCrowdComponentPtr crowd_comp = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<DetourCrowdComponent>();
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
				pos[0] = message->GetPosition().x;
				pos[1] = message->GetPosition().y;
				pos[2] = message->GetPosition().z;

				navquery->findNearestPoly(pos, ext, filter, &targetRef, targetPos);
				if(targetRef)
				{
					crowd->requestMoveTarget(m_Index, targetRef, targetPos);
				}
			}
		}
	}


	static float frand()
	{
		return (float)rand()/(float)RAND_MAX;
	}

	bool DetourCrowdAgentComponent::GetRandomMeshPosition(Vec3 &pos)
	{
		DetourCrowdComponentPtr crowd_comp = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<DetourCrowdComponent>();
		if(crowd_comp)
		{

			Vec3 center = crowd_comp->GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetWorldPosition();
			return crowd_comp->GetRandomPointInCircle(pos,center,500);
		}
		return false;
	}

	void DetourCrowdAgentComponent::OnDelete()
	{
		DetourCrowdComponentPtr crowd_comp = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<DetourCrowdComponent>();
		if(crowd_comp)
		{
			crowd_comp->UnregisterAgent(DYNAMIC_PTR_CAST<DetourCrowdAgentComponent>(shared_from_this()));
		}
	}

	std::string DetourCrowdAgentComponent::GetScript() const
	{
		return m_Script;
	}

	void DetourCrowdAgentComponent::SetScript(const std::string &file)
	{
		m_Script = file;
	}

	std::string DetourCrowdAgentComponent::GetGroup() const
	{
		return m_Group;
	}

	void DetourCrowdAgentComponent::SetGroup(const std::string &group)
	{
		m_Group = group;
	}

	void DetourCrowdAgentComponent::UpdateLocation(double delta_time)
	{
		//transfer position
		if(m_Agent)
		{
			if(m_Agent->active)
			{
				const float* pos = m_Agent->npos;
				int id = (int) this;
				MessagePtr pos_msg(new WorldPositionMessage(Vec3(pos[0],pos[1],pos[2]),id));
				GetSceneObject()->PostMessage(pos_msg);

				//generate rotatation from accumulated velocity
				const float* vel = m_Agent->vel;
				Vec3 view_dir(vel[0],vel[1],vel[2]);

				m_AccTime += delta_time;

				/*static float m_Delay = 0.1;
				if(m_AccTime > m_Delay)
				{
					m_AccTime  = 0;
					if(view_dir.Length() > 0)
					{
						view_dir.Normalize();
						Vec3 up(0,1,0);
						Vec3 right = -Math::Cross(view_dir,up);
						GASS::Mat4 rot_mat;
						rot_mat.Identity();
						rot_mat.SetViewDirVector(view_dir);
						rot_mat.SetRightVector(right);
						rot_mat.SetUpVector(up);
						m_CurrentRot = m_DesiredRot;
						m_DesiredRot.FromRotationMatrix(rot_mat);
					}
				}
				Quaternion rot = Quaternion::Slerp2(m_AccTime/m_Delay,m_CurrentRot,m_DesiredRot);
				MessagePtr rot_msg(new WorldRotationMessage(rot,id));
				GetSceneObject()->PostMessage(rot_msg);*/

				
				if(view_dir.Length() > 0.1)
				{
					view_dir.Normalize();
					Vec3 up(0,1,0);
					Vec3 right = -Math::Cross(view_dir,up);
					GASS::Mat4 rot_mat;
					rot_mat.Identity();
					rot_mat.SetViewDirVector(view_dir);
					rot_mat.SetRightVector(right);
					rot_mat.SetUpVector(up);
					Quaternion rot;
					rot.FromRotationMatrix(rot_mat);
					m_DesiredRot = rot;

					MessagePtr rot_msg(new WorldRotationMessage(rot,id));
					GetSceneObject()->PostMessage(rot_msg);
				}
				MessagePtr vel_msg(new VelocityNotifyMessage(Vec3(vel[0],vel[1],vel[2]),Vec3(0,0,0),id));
				GetSceneObject()->PostMessage(vel_msg);
			}
		}

	}

	void DetourCrowdAgentComponent::OnWorldPosition(WorldPositionMessagePtr message)
	{
		int id = (int) this;
		if(id != message->GetSenderID())
		{
			Vec3 pos = message->GetPosition();
			DetourCrowdComponentPtr crowd_comp = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<DetourCrowdComponent>();
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
		m_MeshData->VertexVector.clear();
		m_MeshData->IndexVector.clear();
		m_MeshData->Material = "WhiteTransparentNoLighting";
		m_MeshData->Type = LINE_STRIP;

		MeshVertex vertex;
		vertex.TexCoord.Set(0,0);
		vertex.Color.Set(1,0,0,1);
		vertex.Normal = Vec3(0,1,0);

		float samples = 24;
		float rad = 2*MY_PI/samples;
		float x,y;
		vertex.Pos = Vec3(0,0,0);
		m_MeshData->VertexVector.push_back(vertex);
		for(float i = 0 ;i <= samples; i++)
		{
			x = sin(rad*i)*m_Radius;
			y = cos(rad*i)*m_Radius;
			vertex.Pos.Set(x,0,y);
			m_MeshData->VertexVector.push_back(vertex);
		}

		for(float i = 0 ;i <= samples; i++)
		{
			x = sin(rad*i)*m_Radius;
			y = cos(rad*i)*m_Radius;
			vertex.Pos.Set(x,m_Height,y);
			m_MeshData->VertexVector.push_back(vertex);
		}

		m_MeshData->VertexVector.push_back(vertex);

		MessagePtr mesh_message(new ManualMeshDataMessage(m_MeshData));
		GetSceneObject()->PostMessage(mesh_message);
	}
}


