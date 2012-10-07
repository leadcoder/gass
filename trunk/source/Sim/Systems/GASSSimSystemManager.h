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
#include "Core/System/GASSISystemManager.h"
#include "Sim/Systems/GASSSimSystem.h"

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
	class GASSExport SimSystemManager : public ISystemManager, public boost::enable_shared_from_this<SimSystemManager>,  public IMessageListener
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

		bool GetSimulationPasued() const {return m_SimulationPaused;}
		void SetPauseSimulation(bool value) {m_SimulationPaused = value;}
		void SetSimulationUpdateInterval(bool value) {m_SimulationUpdateInterval = value;}
		void SetSimulateRealTime(bool value) {m_SimulateRealTime = value;}

		//can be called by user it simulation is paused
		void UpdateSimulation(double delta_time);
		SimSystemPtr GetSystemByName(const std::string &system_name) const;
		
		/**
			Get hold of system by class type. If more then one system
			of the class type exist tbe first one loaded will be returned
		*/
		template <class T>
		boost::shared_ptr<T> GetFirstSystem()
		{
			boost::shared_ptr<T> sys;
			for(size_t i = 0 ; i < m_Systems.size(); i++)
			{
				sys = boost::shared_dynamic_cast<T>(m_Systems[i]);
				if(sys)
					break;
			}
			return sys;
		}

		/**
			Loading systems from xml-file, syntax example:

			<?xml version="1.0" encoding="utf-8"?>
			<Systems>
				<GraphicsSystem type="OgreGraphicsSystem">
					... gfx system params
	  		    </GraphicsSystem>
				<InputSystem type="OISInputSystem">
					... input system params
				</InputSystem>
				...
			<System>
			A system is specified by a name tag and att type attribute that 
			specify the class implementing the system.
		*/
		void Load(const std::string &filename);
	private:
		SystemPtr LoadSystem(TiXmlElement *system_elem);
		size_t GetQueuedMessages() const;
		void OnSimulationStepRequest(RequestTimeStepMessagePtr message);
		void StepSimulation(double delta_time);		
		void SyncMessages(double delta_time);
		MessageManagerPtr m_SystemMessageManager;
		bool m_SimulationPaused;
		bool m_SimulateRealTime;
		double m_SimulationUpdateInterval;
		double m_SimulationTimeToProcess;
		int m_MaxSimSteps;
		int m_LastNumSimulationSteps;

		bool m_StepSimulationRequest;
		double m_RequestDeltaTime;

		SimpleProfileDataMap* m_SimStats;

		struct MessageData
		{
			int Before;
			int After;
		};

		typedef std::map<int,MessageData> MessageStatMap;
		MessageStatMap m_MessageStats;
	
		typedef std::vector<SystemPtr> SystemVector;
		SystemVector m_Systems;
		typedef std::map<int,SystemVector> UpdateMap;
		UpdateMap m_UpdateBuckets;
	};
}