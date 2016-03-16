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

#include "GASSSimulationSystem.h"
#include "Core/Common.h"
#include "Sim/GASSSystemFactory.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSceneManagerFactory.h"
#include "GASSSimulationSceneManager.h"


namespace GASS
{
	SimulationSystem::SimulationSystem(void) //: m_Update(true)
	{
		m_UpdateGroup = UGID_SIM;
	}

	SimulationSystem::~SimulationSystem(void)
	{

	}

	void SimulationSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("SimulationSystem",new GASS::Creator<SimulationSystem, SimSystem>);
	}

	void SimulationSystem::Init()
	{
		SceneManagerFactory::GetPtr()->Register("SimulationSceneManager",new GASS::Creator<SimulationSceneManager, ISceneManager>);
		//SimEngine::Get().GetRuntimeController()->Register(shared_from_this(),m_TaskNodeName);
		//SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(SimulationSystem::OnScenarioEvent,SimEvent,0));
	}

	/*void SimulationSystem::OnScenarioEvent(SimEventPtr message)
	{
		if(message->GetEventType() == SET_PLAY)
		{
			m_Update = true;
		}
		else if(message->GetEventType() == SET_STOP)
		{
			m_Update = false;
		}
	}*/

	/*void SimulationSystem::Update(double delta_time, TaskNode2* caller)
	{
		if(m_Update)
			SimSystem::Update(delta_time,caller);
	}*/
}




