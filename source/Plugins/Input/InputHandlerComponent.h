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

#ifndef INPUT_HANDLER_COMPONENT_H
#define INPUT_HANDLER_COMPONENT_H


#include "Sim/GASSCommon.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/GASSComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/Messages/GASSPlatformMessages.h"

namespace GASS
{
	class SceneObject;
	using SceneObjectPtr = std::shared_ptr<SceneObject>;
	using SceneObjectWeakPtr = std::weak_ptr<SceneObject>;

	/**
		Component used to delegate input from input system to this scene object.
		By listening to enter/exit messages input will only be delegated if user is "inside" object.
		Enter and exit messages are sent by the PlayerInputComponent that should be attached to the player object.
	*/

	class InputHandlerComponent : public Reflection<InputHandlerComponent,Component>
	{
	public:
		InputHandlerComponent();
		~InputHandlerComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;
	private:
		void OnEnter(EnterVehicleRequestPtr message);
		void OnExit(ExitVehicleRequestPtr message);
		void OnInput(ControllSettingsMessagePtr message);
		void SetControlSetting(const std::string &controlsetting);
		std::string GetControlSetting() const;
		std::string m_ControlSetting;
		bool m_Empty{true};
	};

	using InputHandlerComponentPtr = std::shared_ptr<InputHandlerComponent>;
}
#endif
