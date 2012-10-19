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
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSBaseComponentContainerTemplateManager.h"
#include "Core/System/GASSSystemFactory.h"
#include "Sim/GASSSceneManagerFactory.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Scheduling/GASSIRuntimeController.h"
#include "Sim/Interface/GASSIMeshComponent.h"
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

	void ODEPhysicsSystem::Init()
	{
		SimEngine::Get().GetRuntimeController()->Register(shared_from_this(),m_TaskNodeName);

		SceneManagerFactory::GetPtr()->Register("PhysicsSceneManager",new GASS::Creator<ODEPhysicsSceneManager, ISceneManager>);
		ComponentFactory::GetPtr()->Register("PhysicsBodyComponent",new Creator<ODEBodyComponent, IComponent>);
		ComponentFactory::GetPtr()->Register("PhysicsBoxGeometryComponent",new Creator<ODEBoxGeometryComponent, IComponent>);
		ComponentFactory::GetPtr()->Register("PhysicsCylinderGeometryComponent",new Creator<ODECylinderGeometryComponent, IComponent>);
		ComponentFactory::GetPtr()->Register("PhysicsHingeComponent",new Creator<ODEHingeComponent, IComponent>);
		ComponentFactory::GetPtr()->Register("PhysicsMeshGeometryComponent",new Creator<ODEMeshGeometryComponent, IComponent>);
		ComponentFactory::GetPtr()->Register("PhysicsPlaneGeometryComponent",new Creator<ODEPlaneGeometryComponent, IComponent>);
		ComponentFactory::GetPtr()->Register("PhysicsSphereGeometryComponent",new Creator<ODESphereGeometryComponent, IComponent>);
		ComponentFactory::GetPtr()->Register("PhysicsSuspensionComponent",new Creator<ODESuspensionComponent, IComponent>);
		ComponentFactory::GetPtr()->Register("PhysicsTerrainGeometryComponent",new Creator<ODETerrainGeometryComponent, IComponent>);
		
	
		dInitODE2(0);
		dAllocateODEDataForThread(dAllocateMaskAll);
	}

	void ODEPhysicsSystem::OnShutdown(MessagePtr message)
	{
		dCloseODE();
	}
}
