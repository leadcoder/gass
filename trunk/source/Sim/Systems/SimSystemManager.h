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
#include <list>
#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "Sim/Common.h"
#include "Core/MessageSystem/MessageType.h"
#include "Core/System/BaseSystemManager.h"
#include "Sim/Systems/SimSystemMessages.h"

namespace GASS
{
	class MessageManager;

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
		Register for SimSystemMessages
		*/
		int RegisterForMessage(SimSystemMessage type, MessageFuncPtr callback, int priority = 0);
		void UnregisterForMessage(SimSystemMessage type, MessageFuncPtr callback);
		void PostMessage(MessagePtr message);
		void SendImmediate(MessagePtr message);
		void Update(float delta_time);		
	private:
		MessageManager* m_SystemMessageManager;
	};
}