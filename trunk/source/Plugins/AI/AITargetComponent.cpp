#include "AITargetComponent.h"
#include "AISceneManager.h"
#include "Plugins/Base/CoreMessages.h"


namespace GASS
{
	AITargetComponent::AITargetComponent(void) :
		m_Initialized(false),
		m_CurrentTime(0),
		m_Debug(false),
		m_Position(0,0,0),
		m_Type(UNDEFINED_TARGET),
		m_RandomDelay(0,0),
		m_RandomVelocity(0,0),
		m_Enable(false)
	{
		
	}	

	AITargetComponent::~AITargetComponent(void)
	{

	}

	void AITargetComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("AITargetComponent",new Creator<AITargetComponent, IComponent>);
		REG_PROPERTY(bool,Debug,AITargetComponent)
		REG_PROPERTY(bool,Enable,AITargetComponent)
		REG_PROPERTY(Vec2,RandomDelay,AITargetComponent)
		REG_PROPERTY(Vec2,RandomVelocity,AITargetComponent)
		//Set get target type by string
		RegisterProperty<std::string>("Type", &GetType, &SetType);
	}

	void AITargetComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(AITargetComponent::OnTransChanged,TransformationChangedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(AITargetComponent::OnActivate,DoorMessage,0));
		SceneManagerListenerPtr listener = shared_from_this();
		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<AISceneManager>()->Register(listener);
		m_Initialized = true;
	}

	std::string AITargetComponent::GetType() const
	{
		switch(m_Type)
		{
		case RANDOM_TARGET:
			return "RANDOM_TARGET";
			break;
		case RANDOM_RESPAWN_TARGET:
			return "RANDOM_RESPAWN_TARGET";
			break;
		case PLATFORM_TARGET:
			return "PLATFORM_TARGET";
			break;
		case VELOCITY_TARGET:
			return "VELOCITY_TARGET";
			break;
		case UNDEFINED_TARGET:
			return "UNDEFINED_TARGET";
			break;
		}
		return "ERROR";
	}

	void AITargetComponent::SetType(const std::string &value)
	{
		if(value == "RANDOM_TARGET")
			m_Type = RANDOM_TARGET;
		else if(value == "RANDOM_RESPAWN_TARGET")
			m_Type =RANDOM_RESPAWN_TARGET;
		else if(value == "PLATFORM_TARGET")
			m_Type = PLATFORM_TARGET;
		else if(value == "VELOCITY_TARGET")
			m_Type = VELOCITY_TARGET;
		else
			m_Type = UNDEFINED_TARGET;
	}


	void AITargetComponent::OnActivate(DoorMessagePtr message)
	{
		SetEnable(message->GetOpen());
	}

	


	double AITargetComponent::GetDelay() const
	{
		double norm_rand = rand() / double(RAND_MAX);
		return m_RandomDelay.x + norm_rand*(m_RandomDelay.y - m_RandomDelay.x);
	}


	double AITargetComponent::GetVelocity() const
	{
		double norm_rand = rand() / double(RAND_MAX);
		return m_RandomVelocity.x + norm_rand*(m_RandomVelocity.y - m_RandomVelocity.x);
	}

	void AITargetComponent::OnTransChanged(TransformationChangedEventPtr message)
	{
		m_Position = message->GetPosition();
	}

	void AITargetComponent::SceneManagerTick(double delta_time)
	{
		//debug?
		if(m_Debug)
		{
		
		}
		
	}
	
}
