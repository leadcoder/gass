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
#include "Sim/GASSBaseSceneManager.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/Messages/GASSNetworkSceneObjectMessages.h"
#include "Sim/Messages/GASSCoreSceneMessages.h"


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
		void Update(double delta);
	protected:
		void OnLoad(LoadSceneManagersRequestPtr message);
		void OnUnload(UnLoadSceneManagersRequestPtr message);
		void OnNewMasterReplica(MasterReplicaCreatedEventPtr message);
		void GeneratePartID(SceneObjectPtr obj, int &id);
	private:
		bool m_Paused;
		bool m_Init;
		double m_SimulationUpdateInterval;
		double m_TimeToProcess;
		int m_MaxSimSteps;
	};
	typedef boost::shared_ptr<RaknetNetworkSceneManager> RaknetNetworkSceneManagerPtr;
	typedef boost::weak_ptr<RaknetNetworkSceneManager> RaknetNetworkSceneManagerWeakPtr;
	
	
}

