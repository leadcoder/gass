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

#ifndef PHYSICS_PROXY_COMPONENT_H
#define PHYSICS_PROXY_COMPONENT_H


#include "Sim/GASSCommon.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSSceneObjectRef.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Plugins/Game/GameMessages.h"


namespace GASS
{
	class PhysicsReportProxyComponent :  public Reflection<PhysicsReportProxyComponent,BaseSceneComponent>
	{
	public:
		PhysicsReportProxyComponent();
		virtual ~PhysicsReportProxyComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
	private:
		ADD_PROPERTY(SceneObjectRef,TargetObject);
		void OnHingeJointReport(PhysicsHingeJointReportEventPtr message);
	};
}
#endif
