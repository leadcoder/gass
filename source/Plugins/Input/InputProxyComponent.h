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

#ifndef INPUT_PROXY_COMPONENT_H
#define INPUT_PROXY_COMPONENT_H


#include "Sim/GASSCommon.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/GASSSceneObjectRef.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/Messages/GASSInputMessages.h"


namespace GASS
{
	class SceneObject;
	typedef GASS_SHARED_PTR<SceneObject> SceneObjectPtr;
	typedef GASS_WEAK_PTR<SceneObject> SceneObjectWeakPtr;

	/**
		Delegate input from user specified SceneObject to owner for this component 
	*/

	class InputProxyComponent : public Reflection<InputProxyComponent,BaseSceneComponent>
	{
	public:
		InputProxyComponent();
		virtual ~InputProxyComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();
		void OnPlayerInput(InputRelayEventPtr message);
	private:
		ADD_PROPERTY(SceneObjectRef,InputHandlerObject);
	};

	typedef GASS_SHARED_PTR<InputProxyComponent> InputProxyComponentPtr;
}
#endif
