#include "VehicleControllerComponent.h"
#include "AISceneManager.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/Game/GameMessages.h"



namespace GASS
{
	VehicleControllerComponent::VehicleControllerComponent(void) :
		m_Initialized(false),
		m_RandomDelay(0,0),
		m_RandomVelocity(0,0)
	{
		
	}	

	VehicleControllerComponent::~VehicleControllerComponent(void)
	{

	}

	void VehicleControllerComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("VehicleControllerComponent",new Creator<VehicleControllerComponent, IComponent>);
	}

	void VehicleControllerComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(VehicleControllerComponent::OnTriggerEnter,TriggerEnterMessage,0));
		m_Initialized = true;
	}

	double VehicleControllerComponent::GetDelay() const
	{
		double norm_rand = rand() / double(RAND_MAX);
		return m_RandomDelay.x + norm_rand*(m_RandomDelay.y - m_RandomDelay.x);
	}

	double VehicleControllerComponent::GetVelocity() const
	{
		double norm_rand = rand() / double(RAND_MAX);
		return m_RandomVelocity.x + norm_rand*(m_RandomVelocity.y - m_RandomVelocity.x);
	}

	void VehicleControllerComponent::OnTriggerEnter(TriggerEnterMessagePtr message)
	{
		MessagePtr vel_message(new DesiredSpeedMessage(GetVelocity(),(int) (this),GetDelay()));
		message->GetTrigger()->PostMessage(vel_message);
	}

}
