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

#pragma once

#include "Sim/GASSCommon.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/GASSSceneObjectRef.h"
#include "Sim/Messages/GASSPlatformMessages.h"
#include "Sim/Interface/GASSICollisionSceneManager.h"

namespace GASS
{
	class SceneObject;
	using SceneObjectPtr = std::shared_ptr<SceneObject>;
	using SceneObjectWeakPtr = std::weak_ptr<SceneObject>;

	class ChaseCameraComponent : public Reflection<ChaseCameraComponent,BaseSceneComponent>
	{
	public:
		ChaseCameraComponent();
		~ChaseCameraComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;

		void SetPreferredViewport(const std::string &viewport);
		std::string GetPreferredViewport() const;
		void SceneManagerTick(double delta_time) override;
	private:
		void OnEnter(EnterVehicleRequestPtr message);
		void OnExit(ExitVehicleRequestPtr message);

		ILocationComponent* m_CameraLocation{nullptr};
		const ILocationComponent* m_ChaseObjectLocation{nullptr};
		ICollisionSceneManager* m_CollisionSM = nullptr;
		Vec3 m_Velocity;
		std::string m_PreferredViewport;
		SceneObjectRef m_InputHandlerObject;
		Float m_DampingConstant{8};
		Float m_SpringConstant{16};
		Float m_OffsetDistance{5.0};
		Float m_OffsetHeight{1.0};
	};
	using ChaseCameraComponentPtr = std::shared_ptr<ChaseCameraComponent>;
}
