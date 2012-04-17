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
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/Scenario/Scene/GASSBaseSceneManager.h"
#include "Sim/Scenario/Scene/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/GASSNetworkSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/GASSCoreScenarioSceneMessages.h"
#include "Sim/Scheduling/GASSTaskGroups.h"
#include "Sim/Scheduling/GASSITaskListener.h"
#include "Plugins/RakNet/RakNetMessages.h"


namespace GASS
{

	class RaknetNetworkSceneManager  : public Reflection<RaknetNetworkSceneManager, BaseSceneManager> 
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
		void OnNewMasterReplica(MasterReplicaCreatedMessagePtr message);
		void SetTaskGroup(TaskGroup value);
		void GeneratePartID(SceneObjectPtr obj, int &id);
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

