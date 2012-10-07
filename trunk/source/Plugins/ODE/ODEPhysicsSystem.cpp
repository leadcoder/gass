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
#include <boost/bind.hpp>
#include "Core/Utils/GASSLogManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/System/GASSSystemFactory.h"
#include "Sim/Scene/GASSSceneManagerFactory.h"
#include "Sim/Scene/GASSScene.h"
#include "Sim/Scene/GASSSceneObject.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Scheduling/GASSIRuntimeController.h"
#include "Sim/Components/Graphics/Geometry/GASSIMeshComponent.h"
#include "Plugins/ODE/ODEPhysicsSystem.h"
#include "Plugins/ODE/ODEPhysicsSceneManager.h"
#include "Plugins/ODE/ODEBodyComponent.h"


namespace GASS
{

	ODEPhysicsSystem::ODEPhysicsSystem() 
	{

	}

	ODEPhysicsSystem::~ODEPhysicsSystem()
	{

	}

	void ODEPhysicsSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("ODEPhysicsSystem",new GASS::Creator<ODEPhysicsSystem, ISystem>);
	}

	void ODEPhysicsSystem::OnCreate()
	{
		SceneManagerFactory::GetPtr()->Register("PhysicsSceneManager",new GASS::Creator<ODEPhysicsSceneManager, ISceneManager>);
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(ODEPhysicsSystem::OnInit,InitSystemMessage,0));
	}

	void ODEPhysicsSystem::OnInit(MessagePtr message)
	{
		dInitODE2(0);
		dAllocateODEDataForThread(dAllocateMaskAll);
	}

	void ODEPhysicsSystem::OnShutdown(MessagePtr message)
	{
		dCloseODE();
	}
}
