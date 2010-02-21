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

#pragma once

#include <map>
#include "Core/MessageSystem/IMessage.h"
#include "Sim/Scenario/Scene/BaseSceneManager.h"
#include "Sim/Scenario/Scene/SceneObjectMessages.h"
#include "Sim/Scenario/Scene/ScenarioSceneMessages.h"
#include "Sim/Scheduling/TaskGroups.h"
#include "Sim/Scheduling/ITaskListener.h"


namespace GASS
{

	class RaknetNetworkSceneManager  : public Reflection<RaknetNetworkSceneManager, BaseSceneManager> , public ITaskListener
	{
		public:
		RaknetNetworkSceneManager();
		virtual ~RaknetNetworkSceneManager();
		static void RegisterReflection();
		virtual void OnCreate();
		//ITaskListener interface
		void Update(double delta);
		TaskGroup GetTaskGroup() const;
	protected:
		void OnLoad(LoadSceneManagersMessagePtr message);
		void OnUnload(UnloadSceneManagersMessagePtr message);
		void OnLoadSceneObject(SceneObjectCreatedNotifyMessagePtr message);
		void SetTaskGroup(TaskGroup value);
	private:
		bool m_Paused;
		TaskGroup m_TaskGroup;
		bool m_Init;
		double m_SimulationUpdateInterval;
		double m_TimeToProcess;
		int m_MaxSimSteps;
	};
	typedef boost::shared_ptr<RaknetNetworkSceneManager> RaknetNetworkSceneManagerPtr;
	typedef boost::weak_ptr<RaknetNetworkSceneManager> RaknetNetworkSceneManagerWeakPtr;
	
	
}

