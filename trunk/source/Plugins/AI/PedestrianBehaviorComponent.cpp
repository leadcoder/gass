#include "PedestrianBehaviorComponent.h"
#include "TriggerComponent.h"
#include "AITargetComponent.h"
#include "DetourCrowdAgentComponent.h"
#include "RecastNavigationMeshComponent.h"
#include "AISceneManager.h"


namespace GASS
{
	PedestrianBehaviorComponent::PedestrianBehaviorComponent(void) : m_GoalRadius(1),
		m_Initialized(false),
		m_RandomSpeed(2,2),
		m_Position(0,0,0),
		m_Health(1.0),
		m_FleeThreshold(0.9),
		m_FleeSpeed(2)
	{

	}	

	PedestrianBehaviorComponent::~PedestrianBehaviorComponent(void)
	{

	}

	void PedestrianBehaviorComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PedestrianBehaviorComponent",new Creator<PedestrianBehaviorComponent, IComponent>);
		RegisterProperty<SceneObjectID>("TargetID", &GetTargetID, &SetTargetID);
		RegisterProperty<SceneObjectID>("SpawnLocationID", &GetSpawnLocationID, &SetSpawnLocationID);
		RegisterProperty<SceneObjectID>("ExitLocationID", &GetExitLocationID, &SetExitLocationID);

		RegisterProperty<Float>("GoalRadius", &GetGoalRadius, &SetGoalRadius);
		RegisterProperty<Vec2>("RandomSpeed", &GetRandomSpeed, &SetRandomSpeed);
		RegisterProperty<Float>("Health", &GetHealth, &SetHealth);
		RegisterProperty<Float>("FleeThreshold", &GetFleeThreshold, &SetFleeThreshold);
		RegisterProperty<Float>("FleeSpeed", &GetFleeSpeed, &SetFleeSpeed);
	}

	void PedestrianBehaviorComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(PedestrianBehaviorComponent::OnLoad,LocationLoadedMessage,1)); //load after agent
		GetSceneObject()->RegisterForMessage(REG_TMESS(PedestrianBehaviorComponent::OnTransformationChanged,TransformationNotifyMessage ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PedestrianBehaviorComponent::OnTriggerExit,TriggerExitMessage ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PedestrianBehaviorComponent::OnTriggerEnter,TriggerEnterMessage ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PedestrianBehaviorComponent::OnHealthChanged,HealthChangedMessage,0));
		
	}

	void PedestrianBehaviorComponent::OnHealthChanged(HealthChangedMessagePtr message)
	{
		SetHealth(message->GetHealth());
	}

	void PedestrianBehaviorComponent::SetHealth(Float health)
	{
		m_Health = health;
		if(m_Initialized)
		{

			DetourCrowdAgentComponentPtr agent = GetSceneObject()->GetFirstComponentByClass<DetourCrowdAgentComponent>();
			if(m_Health < m_FleeThreshold)
			{
				if(m_Health > 0)
				{
					if(m_TargetLocationID != m_ExitLocationID)
						SetTargetID(m_ExitLocationID);
					m_State = "Flee";
					//double norm_rand = rand() / double(RAND_MAX);
					double flee_speed = m_Health*2 + m_FleeSpeed;
					agent->SetMaxSpeed(flee_speed);
				}
				else
				{
					if(m_State != "Dead")
					{
						MessagePtr message(new StanceChangedMessage(6));
						GetSceneObject()->PostMessage(message);
					}

					m_State = "Dead";
					agent->SetMaxSpeed(0);
				}
			}
			else if(m_Health >= 1.0) //reset?
			{

				if(m_State != "Wander")
				{
					MessagePtr message(new StanceChangedMessage(1));
					GetSceneObject()->PostMessage(message);
				}

				m_State = "Wander";
				SetTargetID(m_InitialTargetLocationID);
				SetRandomSpeed(m_RandomSpeed);
			}
		}
	}

	Float PedestrianBehaviorComponent::GetHealth() const
	{
		return m_Health;
	}

	void PedestrianBehaviorComponent::OnLoad(LocationLoadedMessagePtr message)
	{
		GoToRandomTarget(1.0); //send intial target!
		
		//set random speed!
		m_Initialized = true;
		m_State = "Wander";
		m_InitialTargetLocationID = m_TargetLocationID;
		SetRandomSpeed(m_RandomSpeed);

		SceneManagerListenerPtr listener = shared_from_this();
		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<AISceneManager>()->Register(listener);
	}

	void PedestrianBehaviorComponent::SetRandomSpeed(const Vec2 &value)
	{
		m_RandomSpeed = value;
		if(m_Initialized)
		{
			DetourCrowdAgentComponentPtr agent = GetSceneObject()->GetFirstComponentByClass<DetourCrowdAgentComponent>();
			if(agent)
			{
				double norm_rand = rand() / double(RAND_MAX);
				double random_speed = m_RandomSpeed.x + norm_rand*(m_RandomSpeed.y - m_RandomSpeed.x);
				agent->SetMaxSpeed(random_speed);
			}
		}
	}

	void PedestrianBehaviorComponent::SetTargetID(const SceneObjectID &id)
	{
		m_TargetLocationID = id;

		if(m_Initialized)
		{
			GoToRandomTarget(0.0);
		}
	}

	SceneObjectID PedestrianBehaviorComponent::GetTargetID() const
	{
		return m_TargetLocationID;
	}

	void PedestrianBehaviorComponent::SetSpawnLocationID(const SceneObjectID &id)
	{
		m_SpawnLocationID = id;
	}

	SceneObjectID PedestrianBehaviorComponent::GetSpawnLocationID() const
	{
		return m_SpawnLocationID;
	}

	void PedestrianBehaviorComponent::OnTriggerExit(TriggerExitMessagePtr message)
	{
		SceneObjectPtr trigger = message->GetTrigger();
	}

	void PedestrianBehaviorComponent::OnTriggerEnter(TriggerEnterMessagePtr message)
	{
		SceneObjectPtr trigger = message->GetTrigger();
	}
	
	SceneObjectPtr PedestrianBehaviorComponent::GetRandomLocationObject(const SceneObjectID &id) const
	{
		SceneObjectPtr ret;
		std::vector<SceneObjectPtr> objects;
		GetSceneObject()->GetScene()->GetRootSceneObject()->GetChildrenByID(objects,id,false,true);

		unsigned int num_locations = objects.size();
		if(num_locations > 0)
		{
			int index = rand()%num_locations;
			ret = objects[index];
		}
		return ret;
	}

	Vec3 PedestrianBehaviorComponent::GetRandomLocation(SceneObjectPtr so) const
	{
		Vec3 location(0,0,0);
		LocationComponentPtr loc_comp = so->GetFirstComponentByClass<ILocationComponent>();
		RecastNavigationMeshComponentPtr nm = so->GetFirstComponentByClass<RecastNavigationMeshComponent>();
		ShapePtr shape = so->GetFirstComponentByClass<IShape>();
		if(nm)
		{
			DetourCrowdAgentComponentPtr agent = GetSceneObject()->GetFirstComponentByClass<DetourCrowdAgentComponent>();
			agent->GetRandomMeshPosition(location);
		}
		else if(loc_comp && shape)
		{
			location = shape->GetRandomPoint();
			location.y = loc_comp->GetWorldPosition().y;
		}
		return location;
	}

	void PedestrianBehaviorComponent::SceneManagerTick(double delta_time)
	{
		SceneObjectPtr so = GetCurrentTarget();
		if(so)
		{
			Float distance = (m_Position - m_CurrentTargetLocation).Length();
			AITargetComponentPtr target_comp = so->GetFirstComponentByClass<AITargetComponent>();
			m_DebugState = "GotoTarget:" + so->GetName();
			if(target_comp)
			{
				switch(target_comp->GetTargetType())
				{
				case AITargetComponent::RANDOM_TARGET:
					m_DebugState += "\nTT:Proxy";
					break;
				case AITargetComponent::RANDOM_RESPAWN_TARGET:
					m_DebugState += "\nTT:Respawn";
					break;
				case AITargetComponent::PLATFORM_TARGET: //first child is target object
					m_DebugState += "\nTT:Platform";
					break;
				}
			}

			std::stringstream ss;
			ss << "\nTarget distance:"<<  distance;
			m_DebugState += ss.str(); 


			//distance to target
			if(distance < m_GoalRadius)
			{
				
				if(target_comp)
				{
					//Check the target behavior
					switch(target_comp->GetTargetType())
					{
					case AITargetComponent::RANDOM_TARGET:
						GoToRandomTarget(target_comp->GetDelay());
						break;
					case AITargetComponent::RANDOM_RESPAWN_TARGET:
						RandomRespawn(target_comp->GetDelay());
						break;
					case AITargetComponent::PLATFORM_TARGET: //first child is target object
						SceneObjectPtr new_target = boost::shared_dynamic_cast<SceneObject>(so->GetChildren().getNext());
						if(new_target)
						{
							bool enabled = target_comp->GetEnable();
							//wait for target to be active!
							if(enabled)
								GoToTarget(new_target,target_comp->GetDelay());
							else
								m_DebugState += "\nPlatform Target disabled:";
						}
						else
						{
							m_DebugState += "\nNo child nPlatform Target found:";
						}
						break;
					}
				}
				else
					m_DebugState += "\nOutside Goal radius";
			}
		}
		else
		{
			m_DebugState = "No CurrentTarget";
			//Try to find random target?
		}
		//GetSceneObject()->PostMessage(MessagePtr(new TextCaptionMessage(m_State)));
		std::stringstream ss;
		ss  <<  GetSceneObject()->GetName();
		ss  <<  "\nHealth:" << (int) (m_Health*100) << "%";
		ss  <<  "\nState:" << m_State;
		GetSceneObject()->PostMessage(MessagePtr(new TextCaptionMessage(ss.str())));
	}
	
	void PedestrianBehaviorComponent::OnTransformationChanged(TransformationNotifyMessagePtr message)
	{
		m_Position = message->GetPosition();
	}

	void PedestrianBehaviorComponent::RandomRespawn(double delay)
	{
		if(m_SpawnLocationID != "")
		{
			//get random locations
			SceneObjectPtr so = GetRandomLocationObject(m_SpawnLocationID);
			if(so)
			{
				Vec3 spawn_location =  GetRandomLocation(so);
				//Send position message?
				MessagePtr pos_msg(new WorldPositionMessage(spawn_location,(int) this,delay));
				GetSceneObject()->PostMessage(pos_msg);
				GoToRandomTarget(delay);
			}
			else
			{
				m_DebugState += "\nFailed to find spawn location:";
				m_DebugState += std::string(m_SpawnLocationID);
			}
		}
	}

	void PedestrianBehaviorComponent::GoToRandomTarget(double delay)
	{
		if(m_TargetLocationID != "")
		{
			//get random location
			SceneObjectPtr so = GetRandomLocationObject(m_TargetLocationID);
			if(so)
			{
				GoToTarget(so,delay);
			}
		}
	}

	void PedestrianBehaviorComponent::GoToTarget(SceneObjectPtr &obj, double delay)
	{
		//get random locations
		Vec3 target_location =  GetRandomLocation(obj);
		m_CurrentTarget = obj;
		m_CurrentTargetLocation = target_location;
		//Send goto message?
		MessagePtr goto_msg(new GotoPositionMessage(target_location,(int) this,delay));
		GetSceneObject()->PostMessage(goto_msg);
	}
	
}


