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

#include "Sim/GASSCommon.h"
#include "Sim/Systems/Messages/GASSCoreSystemMessages.h"
#include "Core/MessageSystem/GASSMessageType.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/System/GASSBaseSystemManager.h"

#define PRE_SIM_BUCKET 9998 //magic number
#define POST_SIM_BUCKET 9999 //magic number

namespace GASS
{
	//forward decalare
	class SimpleProfileData;
	typedef std::map<std::string, SimpleProfileData> SimpleProfileDataMap;

	/**
	System manager for all systems used in GASSSim.
	The system manager load it's systems through xml-files
	and then handle all SimSystemMessages in GASSSim.
	To post a SimSystemMessages you simply call 
	PostMessage in this class with your message as argument.
	To handle the messages the SimSystemManager use the 
	MessageManager class
	*/
	class GASSExport SimSystemManager : public BaseSystemManager
	{
	public:
		SimSystemManager();
		virtual ~SimSystemManager();
		/**
		Called by owner before use
		*/
		void Init();
		/**
		Register for system messages
		*/
		int RegisterForMessage(const MessageType &type, MessageFuncPtr callback, int priority = 0);

		/**
		Unregister for system messages
		*/
		void UnregisterForMessage(const MessageType &type, MessageFuncPtr callback);

		/**
		Post system message
		*/
		void PostMessage(MessagePtr message);

		/**
		Force send of system message
		*/
		void SendImmediate(MessagePtr message);

		/**
		Update System
		*/
		void Update(float delta_time);

		/**
		Clear all unproccessed messages
		*/
		void ClearMessages();

		void SetPauseSimulation(bool value) {m_SimulationPaused = value;}
		void SetSimulationUpdateInterval(bool value) {m_SimulationUpdateInterval = value;}
		void SetSimulateRealTime(bool value) {m_SimulateRealTime = value;}

		//can be called by user it simulation is paused
		void StepSimulation(double delta_time);
	private:
		int GetQueuedMessages() const;
		void UpdateSimulation(double delta_time);
		void SyncMessages(double delta_time);
		MessageManagerPtr m_SystemMessageManager;
		bool m_SimulationPaused;
		bool m_SimulateRealTime;
		double m_SimulationUpdateInterval;
		double m_SimulationTimeToProcess;
		int m_MaxSimSteps;
		int m_LastNumSimulationSteps;


		SimpleProfileDataMap* m_SimStats;

		struct MessageData
		{
			int Before;
			int After;
		};

		typedef std::map<int,MessageData> MessageStatMap;
		MessageStatMap m_MessageStats;
	};
}