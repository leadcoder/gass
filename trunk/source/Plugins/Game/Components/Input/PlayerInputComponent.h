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

#include "Sim/Components/Graphics/Geometry/GASSIGeometryComponent.h"
#include "Sim/Components/GASSBaseSceneComponent.h"
#include "Sim/Scene/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Scene/GASSCoreSceneObjectMessages.h"
#include "Sim/Systems/Input/GASSIControlSettingsSystem.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "Sim/GASSCommon.h"
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
		virtual void OnInitialize();
		void OnInput(ControllSettingsMessagePtr message);
		void OnDelete();
	private:
		void SetControlSetting(const std::string &controlsetting);
		std::string GetControlSetting() const;
		std::string m_ControlSetting;
		SceneObjectPtr m_CurrentVehicle;
		SceneObjectPtr m_CurrentSeat;
	};
}
#endif
