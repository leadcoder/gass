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

#pragma once

#include "Sim/GASSCommon.h"
#include "Sim/Messages/GASSCoreSystemMessages.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Utils/GASSException.h"
#include "Sim/GASSSimSystem.h"

#define PRE_SIM_BUCKET 9998 //magic number
#define POST_SIM_BUCKET 9999 //magic number

namespace GASS
{
	typedef std::string SystemType;


	//forward declare
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
	class GASSExport SimSystemManager : public GASS_ENABLE_SHARED_FROM_THIS<SimSystemManager>, public IMessageListener
	{
		friend class SystemStepper;
		friend class SystemGroupStepper;
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
		void PostMessage(SystemMessagePtr message);

		/**
		Force send of system message
		*/
		void SendImmediate(SystemMessagePtr message);

		/**
		Clear all unproccessed messages
		*/
		void ClearMessages();

		SimSystemPtr GetSystemByName(const std::string &system_name) const;

		/**
			Get hold of system by class type. If more then one system
			of the class type exist tbe first one loaded will be returned
		*/
		template <class T>
		GASS_SHARED_PTR<T> GetFirstSystemByClass(bool no_throw = false)
		{
			GASS_SHARED_PTR<T> sys;
			for (size_t i = 0; i < m_Systems.size(); i++)
			{
				sys = GASS_DYNAMIC_PTR_CAST<T>(m_Systems[i]);
				if (sys)
					return sys;
			}
			if (!no_throw)
			{
				std::string sys_name = typeid(T).name();
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "System not found:" + sys_name, "SimSystemManager::GetFirstSystemByClass");
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


		SimSystemPtr AddSystem(const std::string &system_name);

		void AddSystem(SimSystemPtr system);

		/**
		Set simulation node to paused or not
		*/
		void SetSimulationPaused(bool value) { return m_SystemStepper.SetSimulationPaused(value); }

		/**
		Stop simulation node
		*/
		void StopSimulation() { return m_SystemStepper.StopSimulation(); }

		/**
		Start simulation node
		*/
		void StartSimulation() { return m_SystemStepper.StartSimulation(); }

		/**
		Set this to true if we want to step simulation from external source
		*/
		SimulationState GetSimulationState() const { return m_SystemStepper.GetSimulationState(); }

		double GetTime() const { return m_SystemStepper.GetTime(); }
		double GetSimulationTime() const { return m_SystemStepper.GetSimulationTime(); }
		double GetSimulationTimeScale() const { return m_SystemStepper.GetSimulationTimeScale(); }
		void SetSimulationTimeScale(double value) { return m_SystemStepper.SetSimulationTimeScale(value); }
		void SetUpdateSimOnRequest(bool value) { return m_SystemStepper.SetUpdateSimOnRequest(value); }
		bool GetUpdateSimOnRequest() const { return m_SystemStepper.GetUpdateSimOnRequest(); }
		//Move this to private
		void SyncMessages(double delta_time);
		void OnUpdate(double delta_time);
	protected:
		void _UpdateSystems(double delta_time, UpdateGroupID group);
	protected:
		void OnSimulationStepRequest(TimeStepRequestPtr message);
		SimSystemPtr LoadSystem(tinyxml2::XMLElement *system_elem);
		size_t GetQueuedMessages() const;
		MessageManagerPtr m_SystemMessageManager;
		typedef std::vector<SimSystemPtr> SystemVector;
		SystemVector m_Systems;
		SystemStepper m_SystemStepper;
	};
}