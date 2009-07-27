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
#include "Core/Utils/Log.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/Message.h"
#include "Core/System/SystemFactory.h"
#include "Sim/Scenario/Scene/SceneManagerFactory.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Systems/SimSystemManager.h"


#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/SimEngine.h"
#include "Sim/Scheduling/IRuntimeController.h"

#include "Sim/Components/Graphics/Geometry/IMeshComponent.h"
#include "Plugins/ODE/ODEPhysicsSystem.h"
//#include "Main/Root.h"
#include "Plugins/ODE/ODEBody.h"


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
//		SimEngine::GetPtr()->GetRuntimeController()->Register(boost::bind( &ODEPhysicsSystem::Update, this, _1 ),m_PrimaryThread);
		GetSimSystemManager()->RegisterForMessage(SimSystemManager::SYSTEM_RM_INIT, MESSAGE_FUNC( ODEPhysicsSystem::OnInit));
	}

	void ODEPhysicsSystem::OnInit(MessagePtr message)
	{
		dInitODE2(0);
		//Load scenes
		dAllocateODEDataForThread(dAllocateMaskAll);
	}

	void ODEPhysicsSystem::OnShutdown(MessagePtr message)
	{
		dCloseODE();
	}







}
