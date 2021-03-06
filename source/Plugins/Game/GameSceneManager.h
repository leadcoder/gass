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

#ifndef GAME_SCENE_MANAGER_H
#define GAME_SCENE_MANAGER_H

#include "Sim/GASSCommon.h"
#include "Core/MessageSystem/GASSBaseMessage.h"
#include "Sim/GASSBaseSceneManager.h"
#include "Sim/Interface/GASSIMissionSceneManager.h"

namespace GASS
{
	/**
		Scene manager that owns all game components.
	*/
	class GameSceneManager : public Reflection<GameSceneManager, BaseSceneManager>, public IMissionSceneManager
	{
	public:
		GameSceneManager();
		virtual ~GameSceneManager();
		static void RegisterReflection();
		virtual void OnCreate();
		virtual void OnInit();
		virtual void OnShutdown();
		virtual bool GetSerialize() const {return false;}
	protected:
	};
	typedef SPTR<GameSceneManager> GameSceneManagerPtr; 

}
#endif