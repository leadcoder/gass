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

#ifndef VEHICLE_CAMERA_COMPONENT_H
#define VEHICLE_CAMERA_COMPONENT_H

#include "Sim/Components/Graphics/Geometry/GASSIGeometryComponent.h"
#include "Sim/Components/GASSBaseSceneComponent.h"
#include "Sim/Scene/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Scene/GASSCoreSceneObjectMessages.h"
#include "Sim/Scene/GASSSceneObjectLink.h"
#include "Sim/Systems/Input/GASSControlSetting.h"
#include "Sim/GASSCommon.h"
#include "Plugins/Game/GameMessages.h"


namespace GASS
{
	class SceneObject;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr;

	class VehicleCameraComponent : public Reflection<VehicleCameraComponent,BaseSceneComponent>
	{
	public:
		VehicleCameraComponent();
		virtual ~VehicleCameraComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		void OnEnter(EnterVehicleMessagePtr message);
		void OnExit(ExitVehicleMessagePtr message);
		void OnLoad(LoadComponentsMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		void SetPreferredViewport(const std::string &viewport);
		std::string GetPreferredViewport() const;
	private:
		std::string m_PreferredViewport;
		ADD_ATTRIBUTE(SceneObjectLink,InputHandlerObject);
	};
	typedef boost::shared_ptr<VehicleCameraComponent> VehicleCameraComponentPtr;
}
#endif
