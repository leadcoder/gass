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

#ifndef GASS_MESSAGEMANAGER_H
#define GASS_MESSAGEMANAGER_H


#include "Core/Common.h"
#include "Core/MessageSystem/GASSMessageType.h"

namespace tbb
{
	class spin_mutex;
}

namespace GASS
{
	class IMessage;
	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Message
	*  @{
	*/

	/**
		The message manager is responsible to send queued messages to 
		message subscribers. A message subscriber is added to the message manager
		by using the RegisterForMessage method. And messages are posted by using
		the PostMessage method. The user is responsible for calling
		the Update method each tick, it's inside the update method messages get
		delivered to subscribers.
	*/

	class GASSCoreExport MessageManager
	{
	public:
        typedef boost::shared_ptr<IMessage> MessagePtr;
		//typedef tbb::concurrent_queue<MessagePtr> MessageQueue;
		typedef std::list<MessagePtr> MessageQueue;
		typedef std::map<MessageType,MessageTypeListenersPtr> MessageTypeListenerMap;
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
			The message callback has to be of the following type
			void MyClass::MyMessageCallback(MessagePtr messsage)
			Priority argument let you tell the message manager if
			you want to be called early or late when a message is
			processed. This could for instance be useful if you have
			Initialization message  and want a certain call order*/

		int RegisterForMessage(const MessageType &type,  MessageFuncPtr callback, int priority = 0);

		/**
			Unregister to listen to messages of certain type.
			The callback function used during registration has to
			be provided again because its used as identifier.
		*/
		void UnregisterForMessage(const MessageType &type, MessageFuncPtr callback);

		/**
			Process the message queue and deliver messages to
			registred listeners.
		*/
		void Update(float dt);


		void Clear();
		int GetQueuedMessages() const {return m_MessageQueue.size();}
	private:
		//#pragma deprecated(AddMessageToSystem)
		void AddMessageToSystem(const MessageType &type);
		MessageQueue m_MessageQueue;
		MessageTypeListenerMap m_MessageTypes;
		tbb::spin_mutex *m_Mutex;
	};
	typedef boost::shared_ptr<MessageManager> MessageManagerPtr;

	#define REG_TMESS(FUNCTION,TYPED_MESSAGE,PRIORITY) typeid(TYPED_MESSAGE),TYPED_MESSAGE_FUNC(FUNCTION,TYPED_MESSAGE),PRIORITY
	#define UNREG_TMESS(FUNCTION,TYPED_MESSAGE) typeid(TYPED_MESSAGE),TYPED_MESSAGE_FUNC(FUNCTION,TYPED_MESSAGE)

	//#define REG_TMESS_CUSTOM_LISTENER(FUNCTION,TYPED_MESSAGE,LISTENER,PRIORITY) typeid(TYPED_MESSAGE),TYPED_MESSAGE_FUNC_CUSTOM_LISTENER(FUNCTION,TYPED_MESSAGE,LISTENER),PRIORITY
}
#endif // #ifndef MESSAGEMANAGER_HH
