/****************************************************************************
* This file is part of GASS.                                                *
* See https://github.com/leadcoder/gass                                     *
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
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
#include "Sim/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"

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
		ComponentFactory::Get().Register<DebugComponent>();
		RegisterGetSet("ShowSceneObjectName", &GASS::DebugComponent::GetShowNodeName, &GASS::DebugComponent::SetShowNodeName);
	}

	void DebugComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(DebugComponent::OnChangeName, SceneObjectNameChangedEvent, 0));
	
		if(m_ShowNodeName)
		{
			std::string name = GetSceneObject()->GetName();
			GetSceneObject()->PostRequest(TextCaptionRequestPtr(new TextCaptionRequest(name)));
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
				GetSceneObject()->PostRequest(TextCaptionRequestPtr(new TextCaptionRequest(name)));
			}
			else
			{
				std::string name = "";
				GetSceneObject()->PostRequest(TextCaptionRequestPtr(new TextCaptionRequest(name)));
			}
		}
	}
	
	void DebugComponent::OnChangeName(SceneObjectNameChangedEventPtr event)
	{
		if(m_ShowNodeName)
		{
			std::string name = event->GetName();
			GetSceneObject()->PostRequest(TextCaptionRequestPtr(new TextCaptionRequest(event->GetName())));
		}
	}
}
