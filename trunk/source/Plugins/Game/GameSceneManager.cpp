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
#include "Plugins/Game/GameSceneManager.h"
#include "Plugins/Game/GameMessages.h"
#include "Core/Utils/Log.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Sim/Scenario/Scene/SceneManagerFactory.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"

#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/SimEngine.h"
#include "Sim/Scheduling/IRuntimeController.h"





namespace GASS
{
	GameSceneManager::GameSceneManager() :
		m_TaskGroup(MAIN_TASK_GROUP),
		m_Paused(false),
		m_SimulationUpdateInterval(1.0/60.0), //Locked to 60hz, if this value is changed the behavior of simulation is effected and values for bodies and joints must be retweeked
		m_TimeToProcess(0),
		m_MaxSimSteps(4)
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
		SimEngine::GetPtr()->GetRuntimeController()->Register(this);
		ScenarioScenePtr scene = GetScenarioScene();
		if(scene)
		{
			scene->RegisterForMessage(typeid(LoadSceneManagersMessage), MESSAGE_FUNC( GameSceneManager::OnLoad ));
			scene->RegisterForMessage(typeid(UnloadSceneManagersMessage), MESSAGE_FUNC( GameSceneManager::OnUnload ));
			scene->RegisterForMessage(typeid(SceneObjectCreatedNotifyMessage), MESSAGE_FUNC( GameSceneManager::OnLoadSceneObject),ScenarioScene::CORE_COMPONENT_LOAD_PRIORITY);
		}
	}
	
	void GameSceneManager::OnLoadSceneObject(MessagePtr message)
	{
		//Initlize all sim components and send scene mananger as argument
		SceneObjectCreatedNotifyMessagePtr socnm = boost::shared_static_cast<SceneObjectCreatedNotifyMessage>(message);
		assert(socnm);
		SceneObjectPtr obj = socnm->GetSceneObject();
		assert(obj);
		GameSceneManagerPtr core_sm = boost::dynamic_pointer_cast<GameSceneManager>(shared_from_this());
		MessagePtr sim_msg(new LoadGameComponentsMessage(core_sm,(int) this));
		obj->SendImmediate(sim_msg);
	}

	TaskGroup GameSceneManager::GetTaskGroup() const
	{
		return m_TaskGroup;
	}

	void GameSceneManager::Update(double delta_time)
	{
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

	void GameSceneManager::OnLoad(MessagePtr message)
	{
		//LoadSceneManagersMessagePtr lsmm = boost::shared_static_cast<LoadSceneManagersMessagePtr>(message); 
		m_Init = true;
	}

	void GameSceneManager::OnUnload(MessagePtr message)
	{
		SimEngine::GetPtr()->GetRuntimeController()->Unregister(this);
	}

	void GameSceneManager::SetTaskGroup(TaskGroup value)
	{
		m_TaskGroup = value;
	}
}
