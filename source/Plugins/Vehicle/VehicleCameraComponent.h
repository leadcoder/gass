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

#ifndef VEHICLE_CAMERA_COMPONENT_H
#define VEHICLE_CAMERA_COMPONENT_H

#include "Sim/GASSCommon.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/GASSSceneObjectRef.h"
#include "Sim/Messages/GASSPlatformMessages.h"

namespace GASS
{
	class SceneObject;
	typedef GASS_SHARED_PTR<SceneObject> SceneObjectPtr;
	typedef GASS_WEAK_PTR<SceneObject> SceneObjectWeakPtr;

	class VehicleCameraComponent : public Reflection<VehicleCameraComponent,BaseSceneComponent>
	{
	public:
		VehicleCameraComponent();
		~VehicleCameraComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;

		void SetPreferredViewport(const std::string &viewport);
		std::string GetPreferredViewport() const;
	private:
		void OnEnter(EnterVehicleRequestPtr message);
		void OnExit(ExitVehicleRequestPtr message);

		std::string m_PreferredViewport;
		SceneObjectRef m_InputHandlerObject;
	};
	typedef GASS_SHARED_PTR<VehicleCameraComponent> VehicleCameraComponentPtr;
}
#endif
