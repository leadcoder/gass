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

#include "VehicleCameraComponent.h"
#include "Sim/Messages/GASSPlatformMessages.h"
#include "Sim/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/Interface/GASSICameraComponent.h"
#include "Sim/Interface/GASSIInputComponent.h"
#include "Sim/Messages/GASSGraphicsSceneMessages.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"

namespace GASS
{
	VehicleCameraComponent::VehicleCameraComponent() : m_PreferredViewport("")
	{

	}

	VehicleCameraComponent::~VehicleCameraComponent()
	{

	}

	void VehicleCameraComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register<VehicleCameraComponent>();
		RegisterGetSet("PreferredViewport", &VehicleCameraComponent::GetPreferredViewport, &VehicleCameraComponent::SetPreferredViewport);
		RegisterMember("InputHandlerObject", &VehicleCameraComponent::m_InputHandlerObject);
	}

	void VehicleCameraComponent::OnInitialize()
	{
		Component::InitializeSceneObjectRef();
		Component::OnInitialize();

		if(!m_InputHandlerObject.IsValid())
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"No InputHandlerObject found", " VehicleCameraComponent::OnInitialize");

		m_InputHandlerObject->RegisterForMessage(REG_TMESS(VehicleCameraComponent::OnEnter,EnterVehicleRequest,0));
		m_InputHandlerObject->RegisterForMessage(REG_TMESS(VehicleCameraComponent::OnExit,ExitVehicleRequest,0));
		//GraphicsSystemPtr gfx_sys = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<IGraphicsSystem>();
	}

	void VehicleCameraComponent::OnDelete()
	{
		auto input = GASS_DYNAMIC_PTR_CAST<Component>(GetSceneObject()->GetFirstParentComponentByClass<IInputComponent>());
		if(input)
		{
			input->GetSceneObject()->UnregisterForMessage(UNREG_TMESS(VehicleCameraComponent::OnEnter,EnterVehicleRequest));
			input->GetSceneObject()->UnregisterForMessage(UNREG_TMESS(VehicleCameraComponent::OnExit,ExitVehicleRequest));
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

	void VehicleCameraComponent::OnEnter(EnterVehicleRequestPtr message)
	{
		CameraComponentPtr camera = GetSceneObject()->GetFirstComponentByClass<ICameraComponent>();
		camera->ShowInViewport(m_PreferredViewport);
	}

	void VehicleCameraComponent::OnExit(ExitVehicleRequestPtr message)
	{

	}
}
