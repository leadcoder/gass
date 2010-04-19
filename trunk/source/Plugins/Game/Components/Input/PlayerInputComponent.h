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

#ifndef PLAYER_INPUT_COMPONENT_H
#define PLAYER_INPUT_COMPONENT_H

#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Scenario/Scene/SceneObjectMessages.h"
#include "Sim/Systems/Input/ControlSetting.h"
#include "Sim/Common.h"
#include "Plugins/Game/GameMessages.h"


namespace GASS
{
	class SceneObject;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr;

	class PlayerInputComponent : public Reflection<PlayerInputComponent,BaseSceneComponent>
	{
	public:
		PlayerInputComponent();
		virtual ~PlayerInputComponent();
		static void RegisterReflection();
		virtual void OnCreate();
		//void OnEnter(AnyMessagePtr message);
		//void OnExit(AnyMessagePtr message);
		void OnInput(ControllerMessagePtr message);
		//void OnLoad(LoadGameComponentsMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
	
	private:
		void SetControlSetting(const std::string &controlsetting);
		std::string GetControlSetting() const;

		std::string m_ControlSetting;

		SceneObjectPtr m_CurrentVehicle;
		SceneObjectPtr m_CurrentSeat;
	};
}
#endif
