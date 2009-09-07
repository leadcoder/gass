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
#include "Sim/Scenario/Scene/SimSceneManager.h"
#include "Core/Utils/Log.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/Message.h"
#include "Sim/Scenario/Scene/SceneManagerFactory.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"

#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/SimEngine.h"
#include "Sim/Scheduling/IRuntimeController.h"





namespace GASS
{

	SimSceneManager::SimSceneManager() :
		m_TaskGroup(MAIN_TASK_GROUP),
		m_Paused(false),
		m_SimulationUpdateInterval(1.0/60.0), //Locked to 60hz, if this value is changed the behavior of simulation is effected and values for bodies and joints must be retweeked
		m_TimeToProcess(0),
		m_MaxSimSteps(4)
	{

	}

	SimSceneManager::~SimSceneManager()
	{
	}

	void SimSceneManager::RegisterReflection()
	{
		SceneManagerFactory::GetPtr()->Register("SimSceneManager",new GASS::Creator<SimSceneManager, ISceneManager>);
		//RegisterProperty<bool>( "PrimaryThread", &GASS::SimSceneManager::GetPrimaryThread, &GASS::SimSceneManager::SetPrimaryThread);
	}

	void SimSceneManager::OnCreate()
	{
		SimEngine::GetPtr()->GetRuntimeController()->Register(this);
		m_Scene->RegisterForMessage(ScenarioScene::SCENARIO_RM_LOAD_SCENE_MANAGERS, MESSAGE_FUNC( SimSceneManager::OnLoad ));
		m_Scene->RegisterForMessage(ScenarioScene::SCENARIO_RM_UNLOAD_SCENE_MANAGERS, MESSAGE_FUNC( SimSceneManager::OnUnload ));
		m_Scene->RegisterForMessage(ScenarioScene::SCENARIO_NM_SCENE_OBJECT_CREATED, MESSAGE_FUNC( SimSceneManager::OnLoadSceneObject),ScenarioScene::SIM_COMPONENT_LOAD_PRIORITY);
	}

	void SimSceneManager::OnLoadSceneObject(MessagePtr message)
	{
		//Initlize all sim components and send scene mananger as argument
		SceneObjectPtr obj = boost::any_cast<SceneObjectPtr>(message->GetData("SceneObject"));
		assert(obj);
		MessagePtr sim_msg(new Message(SceneObject::OBJECT_RM_LOAD_SIM_COMPONENTS,(int) this));
		sim_msg->SetData("SimSceneManager",boost::any(this));
		obj->SendImmediate(sim_msg);
	}

	TaskGroup SimSceneManager::GetTaskGroup() const
	{
		return m_TaskGroup;
	}


	void SimSceneManager::Update(double delta_time)
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

	void SimSceneManager::OnLoad(MessagePtr message)
	{
		ScenarioScene* scene = boost::any_cast<ScenarioScene*>(message->GetData("ScenarioScene"));
		m_Init = true;
	}

	void SimSceneManager::OnUnload(MessagePtr message)
	{
		SimEngine::GetPtr()->GetRuntimeController()->Unregister(this);
	}

	void SimSceneManager::SetTaskGroup(TaskGroup value)
	{
		m_TaskGroup = value;
	}

}
