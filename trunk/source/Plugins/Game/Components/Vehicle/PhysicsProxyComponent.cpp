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

#include "PhysicsProxyComponent.h"
#include "GameMessages.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSSoundSceneObjectMessages.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Plugins/Game/GameSceneManager.h"


#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"


namespace GASS
{
	PhysicsProxyComponent::PhysicsProxyComponent() 
	{

	}

	PhysicsProxyComponent::~PhysicsProxyComponent()
	{

	}

	void PhysicsProxyComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PhysicsProxyComponent",new Creator<PhysicsProxyComponent, IComponent>);
	}

	void PhysicsProxyComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysicsProxyComponent::OnForceEvent,PhysicsJointForceEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysicsProxyComponent::OnVelocityEvent,PhysicsJointVelocityEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysicsProxyComponent::OnHingeVelocity,PhysicsHingeJointVelocityRequest,0));
	}

	void PhysicsProxyComponent::OnVelocityEvent(PhysicsJointVelocityEventPtr message)
	{
		
	}

	void PhysicsProxyComponent::OnForceEvent(PhysicsJointForceEventPtr message)
	{

	}

	void PhysicsProxyComponent::OnHingeVelocity(PhysicsHingeJointVelocityRequestPtr message)
	{

	}
}
