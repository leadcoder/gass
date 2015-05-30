/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                        *
*                                                                           *
* Copyright (c) 2008-2015 GASS team. See Contributors.txt for details.      *
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
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/GASSSceneObjectRef.h"
#include "Sim/Messages/GASSPlatformMessages.h"
#include "Sim/Messages/GASSInputMessages.h"


namespace GASS
{
	class SceneObject;
	typedef SPTR<SceneObject> SceneObjectPtr;
	typedef WPTR<SceneObject> SceneObjectWeakPtr;

	class ChaseCameraComponent : public Reflection<ChaseCameraComponent,BaseSceneComponent>
	{
	public:
		ChaseCameraComponent();
		virtual ~ChaseCameraComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();

		void SetPreferredViewport(const std::string &viewport);
		std::string GetPreferredViewport() const;
		void SceneManagerTick(double delta_time);
	private:
		void OnEnter(EnterVehicleRequestPtr message);
		void OnExit(ExitVehicleRequestPtr message);

		Vec3 m_Velocity;
		std::string m_PreferredViewport;
		ADD_PROPERTY(SceneObjectRef,InputHandlerObject);
		ADD_PROPERTY(Float,DampingConstant);
		ADD_PROPERTY(Float,SpringConstant);
		ADD_PROPERTY(Float,OffsetDistance);
		ADD_PROPERTY(Float,OffsetHeight);
	};
	typedef SPTR<ChaseCameraComponent> ChaseCameraComponentPtr;
}
