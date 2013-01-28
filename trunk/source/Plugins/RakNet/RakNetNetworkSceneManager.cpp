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
#include "Core/Utils/GASSException.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/ComponentSystem/GASSBaseComponentContainerTemplateManager.h"
#include "Core/ComponentSystem/GASSComponentContainerFactory.h"
#include "Sim/GASSSceneManagerFactory.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"


#include "Sim/GASSSimEngine.h"


#include "Sim/Interface/GASSIMeshComponent.h"
#include "Sim/GASSSimSystemManager.h"
#include "Plugins/RakNet/RakNetNetworkSceneManager.h"
#include "Plugins/RakNet/RakNetNetworkMasterComponent.h"
#include "Plugins/RakNet/RakNetNetworkChildComponent.h"
#include "Plugins/RakNet/RakNetNetworkSystem.h"
#include "Plugins/RakNet/RakNetMasterReplica.h"




namespace GASS
{

	RaknetNetworkSceneManager::RaknetNetworkSceneManager() :
		m_Paused(false),
		m_SimulationUpdateInterval(1.0/60.0), //Locked to 60hz, if this value is changed the behavior of simulation is effected and values for bodies and joints must be retweeked
		m_TimeToProcess(0),
		m_MaxSimSteps(4)
	{

	}

	RaknetNetworkSceneManager::~RaknetNetworkSceneManager()
	{
	}

	void RaknetNetworkSceneManager::RegisterReflection()
	{
		//RegisterProperty<TaskGroup>("TaskGroup", &GASS::RaknetNetworkSceneManager::GetTaskGroup, &GASS::RaknetNetworkSceneManager::SetTaskGroup);
	}

	void RaknetNetworkSceneManager::OnCreate()
	{
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(RaknetNetworkSceneManager::OnNewMasterReplica,MasterReplicaCreatedEvent,0));
	}


	void RaknetNetworkSceneManager::OnNewMasterReplica(MasterReplicaCreatedEventPtr message)
	{
		RakNetMasterReplica* replica = message->GetReplica();
		std::string template_name = replica->GetTemplateName();
		SceneObjectPtr so = SimEngine::Get().CreateObjectFromTemplate(template_name);
		if(so)
		{
			RakNetNetworkMasterComponentPtr comp = so->GetFirstComponentByClass<RakNetNetworkMasterComponent>();
			comp->SetReplica(replica);
			GetScene()->GetRootSceneObject()->AddChildSceneObject(so,true);
		}
	}

	

	void RaknetNetworkSceneManager::GeneratePartID(SceneObjectPtr obj, int &id)
	{
		RakNetNetworkChildComponentPtr comp =  obj->GetFirstComponentByClass<RakNetNetworkChildComponent>();
		if(comp)
			comp->SetPartId(id);
		IComponentContainer::ComponentContainerIterator cc_iter = obj->GetChildren();
		while(cc_iter.hasMoreElements())
		{
			SceneObjectPtr child = STATIC_PTR_CAST<SceneObject>(cc_iter.getNext());
			GeneratePartID(child,++id);
		}
	}

	void RaknetNetworkSceneManager::OnInit()
	{
		RakNetNetworkSystemPtr system =  SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<RakNetNetworkSystem>();
		if(system == NULL)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to find RakNetNetworkSystem", "RaketNetworkSceneManager::OnLoad");
		system->Register(shared_from_this());
	}

	void RaknetNetworkSceneManager::OnShutdown()
	{
		int address = (int) this;
		SimEngine::Get().GetSimSystemManager()->UnregisterForMessage(UNREG_TMESS(RaknetNetworkSceneManager::OnNewMasterReplica,MasterReplicaCreatedEvent));
		//SimEngine::GetPtr()->GetRuntimeController()->Unregister(this);
	}
}
