/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                        *
*                                                                           *
* Copyright (c) 2008-2015 GASS team. See Contributors.txt for details.      *
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

#include "GameSystem.h"
#include "Core/Common.h"
#include "Sim/GASSSystemFactory.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSceneManagerFactory.h"
#include "GameSceneManager.h"


namespace GASS
{
	GameSystem::GameSystem(void) //: m_Update(true)
	{
		m_UpdateGroup = UGID_SIM;
	}

	GameSystem::~GameSystem(void)
	{
	
	}

	void GameSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("GameSystem",new GASS::Creator<GameSystem, SimSystem>);
	}

	void GameSystem::Init()
	{
		SceneManagerFactory::GetPtr()->Register("GameSceneManager",new GASS::Creator<GameSceneManager, ISceneManager>);
		//SimEngine::Get().GetRuntimeController()->Register(shared_from_this(),m_TaskNodeName);
		//SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(GameSystem::OnScenarioEvent,SimEvent,0));
	}

	/*void GameSystem::OnScenarioEvent(SimEventPtr message)
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

	/*void GameSystem::Update(double delta_time, TaskNode2* caller)
	{
		if(m_Update)
			SimSystem::Update(delta_time,caller);
	}*/
}




