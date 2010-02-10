/****************************************************************************
*                                                                           *
* HiFiEngine                                                                *
* Copyright (C)2003 - 2005 Johan Hedstrom                                   *
* Email: hifiengine@gmail.com                                               *
* Web page: http://n00b.dyndns.org/HiFiEngine                               *
*                                                                           *
* HiFiEngine is only used with knowledge from the author. This software     *
* is not allowed to redistribute without permission from the author.        *
* For further license information, please turn to the product home page or  *
* contact author. Abuse against the HiFiEngine license is prohibited by law.*
*                                                                           *
*****************************************************************************/


#include "DebugComponent.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Utils/Log.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/SimEngine.h"

namespace GASS
{
	DebugComponent::DebugComponent(void) : m_ShowNodeName(false)
	{

	}

	DebugComponent::~DebugComponent(void)
	{
		
	}

	void DebugComponent::RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("DebugComponent",new GASS::Creator<DebugComponent, IComponent>);
		RegisterProperty<bool>("ShowSceneObjectName", &GASS::DebugComponent::GetShowNodeName, &GASS::DebugComponent::SetShowNodeName);
	}

	void DebugComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(typeid(LoadSimComponentsMessage),MESSAGE_FUNC(DebugComponent::OnLoad),0);
		GetSceneObject()->RegisterForMessage(typeid(SceneObjectNameMessage),MESSAGE_FUNC(DebugComponent::OnChangeName),0);
	}

	bool DebugComponent::GetShowNodeName() const
	{
		return m_ShowNodeName;
	}

	void DebugComponent::SetShowNodeName(bool value)
	{
		m_ShowNodeName = value;
	}
	
	void DebugComponent::OnChangeName(GASS::MessagePtr message)
	{
		if(m_ShowNodeName)
		{
			SceneObjectNameMessagePtr name_mess = boost::shared_dynamic_cast<SceneObjectNameMessage>(message);
			if(name_mess)
			{
				std::string name = name_mess->GetName();
				MessagePtr text_mess(new TextCaptionMessage(name));
				GetSceneObject()->PostMessage(text_mess);
			}
		}
	}
	
	void DebugComponent::OnLoad(MessagePtr message)
	{
		if(m_ShowNodeName)
		{
			std::string name = GetSceneObject()->GetName();
			MessagePtr text_mess(new TextCaptionMessage(name));
			GetSceneObject()->PostMessage(text_mess);
		}
	}
}
