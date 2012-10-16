#include "VelocityTriggerComponent.h"
#include "AISceneManager.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Game/GameMessages.h"



namespace GASS
{
	VelocityTriggerComponent::VelocityTriggerComponent(void) :
		m_Initialized(false),
		m_RandomDelay(0,0),
		m_RandomVelocity(0,0)
	{
		
	}	

	VelocityTriggerComponent::~VelocityTriggerComponent(void)
	{

	}

	void VelocityTriggerComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("VelocityTriggerComponent",new Creator<VelocityTriggerComponent, IComponent>);
		REG_ATTRIBUTE(bool,Enable,VelocityTriggerComponent)
		REG_ATTRIBUTE(Vec2,RandomDelay,VelocityTriggerComponent)
		REG_ATTRIBUTE(Vec2,RandomVelocity,VelocityTriggerComponent)
	}

	void VelocityTriggerComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(VelocityTriggerComponent::OnTriggerEnter,TriggerEnterMessage,0));
		m_Initialized = true;
	}

	double VelocityTriggerComponent::GetDelay() const
	{
		double norm_rand = rand() / double(RAND_MAX);
		return m_RandomDelay.x + norm_rand*(m_RandomDelay.y - m_RandomDelay.x);
	}

	double VelocityTriggerComponent::GetVelocity() const
	{
		double norm_rand = rand() / double(RAND_MAX);
		return m_RandomVelocity.x + norm_rand*(m_RandomVelocity.y - m_RandomVelocity.x);
	}


	void VelocityTriggerComponent::OnTriggerEnter(TriggerEnterMessagePtr message)
	{
		MessagePtr vel_message(new DesiredSpeedMessage(GetVelocity(),(int) (this),GetDelay()));
		message->GetTrigger()->PostMessage(vel_message);
	}

	
}
