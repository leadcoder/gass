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
#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/Scene/GASSScene.h"
#include "Sim/Scene/GASSSceneObject.h"
#include "Sim/Scene/GASSSceneObjectManager.h"

#include "Sim/Systems/Resource/GASSIResourceSystem.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "Sim/Scheduling/GASSIRuntimeController.h"
#include "Sim/Systems/Input/GASSControlSettingsManager.h"
#include "Sim/Systems/Input/GASSControlSetting.h"
#include "Sim/Components/Graphics/GASSICameraComponent.h"


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
		RegisterProperty<SceneObjectLink>("InputHandlerObject", &InputProxyComponent::GetInputHandlerObject, &InputProxyComponent::SetInputHandlerObject);
	}

	void InputProxyComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(InputProxyComponent::OnLoad,LoadGameComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(InputProxyComponent::OnUnload,UnloadComponentsMessage,0));
		BaseSceneComponent::OnCreate();
	}

	void InputProxyComponent::OnLoad(LoadGameComponentsMessagePtr message)
	{
		m_InputHandlerObject->RegisterForMessage(REG_TMESS(GASS::InputProxyComponent::OnPlayerInput,ControllerMessage,0));
	}

	void InputProxyComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
		boost::shared_static_cast<SceneObject>(GetSceneObject()->GetParent())->UnregisterForMessage(UNREG_TMESS(GASS::InputProxyComponent::OnPlayerInput,ControllerMessage));
	}

	void InputProxyComponent::OnPlayerInput(ControllerMessagePtr message)
	{
		GetSceneObject()->SendImmediate(message);
	}

}
