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
#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSException.h"
#include "Sim/GASSSceneManagerFactory.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"
#include "GASSCoreSceneManager.h"
#include "GASSCoreSystem.h"

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
		CoreSystemPtr system =  SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<CoreSystem>();
		SystemListenerPtr listener = shared_from_this();
		system->Register(listener);
	}

	void CoreSceneManager::OnInit()
	{
		
 	}

	void CoreSceneManager::OnShutdown()
	{
		
 	}
}
