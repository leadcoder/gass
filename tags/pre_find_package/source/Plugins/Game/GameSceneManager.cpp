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

#include "GameSceneManager.h"
#include "GameMessages.h"
#include "GameSystem.h"
#include "Plugins/Game/GameMessages.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSException.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSSceneManagerFactory.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"

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
	}

	void GameSceneManager::OnCreate()
	{
	}
	
	void GameSceneManager::OnInit()
	{
		GameSystemPtr system =  SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<GameSystem>();
		SystemListenerPtr listener = shared_from_this();
		system->Register(listener);
	}

	void GameSceneManager::OnShutdown()
	{

	}
	
}
