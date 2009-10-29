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

#include "InputProxyComponent.h"
#include "GameMessages.h"
#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/MessageSystem/AnyMessage.h"
#include "Core/Utils/Log.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"

#include "Sim/Systems/Resource/IResourceSystem.h"
#include "Sim/SimEngine.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Scheduling/IRuntimeController.h"
#include "Sim/Systems/Input/ControlSettingsManager.h"
#include "Sim/Systems/Input/ControlSetting.h"
#include "Sim/Components/Graphics/ICameraComponent.h"


namespace GASS
{
	InputProxyComponent::InputProxyComponent() 
	{

	}

	InputProxyComponent::~InputProxyComponent()
	{

	}
	
	void InputProxyComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("InputProxyComponent",new Creator<InputProxyComponent, IComponent>);
		RegisterProperty<std::string>("InputHandler", &GetInputHandler, &SetInputHandler);
	}

	void InputProxyComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(OBJECT_RM_UNLOAD_COMPONENTS, MESSAGE_FUNC(InputProxyComponent::OnUnload));
		GetSceneObject()->RegisterForMessage(OBJECT_RM_LOAD_SIM_COMPONENTS, TYPED_MESSAGE_FUNC(InputProxyComponent::OnLoad,LoadSimComponentsMessage));
	}

	void InputProxyComponent::OnLoad(LoadSimComponentsMessagePtr message)
	{
		boost::shared_static_cast<SceneObject>(GetSceneObject()->GetParent())->RegisterForMessage((SceneObjectMessage)OBJECT_NM_PLAYER_INPUT, TYPED_MESSAGE_FUNC(GASS::InputProxyComponent::OnPlayerInput,AnyMessage));
	}

	void InputProxyComponent::OnUnload(MessagePtr message)
	{
		boost::shared_static_cast<SceneObject>(GetSceneObject()->GetParent())->UnregisterForMessage((SceneObjectMessage)OBJECT_NM_PLAYER_INPUT, TYPED_MESSAGE_FUNC(GASS::InputProxyComponent::OnPlayerInput,AnyMessage));
	}

	void InputProxyComponent::OnPlayerInput(AnyMessagePtr message)
	{
		GetSceneObject()->SendImmediate(message);	
	}

	void InputProxyComponent::SetInputHandler(const std::string &handler)
	{
		m_InputHandler = handler;
	}

	std::string InputProxyComponent::GetInputHandler() const
	{
		return m_InputHandler;
	}

}
