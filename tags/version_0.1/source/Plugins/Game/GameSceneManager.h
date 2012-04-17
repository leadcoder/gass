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

#ifndef SIMPLE_SCENE_MANAGER_H
#define SIMPLE_SCENE_MANAGER_H

#include <map>
#include "Core/MessageSystem/BaseMessage.h"
#include "Sim/Common.h"
#include "Sim/Scenario/Scene/BaseSceneManager.h"
#include "Sim/Scheduling/TaskGroups.h"
#include "Sim/Scheduling/ITaskListener.h"

namespace GASS
{
	/**
		Scene manager that owns all game components.
	*/
	class GameSceneManager : public Reflection<GameSceneManager, BaseSceneManager>, public ITaskListener
	{
	public:
		GameSceneManager();
		virtual ~GameSceneManager();
		static void RegisterReflection();
		virtual void OnCreate();
		void Update(double delta_time);
		TaskGroup GetTaskGroup() const;
	protected:
		void OnLoad(MessagePtr message);
		void OnUnload(MessagePtr message);
		void OnLoadSceneObject(MessagePtr message);
	private:
		void SetTaskGroup(TaskGroup value);
		TaskGroup m_TaskGroup;
	};
	typedef boost::shared_ptr<GameSceneManager> GameSceneManagerPtr; 

}
#endif