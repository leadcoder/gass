#include "PedestrianBehaviorComponent.h"
#include "TriggerComponent.h"
#include "AITargetComponent.h"
#include "DetourCrowdAgentComponent.h"
#include "AISceneManager.h"


namespace GASS
{
	PedestrianBehaviorComponent::PedestrianBehaviorComponent(void) : m_GoalRadius(1),
		m_Initlized(false),
		m_RandomSpeed(2,2),
		m_Position(0,0,0)
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
		RegisterProperty<Float>("GoalRadius", &GetGoalRadius, &SetGoalRadius);
		RegisterProperty<Vec2>("RandomSpeed", &GetRandomSpeed, &SetRandomSpeed);
	}

	void PedestrianBehaviorComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(PedestrianBehaviorComponent::OnLoad,LocationLoadedMessage,1)); //load after agent
		GetSceneObject()->RegisterForMessage(REG_TMESS(PedestrianBehaviorComponent::OnUnload,UnloadComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PedestrianBehaviorComponent::OnTransformationChanged,TransformationNotifyMessage ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PedestrianBehaviorComponent::OnTriggerExit,TriggerExitMessage ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PedestrianBehaviorComponent::OnTriggerEnter,TriggerEnterMessage ,0));
	}

	void PedestrianBehaviorComponent::OnLoad(LocationLoadedMessagePtr message)
	{
		GoToRandomTarget(1.0); //send intial target!
		//set random speed!

		//register within triggers
		/*std::vector<SceneObjectPtr> triggers;

		if(m_TriggerID != "")
		{
			GetSceneObject()->GetScene()->GetRootSceneObject()->GetChildrenByID(triggers,m_TriggerID,false,true);

			for(size_t i = 0; i < triggers.size();i++)
			{
				TriggerComponentPtr trigger = triggers[i]->GetFirstComponentByClass<TriggerComponent>();
				trigger->RegisterListener(GetSceneObject());
			}
		}*/
		m_Initlized = true;

		SetRandomSpeed(m_RandomSpeed);

		SceneManagerListenerPtr listener = shared_from_this();
		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<AISceneManager>()->Register(listener);

	}

	void PedestrianBehaviorComponent::SetRandomSpeed(const Vec2 &value)
	{
		m_RandomSpeed = value;
		if(m_Initlized)
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

		if(m_Initlized)
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
	
	void PedestrianBehaviorComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
		
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
		//GeometryComponentPtr mesh_comp = so->GetFirstComponentByClass<IGeometryComponent>();
		ShapePtr shape = so->GetFirstComponentByClass<IShape>();
		if(loc_comp && shape)
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
			AITargetComponentPtr target_comp = so->GetFirstComponentByClass<AITargetComponent>();
			Float distance = (m_Position - m_CurrentTargetLocation).Length();

			m_State = "GotoTarget:" + so->GetName();
			if(target_comp)
			{
				switch(target_comp->GetTargetType())
				{
				case AITargetComponent::RANDOM_TARGET:
					m_State += "\nTT:Proxy";
					break;
				case AITargetComponent::RANDOM_RESPAWN_TARGET:
					m_State += "\nTT:Respawn";
					break;
				case AITargetComponent::PLATFORM_TARGET: //first child is target object
					m_State += "\nTT:Platform";
					break;
				}
			}

			std::stringstream ss;
			ss << "\nTarget distance:"<<  distance;
			m_State += ss.str(); 


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
								m_State += "\nPlatform Target disabled:";
						}
						else
						{
							m_State += "\nNo child nPlatform Target found:";
						}
						break;
					}
				}
				else
					m_State += "\nOutside Goal radius";
			}
		}
		else
		{
			m_State = "No CurrentTarget";
			//Try to find random target?
		}
		GetSceneObject()->PostMessage(MessagePtr(new TextCaptionMessage(m_State)));
		//post state!
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
			SceneObjectPtr so = GetRandomLocationObject(m_SpawnLocationID );
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
				m_State += "\nFailed to find spawn location:";
				m_State += std::string(m_SpawnLocationID);
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


