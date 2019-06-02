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
#include "Sim/GASSSystemFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Sim/GASSSceneManagerFactory.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"

namespace GASS
{
	ODECollisionSystem::ODECollisionSystem(SimSystemManagerWeakPtr manager) : Reflection(manager)
	{
		m_UpdateGroup = UGID_PRE_SIM;
	}

	ODECollisionSystem::~ODECollisionSystem()
	{

	}
	
	void ODECollisionSystem::OnSystemInit()
	{
		ComponentFactory::GetPtr()->Register("ODECollisionGeometryComponent",new Creator<ODECollisionGeometryComponent, Component>);
		SceneManagerFactory::GetPtr()->Register<ODECollisionSceneManager>("ODECollisionSceneManager");

		SimSystemPtr system = SimEngine::Get().GetSimSystemManager()->GetSystemByName("ODEPhysicsSystem");
		if(!(system)) //check if ode physics system present, if not initialize ode
		{
			dInitODE2(0);
			dAllocateODEDataForThread(~0U);
		}
	}

	void ODECollisionSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register<ODECollisionSystem>("ODECollisionSystem");//,new GASS::Creator<ODECollisionSystem, SimSystem>);
	}
}
