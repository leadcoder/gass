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
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"

#include "Sim/GASSSimEngine.h"
#include "Sim/Interface/GASSIGraphicsSystem.h"



#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"

#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/Interface/GASSICameraComponent.h"
#include "Sim/Messages/GASSGraphicsSceneMessages.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"
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
		RegisterProperty<SceneObjectRef>("InputHandlerObject", &VehicleCameraComponent::GetInputHandlerObject, &VehicleCameraComponent::SetInputHandlerObject);
	}

	void VehicleCameraComponent::OnInitialize()
	{
		BaseSceneComponent::OnInitialize();
		
		m_InputHandlerObject->RegisterForMessage(REG_TMESS(VehicleCameraComponent::OnEnter,EnterVehicleMessage,0));
		m_InputHandlerObject->RegisterForMessage(REG_TMESS(VehicleCameraComponent::OnExit,ExitVehicleMessage,0));
		//GraphicsSystemPtr gfx_sys = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<IGraphicsSystem>();
	}

	void VehicleCameraComponent::OnDelete()
	{
		InputHandlerComponentPtr vehicle = GetSceneObject()->GetFirstParentComponentByClass<InputHandlerComponent>();
		if(vehicle)
		{
			vehicle->GetSceneObject()->UnregisterForMessage(UNREG_TMESS(VehicleCameraComponent::OnEnter,EnterVehicleMessage));
			vehicle->GetSceneObject()->UnregisterForMessage(UNREG_TMESS(VehicleCameraComponent::OnExit,ExitVehicleMessage));
		}
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
		CameraComponentPtr camera = GetSceneObject()->GetFirstComponentByClass<ICameraComponent>();
		MessagePtr cam_msg(new ChangeCameraRequest(camera,m_PreferredViewport));
		//SimEngine::Get().GetSimSystemManager()->SendImmediate(cam_msg);
	}

	void VehicleCameraComponent::OnExit(ExitVehicleMessagePtr message)
	{

	}

	

	

}
