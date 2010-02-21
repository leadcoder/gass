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
#include "Core/MessageSystem/IMessage.h"
#include "Sim/Scenario/Scene/SceneManagerFactory.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"

#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/SimEngine.h"
#include "Sim/Scheduling/IRuntimeController.h"

#include "Sim/Components/Graphics/Geometry/IMeshComponent.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Plugins/RakNet/RaknetNetworkSceneManager.h"



namespace GASS
{

	RaknetNetworkSceneManager::RaknetNetworkSceneManager() :
		m_Paused(false),
		m_TaskGroup(PHYSICS_TASK_GROUP),
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
		SceneManagerFactory::GetPtr()->Register("PhysicsSceneManager",new GASS::Creator<RaknetNetworkSceneManager, ISceneManager>);
		RegisterProperty<TaskGroup>("TaskGroup", &GASS::RaknetNetworkSceneManager::GetTaskGroup, &GASS::RaknetNetworkSceneManager::SetTaskGroup);
	}


	void RaknetNetworkSceneManager::OnCreate()
	{
		
		GetScenarioScene()->RegisterForMessage(REG_TMESS(RaknetNetworkSceneManager::OnLoad,LoadSceneManagersMessage,0));
		GetScenarioScene()->RegisterForMessage(REG_TMESS(RaknetNetworkSceneManager::OnUnload,UnloadSceneManagersMessage,0));
		GetScenarioScene()->RegisterForMessage(REG_TMESS(RaknetNetworkSceneManager::OnLoadSceneObject,SceneObjectCreatedNotifyMessage,ScenarioScene::PHYSICS_COMPONENT_LOAD_PRIORITY));
	}

	void RaknetNetworkSceneManager::OnLoadSceneObject(SceneObjectCreatedNotifyMessagePtr message)
	{
		//Initlize all network components and send scene mananger as argument
		SceneObjectPtr obj = message->GetSceneObject();
		assert(obj);
//		MessagePtr phy_msg(new LoadNetworkComponentsMessage(shared_from_this(),(int) this));
//		obj->SendImmediate(phy_msg);
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


	void RaknetNetworkSceneManager::OnLoad(LoadSceneManagersMessagePtr message)
	{
		ScenarioScenePtr scene = message->GetScenarioScene();

		SimEngine::GetPtr()->GetRuntimeController()->Register(this);

	

	
	}

	void RaknetNetworkSceneManager::OnUnload(UnloadSceneManagersMessagePtr message)
	{
		
		int address = (int) this;
		SimEngine::GetPtr()->GetRuntimeController()->Unregister(this);
	}


	void RaknetNetworkSceneManager::SetTaskGroup(TaskGroup value)
	{
		m_TaskGroup = value;
	}

	TaskGroup RaknetNetworkSceneManager::GetTaskGroup() const
	{
		return m_TaskGroup;
	}
}
