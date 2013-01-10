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
#include "AISceneManager.h"
#include "AISystem.h"
#include "Sim/GASS.h"


namespace GASS
{
	AISceneManager::AISceneManager() : m_Paused(false)
	{

	}

	AISceneManager::~AISceneManager()
	{

	}

	void AISceneManager::RegisterReflection()
	{
		SceneManagerFactory::GetPtr()->Register("AISceneManager",new GASS::Creator<AISceneManager, ISceneManager>);
		REG_ATTRIBUTE(bool,Paused,AISceneManager)
	}

	void AISceneManager::OnCreate()
	{
		ScenePtr scene = GetScene();
		if(scene)
		{
			scene->RegisterForMessage(REG_TMESS(AISceneManager::OnLoad ,LoadSceneManagersRequest,0));
			scene->RegisterForMessage(REG_TMESS(AISceneManager::OnUnload ,UnLoadSceneManagersRequest,0));
		}
		AISystemPtr system =  SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<AISystem>();
		//AISystem not found, cast exception
		if(!system)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to find AISystem", "AISceneManager::OnCreate");

		SystemListenerPtr listener = shared_from_this();
		system->Register(listener);
	}

	void AISceneManager::OnLoad(MessagePtr message)
	{
		
	}

	void AISceneManager::OnUnload(MessagePtr message)
	{

	}

	void AISceneManager::SystemTick(double delta_time) 
	{
		if(!m_Paused)
			BaseSceneManager::SystemTick(delta_time);
	}
}
