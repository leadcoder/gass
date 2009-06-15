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

#ifndef MESSAGEMANAGER_HH
#define MESSAGEMANAGER_HH

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
        typedef boost::shared_ptr<Message> MessagePtr;
		//typedef tbb::concurrent_queue<MessagePtr> MessageQueue;
		typedef std::list<MessagePtr> MessageQueue;
		typedef std::map<int,MessageType*> MessageTypeMap;
	public:
		MessageManager();
		virtual ~MessageManager();

		/**
			This function will put the message in the message queue
			of the message manager. When the update function is called 
			the message queue is processed and the message is handled

		*/
		void PostMessage(MessagePtr  message);
		
		/**
			This function will skip the message queue and let 
			the message manager deliver this message immediately
		*/
		void SendImmediate(MessagePtr  message);
		
		/**
			Register to listen to messages of certain type.
			The message callback has to be of the follwing type
			void MyClass::MyMessageCallback(MessagePtr messsage)
			Priority argument let you tell the message manager if 
			you want to be called early or late when a message is
			processed. This could for instance be useful if you have 
			Initialization message  want a certain call order*/

		int RegisterForMessage(int type,  MessageFunc callback, int priority = 0);

		/**
			Unregister to listen to messages of certain type.
			The callback function used during registration has to 
			be provided again because its used as identifier.
		*/
		void UnregisterForMessage(int type, MessageFunc callback);

		/**
			Process the message queue and deliver messages to 
			registred listeners.
		*/
		void Update(float dt);
	private:
		//#pragma deprecated(AddMessageToSystem)
		void AddMessageToSystem(int type);
		MessageQueue m_MessageQueue;
		MessageTypeMap m_MessageTypes;
		tbb::spin_mutex *m_Mutex;
	};
}
#endif // #ifndef MESSAGEMANAGER_HH
