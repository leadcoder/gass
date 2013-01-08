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
		GetScene()->RegisterForMessage(REG_TMESS(RaknetNetworkSceneManager::OnLoad,LoadSceneManagersRequest,0));
		GetScene()->RegisterForMessage(REG_TMESS(RaknetNetworkSceneManager::OnUnload,UnLoadSceneManagersRequest,0));
		//GetScene()->RegisterForMessage(REG_TMESS(RaknetNetworkSceneManager::OnLoadSceneObject,SceneObjectCreatedNotifyMessage,Scene::PHYSICS_COMPONENT_LOAD_PRIORITY));
		
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(RaknetNetworkSceneManager::OnNewMasterReplica,MasterReplicaCreatedEvent,0));
	}

	/*void RaknetNetworkSceneManager::OnLoadSceneObject(SceneObjectCreatedNotifyMessagePtr message)
	{
		
	}*/

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
			SceneObjectPtr child = boost::shared_static_cast<SceneObject>(cc_iter.getNext());
			GeneratePartID(child,++id);
		}
	}

	void RaknetNetworkSceneManager::Update(double delta_time)
	{

		if (m_Paused)
			return;

		//do some time slicing
		m_TimeToProcess += delta_time;
		int num_steps = (int) (m_TimeToProcess / m_SimulationUpdateInterval);
		int clamp_num_steps = num_steps;

		//Take max 4 simulation step each frame
		if(num_steps > m_MaxSimSteps) clamp_num_steps = m_MaxSimSteps;

		for (int i = 0; i < clamp_num_steps; ++i)
		{

		}
		//std::cout << "Steps:" <<  clamp_num_steps << std::endl;
		m_TimeToProcess -= m_SimulationUpdateInterval * num_steps;




	}


	void RaknetNetworkSceneManager::OnLoad(LoadSceneManagersRequestPtr message)
	{
		ScenePtr scene = message->GetScene();
		//SimEngine::GetPtr()->GetRuntimeController()->Register(this);
		RakNetNetworkSystemPtr system =  SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<RakNetNetworkSystem>();
		if(system == NULL)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to find RakNetNetworkSystem", "RaketNetworkSceneManager::OnLoad");
		system->Register(shared_from_this());
	}

	void RaknetNetworkSceneManager::OnUnload(UnLoadSceneManagersRequestPtr message)
	{

		int address = (int) this;
		SimEngine::Get().GetSimSystemManager()->UnregisterForMessage(UNREG_TMESS(RaknetNetworkSceneManager::OnNewMasterReplica,MasterReplicaCreatedEvent));
		//SimEngine::GetPtr()->GetRuntimeController()->Unregister(this);
	}
}
