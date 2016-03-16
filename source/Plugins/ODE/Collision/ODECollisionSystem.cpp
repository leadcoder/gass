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

#include "Plugins/ODE/Collision/ODECollisionSystem.h"
#include "Plugins/ODE/Collision/ODECollisionSceneManager.h"
#include "Plugins/ODE/Collision/ODECollisionGeometryComponent.h"
#include "Plugins/ODE/Collision/ODELineCollision.h"

#include "Sim/GASSSystemFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSException.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneManagerFactory.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Interface/GASSIMeshComponent.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Interface/GASSIMeshComponent.h"


namespace GASS
{
	ODECollisionSystem::ODECollisionSystem()  
	{
		 m_UpdateGroup=UGID_PRE_SIM;
	}

	ODECollisionSystem::~ODECollisionSystem()
	{

	}
	
	void ODECollisionSystem::Init()
	{
		ComponentFactory::GetPtr()->Register("ODECollisionGeometryComponent",new Creator<ODECollisionGeometryComponent, Component>);
		SceneManagerFactory::GetPtr()->Register("ODECollisionSceneManager",new GASS::Creator<ODECollisionSceneManager, ISceneManager>);

		//SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(ODECollisionSystem::OnSceneUnloaded,SceneUnloadedEvent,0));
		//SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(ODECollisionSystem::OnPreSceneCreate,PreSceneCreateEvent,0));
		//SimEngine::Get().GetRuntimeController()->Register(shared_from_this(),m_TaskNodeName);
		SimSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetSystemByName("ODEPhysicsSystem");
		if(!(system)) //check if ode physics system present, if not initialize ode
		{
			dInitODE2(0);
			dAllocateODEDataForThread(~0U);
		}
	}

	void ODECollisionSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("ODECollisionSystem",new GASS::Creator<ODECollisionSystem, SimSystem>);
	}
}
