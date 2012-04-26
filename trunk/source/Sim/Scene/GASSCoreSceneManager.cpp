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
#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSException.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/Scene/GASSSceneManagerFactory.h"
#include "Sim/Scene/GASSScene.h"
#include "Sim/Scene/GASSSceneObject.h"
#include "Sim/Scene/GASSCoreSceneManager.h"
#include "Sim/Scene/GASSSceneObjectManager.h"
#include "Sim/Systems/GASSCoreSystem.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Scheduling/GASSIRuntimeController.h"

namespace GASS
{
	CoreSceneManager::CoreSceneManager() 
	{

	}

	CoreSceneManager::~CoreSceneManager()
	{

	}

	void CoreSceneManager::RegisterReflection()
	{
		SceneManagerFactory::GetPtr()->Register("CoreSceneManager",new GASS::Creator<CoreSceneManager, ISceneManager>);
	}

	void CoreSceneManager::OnCreate()
	{
		CoreSystemPtr system =  SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<CoreSystem>();
		
		//core system not found, cast exception
		if(!system)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to find CoreSystem", "CoreSceneManager::OnCreate");
		
			SystemListenerPtr listener = shared_from_this();
		system->Register(listener);
		ScenePtr scenario = GetScene();
		if(scenario)
		{
			scenario->RegisterForMessage(typeid(LoadSceneManagersMessage), MESSAGE_FUNC( CoreSceneManager::OnLoad ));
			scenario->RegisterForMessage(typeid(SceneObjectCreatedNotifyMessage), MESSAGE_FUNC( CoreSceneManager::OnLoadSceneObject),Scene::CORE_COMPONENT_LOAD_PRIORITY);
		}
	}

	void CoreSceneManager::OnLoadSceneObject(MessagePtr message)
	{
		//Initlize all sim components and send scene mananger as argument
		SceneObjectCreatedNotifyMessagePtr socnm = boost::shared_static_cast<SceneObjectCreatedNotifyMessage>(message);
		assert(socnm);
		SceneObjectPtr obj = socnm->GetSceneObject();
		assert(obj);
		CoreSceneManagerPtr core_sm = boost::dynamic_pointer_cast<CoreSceneManager>(shared_from_this());
		MessagePtr sim_msg(new LoadCoreComponentsMessage(core_sm,(int) this));
		obj->SendImmediate(sim_msg);
	}

	void CoreSceneManager::OnLoad(MessagePtr message)
	{
		//LoadSceneManagersMessagePtr lsmm = boost::shared_static_cast<LoadSceneManagersMessagePtr>(message);
		m_Init = true;
 	}
}
