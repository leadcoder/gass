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

#include "PhysicsReportProxyComponent.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSSceneObject.h"

namespace GASS
{
	PhysicsReportProxyComponent::PhysicsReportProxyComponent() 
	{

	}

	PhysicsReportProxyComponent::~PhysicsReportProxyComponent()
	{

	}

	void PhysicsReportProxyComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register<PhysicsReportProxyComponent>();
		RegisterMember("TargetObject", &PhysicsReportProxyComponent::m_TargetObject);
	}

	void PhysicsReportProxyComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(PhysicsReportProxyComponent::OnHingeJointReport,PhysicsHingeJointReportEvent,0));
	}

	void PhysicsReportProxyComponent::OnHingeJointReport(PhysicsHingeJointReportEventPtr message)
	{
		if(m_TargetObject.IsValid())
			m_TargetObject->SendImmediateEvent(message);
	}
}
