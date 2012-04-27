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

#include "VehicleCameraComponent.h"
#include "GameMessages.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/Scene/GASSScene.h"
#include "Sim/Scene/GASSSceneObject.h"
#include "Sim/Scene/GASSSceneObjectManager.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Systems/Graphics/GASSIGraphicsSystem.h"


#include "Sim/Systems/Resource/GASSIResourceSystem.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "Sim/Scheduling/GASSIRuntimeController.h"
#include "Sim/Systems/Input/GASSControlSettingsManager.h"
#include "Sim/Systems/Input/GASSControlSetting.h"
#include "Sim/Components/Graphics/GASSICameraComponent.h"
#include "Sim/Scene/GASSGraphicsSceneMessages.h"
#include "Plugins/Game/Components/Input/InputHandlerComponent.h"




namespace GASS
{
	VehicleCameraComponent::VehicleCameraComponent() : m_PreferredViewport("ALL")
	{

	}

	VehicleCameraComponent::~VehicleCameraComponent()
	{

	}

	void VehicleCameraComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("VehicleCameraComponent",new Creator<VehicleCameraComponent, IComponent>);
		RegisterProperty<std::string>("PreferredViewport", &VehicleCameraComponent::GetPreferredViewport, &VehicleCameraComponent::SetPreferredViewport);
		RegisterProperty<SceneObjectLink>("InputHandlerObject", &VehicleCameraComponent::GetInputHandlerObject, &VehicleCameraComponent::SetInputHandlerObject);
	}

	void VehicleCameraComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(VehicleCameraComponent::OnUnload,UnloadComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(VehicleCameraComponent::OnLoad,LoadGameComponentsMessage,0));
		BaseSceneComponent::OnCreate();
	}

	void VehicleCameraComponent::SetPreferredViewport(const std::string &viewport)
	{
		m_PreferredViewport = viewport;
	}

	std::string VehicleCameraComponent::GetPreferredViewport() const
	{
		return m_PreferredViewport;
	}

	void VehicleCameraComponent::OnEnter(EnterVehicleMessagePtr message)
	{
		MessagePtr cam_msg(new ChangeCameraMessage(GetSceneObject(),m_PreferredViewport));
		//GetSceneObject()->GetSceneObjectManager()->GetScene()->SendImmediate(cam_msg);
	}

	void VehicleCameraComponent::OnExit(ExitVehicleMessagePtr message)
	{

	}

	void VehicleCameraComponent::OnLoad(LoadGameComponentsMessagePtr message)
	{
		
		m_InputHandlerObject->RegisterForMessage(REG_TMESS(VehicleCameraComponent::OnEnter,EnterVehicleMessage,0));
		m_InputHandlerObject->RegisterForMessage(REG_TMESS(VehicleCameraComponent::OnExit,ExitVehicleMessage,0));
		GraphicsSystemPtr gfx_sys = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<IGraphicsSystem>();
	}

	void VehicleCameraComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
		InputHandlerComponentPtr vehicle = GetSceneObject()->GetFirstParentComponentByClass<InputHandlerComponent>();
		if(vehicle)
		{
			vehicle->GetSceneObject()->UnregisterForMessage(UNREG_TMESS(VehicleCameraComponent::OnEnter,EnterVehicleMessage));
			vehicle->GetSceneObject()->UnregisterForMessage(UNREG_TMESS(VehicleCameraComponent::OnExit,ExitVehicleMessage));
		}
	}

}
