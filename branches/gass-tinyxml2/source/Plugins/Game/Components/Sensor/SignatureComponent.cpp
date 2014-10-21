#include "SignatureComponent.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponent.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"

#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/GASSSimSystemManager.h"

namespace GASS
{
	SignatureComponent::SignatureComponent(void) :
		m_Initialized(false),
		m_RadarCrossSection(1.0),
		m_Pos(0,0,0),
		m_Velocity(0,0,0),
		m_PlatformType(PT_CAR),
		m_PlatformTeam(BLUE_TEAM)
	{
		
	}	

	SignatureComponent::~SignatureComponent(void)
	{

	}

	void SignatureComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("SignatureComponent",new Creator<SignatureComponent, Component>);
		REG_PROPERTY(float,RadarCrossSection,SignatureComponent)
		REG_PROPERTY(PlatformTypeReflection,PlatformType,SignatureComponent)
		REG_PROPERTY(PlatformTeamReflection,PlatformTeam,SignatureComponent)
	}

	void SignatureComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(SignatureComponent::OnTransChanged,TransformationChangedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(SignatureComponent::OnVelocity,PhysicsVelocityEvent,0));
		m_Initialized = true;
	}

	void SignatureComponent::OnTransChanged(TransformationChangedEventPtr message)
	{
		m_Pos = message->GetPosition();
		m_Rotation = message->GetRotation();
	}

	void SignatureComponent::OnVelocity(PhysicsVelocityEventPtr message)
	{
		m_Velocity = message->GetLinearVelocity();
	}

	
}


