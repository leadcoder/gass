#include "SignatureComponent.h"
#include <boost/bind.hpp>
#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSIComponent.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Scene/GASSScene.h"
#include "Sim/Scene/GASSSceneObject.h"

#include "Sim/Components/Graphics/GASSILocationComponent.h"
#include "Sim/Systems/GASSSimSystemManager.h"

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
		ComponentFactory::GetPtr()->Register("SignatureComponent",new Creator<SignatureComponent, IComponent>);
		REG_ATTRIBUTE(float,RadarCrossSection,SignatureComponent)
		REG_ATTRIBUTE(PlatformTypeReflection,PlatformType,SignatureComponent)
		REG_ATTRIBUTE(PlatformTeamReflection,PlatformTeam,SignatureComponent)
	}

	void SignatureComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(SignatureComponent::OnLoad,LoadGFXComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(SignatureComponent::OnUnload,UnloadComponentsMessage,1));
		GetSceneObject()->RegisterForMessage(REG_TMESS(SignatureComponent::OnTransChanged,TransformationNotifyMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(SignatureComponent::OnVelocity,VelocityNotifyMessage,0));
	}

	void SignatureComponent::OnLoad(MessagePtr message)
	{
		m_Initialized = true;
	}

	void SignatureComponent::OnUnload(MessagePtr message)
	{
		m_Initialized = false;
	}

	void SignatureComponent::OnTransChanged(TransformationNotifyMessagePtr message)
	{
		m_Pos = message->GetPosition();
		m_Rotation = message->GetRotation();
	}

	void SignatureComponent::OnVelocity(VelocityNotifyMessagePtr message)
	{
		m_Velocity = message->GetLinearVelocity();
	}

	
}


