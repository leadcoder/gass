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

#ifndef PRISMATIC_INTERACTION_COMPONENT_H
#define PRISMATIC_INTERACTION_COMPONENT_H


#include "Sim/GASSCommon.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/Messages/GASSInputMessages.h"


namespace GASS
{

	class SceneObject;
	typedef GASS_SHARED_PTR<SceneObject> SceneObjectPtr;
	typedef GASS_WEAK_PTR<SceneObject> SceneObjectWeakPtr;

	class PrismaticInteractionComponent :  public Reflection<PrismaticInteractionComponent,BaseSceneComponent>
	{
	public:
		PrismaticInteractionComponent();
		virtual ~PrismaticInteractionComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
	private:
		ADD_PROPERTY(std::string,InputMapping);
		ADD_PROPERTY(Float,MaxVelocity);
		void OnInput(InputRelayEventPtr message);
	};
}
#endif
