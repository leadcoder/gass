/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                 *
*                                                                           *
* Copyright (c) 2008-2009 GASS team. See Contributors.txt for details.      *
*                                                                           *
* GASS is free software: you can redistribute it and/or modify              *
* it under the terms of the GNU Lesser General Public License as published  *
* by the Free Software Foundation, either version 3 of the License, or      *
* (at your option) any later version.                                       *
*                                                                           *
* GASS is distributed in the hope that it will be useful,                   *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU Lesser General Public License for more details.                       *
*                                                                           *
* You should have received a copy of the GNU Lesser General Public License  *
* along with GASS. If not, see <http://www.gnu.org/licenses/>.              *
*****************************************************************************/


#include "DebugComponent.h"
#include "Plugins/Base/CoreMessages.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/GASSSimEngine.h"

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

	void DebugComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(typeid(SceneObjectNameMessage),MESSAGE_FUNC(DebugComponent::OnChangeName),0);
		GetSceneObject()->RegisterForMessage(typeid(DebugComponentSettingsMessage),MESSAGE_FUNC(DebugComponent::OnSettings),0);

		if(m_ShowNodeName)
		{
			std::string name = GetSceneObject()->GetName();
			GetSceneObject()->PostRequest(TextCaptionMessagePtr(new TextCaptionMessage(name)));
		}
	}

	bool DebugComponent::GetShowNodeName() const
	{
		return m_ShowNodeName;
	}

	void DebugComponent::SetShowNodeName(bool value)
	{
		m_ShowNodeName = value;
		if(GetSceneObject())
		{
			if(m_ShowNodeName)
			{
				std::string name = GetSceneObject()->GetName();
				GetSceneObject()->PostRequest(TextCaptionMessagePtr(new TextCaptionMessage(name)));
			}
			else
			{
				std::string name = "";
				GetSceneObject()->PostRequest(TextCaptionMessagePtr(new TextCaptionMessage(name)));
			}
		}
	}
	
	void DebugComponent::OnChangeName(GASS::MessagePtr message)
	{
		if(m_ShowNodeName)
		{
			SceneObjectNameMessagePtr name_mess = DYNAMIC_PTR_CAST<SceneObjectNameMessage>(message);
			if(name_mess)
			{
				std::string name = name_mess->GetName();
				GetSceneObject()->PostRequest(TextCaptionMessagePtr(new TextCaptionMessage(name)));
			}
		}
	}

	void DebugComponent::OnSettings(GASS::MessagePtr message)
	{
		DebugComponentSettingsMessagePtr settings_mess = DYNAMIC_PTR_CAST<DebugComponentSettingsMessage>(message);
		if(settings_mess)
		{
			SetShowNodeName(settings_mess->GetShowObjectName());
		}
	}
}
