#include "SignatureComponent.h"
#include <boost/bind.hpp>
#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/IComponent.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Sim/SimEngine.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Components/Graphics/ILocationComponent.h"
#include "Sim/Systems/SimSystemManager.h"

namespace GASS
{
	SignatureComponent::SignatureComponent(void) :
		m_Initialized(false),
		m_RadarCrossSection(1.0),
		m_Pos(0,0,0),
		m_Velocity(0,0,0)
		
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


