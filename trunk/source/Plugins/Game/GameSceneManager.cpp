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
#include "GameSceneManager.h"
#include "GameMessages.h"
#include "Plugins/Game/GameMessages.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/Scene/GASSSceneManagerFactory.h"
#include "Sim/Scene/GASSScene.h"
#include "Sim/Scene/GASSSceneObject.h"
#include "Sim/Systems/GASSCoreSystem.h"

#include "Sim/Scene/GASSSceneObjectManager.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "Sim/Scheduling/GASSIRuntimeController.h"





namespace GASS
{
	GameSceneManager::GameSceneManager() 
	{

	}

	GameSceneManager::~GameSceneManager()
	{

	}

	void GameSceneManager::RegisterReflection()
	{
		SceneManagerFactory::GetPtr()->Register("GameSceneManager",new GASS::Creator<GameSceneManager, ISceneManager>);
	}

	void GameSceneManager::OnCreate()
	{
		ScenePtr scene = GetScene();
		if(scene)
		{
			scene->RegisterForMessage(typeid(LoadSceneManagersMessage), MESSAGE_FUNC( GameSceneManager::OnLoad ));
			scene->RegisterForMessage(typeid(UnloadSceneManagersMessage), MESSAGE_FUNC( GameSceneManager::OnUnload ));
			scene->RegisterForMessage(typeid(SceneObjectCreatedNotifyMessage), MESSAGE_FUNC( GameSceneManager::OnLoadSceneObject),Scene::CORE_COMPONENT_LOAD_PRIORITY);
		}
	}

	void GameSceneManager::OnLoadSceneObject(MessagePtr message)
	{
		//Initlize all sim components and send scene mananger as argument
		SceneObjectCreatedNotifyMessagePtr socnm = boost::shared_static_cast<SceneObjectCreatedNotifyMessage>(message);
		assert(socnm);
		SceneObjectPtr obj = socnm->GetSceneObject();
		assert(obj);
		GameSceneManagerPtr game_sm = boost::dynamic_pointer_cast<GameSceneManager>(shared_from_this());
		MessagePtr sim_msg(new LoadGameComponentsMessage(game_sm,(int) this));
		obj->SendImmediate(sim_msg);
	}

	void GameSceneManager::OnLoad(MessagePtr message)
	{
		CoreSystemPtr system =  SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<CoreSystem>();
		SystemListenerPtr listener = shared_from_this();
		system->Register(listener);
	}


	void GameSceneManager::OnUnload(MessagePtr message)
	{

	}
	
}
