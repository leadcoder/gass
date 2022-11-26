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

#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSComponentFactory.h"
#include "Sim/GASSSystemFactory.h"
#include "Sim/GASSSceneManagerFactory.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"
#include "Plugins/ODE/ODEPhysicsSystem.h"
#include "Plugins/ODE/ODEPhysicsSceneManager.h"
#include "Plugins/ODE/ODEBodyComponent.h"
#include "Plugins/ODE/ODEBoxGeometryComponent.h"
#include "Plugins/ODE/ODECylinderGeometryComponent.h"
#include "Plugins/ODE/ODEHingeComponent.h"
#include "Plugins/ODE/ODEPlaneGeometryComponent.h"
#include "Plugins/ODE/ODESphereGeometryComponent.h"
#include "Plugins/ODE/ODESuspensionComponent.h"
#include "Plugins/ODE/ODETerrainGeometryComponent.h"
#include "Plugins/ODE/ODEMeshGeometryComponent.h"

namespace GASS
{

	ODEPhysicsSystem::ODEPhysicsSystem(SimSystemManagerWeakPtr manager) : Reflection(manager)  , m_Init(false)
	{
		m_UpdateGroup=UGID_SIM;
	}

	ODEPhysicsSystem::~ODEPhysicsSystem()
	{

	}

	void ODEPhysicsSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register<ODEPhysicsSystem>("ODEPhysicsSystem");
		
	}

	void ODEPhysicsSystem::OnSystemInit()
	{
		dInitODE2(0);
		dAllocateODEDataForThread(~0U);
		SceneManagerFactory::GetPtr()->Register<ODEPhysicsSceneManager>("ODEPhysicsSceneManager");
		ComponentFactory::Get().Register<ODEBodyComponent>("PhysicsBodyComponent");
		ComponentFactory::Get().Register<ODEBoxGeometryComponent>("PhysicsBoxGeometryComponent");
		ComponentFactory::Get().Register<ODECylinderGeometryComponent>("PhysicsCylinderGeometryComponent");
		ComponentFactory::Get().Register<ODEHingeComponent>("PhysicsHingeComponent");
		ComponentFactory::Get().Register<ODEMeshGeometryComponent>("PhysicsMeshGeometryComponent");
		ComponentFactory::Get().Register<ODEPlaneGeometryComponent>("PhysicsPlaneGeometryComponent");
		ComponentFactory::Get().Register<ODESphereGeometryComponent>("PhysicsSphereGeometryComponent");
		ComponentFactory::Get().Register<ODESuspensionComponent>("PhysicsSuspensionComponent");
		ComponentFactory::Get().Register<ODETerrainGeometryComponent>("PhysicsTerrainGeometryComponent");
	}

	void ODEPhysicsSystem::OnSystemShutdown()
	{
		dCloseODE();
	}
}
