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
//#include <tbb/concurrent_queue.h>
#include <map>
#include <boost/shared_ptr.hpp>
#include "Core/Common.h"
#include "Core/MessageSystem/MessageType.h"


namespace tbb
{
	class spin_mutex;
}

namespace GASS
{
	class Message;
	class GASSCoreExport MessageManager
	{
	public:
		typedef MessagePtr MessagePtr;
		//typedef tbb::concurrent_queue<MessagePtr> MessageQueue;
		typedef std::list<MessagePtr> MessageQueue;
		typedef std::map<int,MessageType*> MessageTypeMap;
	public:
		MessageManager();
		virtual ~MessageManager();
		void SendGlobalMessage(MessagePtr  message);
		void SendImmediate(MessagePtr  message);
		int RegisterForMessage(int type,  MessageFunc callback, int priority = 0);
		void UnregisterForMessage(int type, MessageFunc callback);
		void Update(float dt);
	private:
		//#pragma deprecated(AddMessageToSystem)
		void AddMessageToSystem(int type);
		MessageQueue m_MessageQueue;
		MessageTypeMap m_MessageTypes;
		tbb::spin_mutex *m_Mutex;
	};
}