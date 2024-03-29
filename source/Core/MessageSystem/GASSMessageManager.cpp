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
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/MessageSystem/GASSMessageType.h"
#include "Core/Utils/GASSException.h"
#include <iostream>

namespace GASS
{
	void MessageManager::AddMessageToSystem(const MessageType &type)
	{
		MessageTypeListenerMap::iterator message_type;

		message_type = m_MessageTypes.find(type);
		if(message_type == m_MessageTypes.end())
		{
			const MessageTypeListenerPtr new_type = GASS_MAKE_SHARED<MessageTypeListeners>();
			new_type->m_TypeID = type;
			m_MessageTypes[type] = new_type;
		}
	}

	void MessageManager::PostMessage(MessagePtr  message)
	{
		//lock
		GASS_MUTEX_LOCK(m_Mutex);
		m_MessageQueue.push_back(message);
	}


	void MessageManager::SendImmediate(MessagePtr  message)
	{
		//lock
		//GASS_MUTEX_LOCK(*m_Mutex);

		MessageTypeListenerMap::iterator message_type;
		message_type = m_MessageTypes.find(message->GetType());
		if(message_type == m_MessageTypes.end())
		{

			/*    MessageType mt = message->GetType();
			FileLog::Print("Failed %s",message->GetType().name());
			message_type = m_MessageTypes.begin();
			while(message_type != m_MessageTypes.end())
			{
			if(mt == message_type->first)
			FileLog::Print("REG failed name %s",message_type->first.name());
			else
			FileLog::Print("REG name %s",message_type->first.name());
			message_type++;
			}*/
			return;
		}
		auto msg_reg = message_type->second->m_MessageRegistrations.begin();
		while(msg_reg != message_type->second->m_MessageRegistrations.end())
		{

			//(*msg_reg)->m_Callback->Fire(message);
			//++msg_reg;
			if(!(*msg_reg)->m_Callback->GetObjectPtr())
			{
				msg_reg = message_type->second->m_MessageRegistrations.erase(msg_reg);
			}
			else
			{
				(*msg_reg)->m_Callback->Fire(message);
				++msg_reg;
			}
		}
	}

	bool MessageRegSortPredicate(const MessageRegPtr &lhs, const MessageRegPtr &rhs)
	{
		return lhs->m_Priority < rhs->m_Priority;
	}

	int MessageManager::RegisterForMessage(const MessageType &type, MessageFuncPtr callback, int priority)
	{
		//lock
		GASS_MUTEX_LOCK(m_Mutex);

		MessageTypeListenerMap::iterator message_type;

		message_type = m_MessageTypes.find(type);
		if(message_type == m_MessageTypes.end())
		{
			//return 1;//Register error;
			AddMessageToSystem(type);
			message_type = m_MessageTypes.find(type);
		}

		auto msg_reg = message_type->second->m_MessageRegistrations.begin();
		while(msg_reg != message_type->second->m_MessageRegistrations.end())
		{
			if(*(*msg_reg)->m_Callback == *callback)
			{
				GASS_EXCEPT(Exception::ERR_DUPLICATE_ITEM, "Callback already registered for message", "MessageManager::RegisterForMessage");
				//return 1;
			}	
			++msg_reg;
		}
		MessageRegPtr new_reg (new MessageReg());
		new_reg->m_Callback = callback;
		new_reg->m_Priority = priority;

		message_type->second->m_MessageRegistrations.push_back(new_reg);
		message_type->second->m_MessageRegistrations.sort(MessageRegSortPredicate);
		//std::sort(message_type->second->m_MessageRegistrations.begin(),message_type->second->m_MessageRegistrations.end(),MessageSorter());
		return 0;
	}

	void MessageManager::UnregisterForMessage(const MessageType &type, MessageFuncPtr callback)
	{
		//lock
		GASS_MUTEX_LOCK(m_Mutex);

		MessageTypeListenerMap::iterator message_type;

		message_type = m_MessageTypes.find(type);
		if(message_type == m_MessageTypes.end())
		{
			return;//Register error;
		}

		auto msg_reg = message_type->second->m_MessageRegistrations.begin();

		while(msg_reg != message_type->second->m_MessageRegistrations.end())
		{
			if(*(*msg_reg)->m_Callback == *callback)
			{
				msg_reg = message_type->second->m_MessageRegistrations.erase(msg_reg);
				return;
			}
			++msg_reg;
		}
	}


	void MessageManager::Clear()
	{
		//lock
		GASS_MUTEX_LOCK(m_Mutex);

		m_MessageQueue.clear();
		//m_MessageTypes.clear();
	}

	// Updates the message manager and deliver messages to suscribers
	void MessageManager::Update(double dt)
	{
		//lock message queue and copy to temporary queue for processing
		MessageQueue work_queue;
		{
			GASS_MUTEX_LOCK(m_Mutex);
			if(m_MessageQueue.size() == 0)
				return;

			work_queue =  m_MessageQueue;
			m_MessageQueue.clear();
		}


		//std::cout << "Messages:" << m_MessageQueue.size()<< std::endl;
		auto iter = work_queue.begin();

		//start processing messages!
		while (iter !=  work_queue.end())
		{
			double delay = (*iter)->GetDeliverDelay();
			if(delay > 0)
			{
				delay -= dt;
				(*iter)->SetDeliverDelay(delay);
				++iter;
			}
			else
			{
				MessageTypeListenerMap::iterator message_type;
				message_type = m_MessageTypes.find((*iter)->GetType());

				//if message type not found, add this type to the message type list
				if(message_type == m_MessageTypes.end())
				{
					AddMessageToSystem((*iter)->GetType());
					message_type = m_MessageTypes.find((*iter)->GetType());
					//iter++;
					//continue;
				}

				auto msg_reg = message_type->second->m_MessageRegistrations.begin();
				while(msg_reg != message_type->second->m_MessageRegistrations.end())
				{
					//Check if message listener still alive, if not, erase it
					if(!(*msg_reg)->m_Callback->GetObjectPtr())
					{
						msg_reg = message_type->second->m_MessageRegistrations.erase(msg_reg);
					}
					else
					{
						(*msg_reg)->m_Callback->Fire(*iter);
						++msg_reg;	
					}
				}
				iter = work_queue.erase(iter);
			}
		}

		//lock and add unprocessed messages back to queue due to delayed delivery
		{
			GASS_MUTEX_LOCK(m_Mutex);
			auto work_iter = work_queue.begin();
			while (work_iter !=  work_queue.end())
			{
				m_MessageQueue.push_back(*work_iter);
				++work_iter;
			}
		}
	}
}
