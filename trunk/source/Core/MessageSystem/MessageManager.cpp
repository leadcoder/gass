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
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/MessageSystem/MessageType.h"
#include "tbb/spin_mutex.h"
#include <stdio.h>
#include <iostream>
#include <fstream>

namespace GASS
{
	MessageManager::MessageManager()
	{
		m_Mutex = new tbb::spin_mutex;
	}
	MessageManager::~MessageManager()
	{
        delete m_Mutex;
	}

	void MessageManager::AddMessageToSystem(const MessageType &type)
	{
		MessageTypeListenerMap::iterator message_type;

		message_type = m_MessageTypes.find(type);
		if(message_type == m_MessageTypes.end())
		{
			MessageTypeListeners* new_type = new MessageTypeListeners;
			new_type->m_TypeID = type;
			m_MessageTypes[type] = new_type;
		}
	}

	void MessageManager::PostMessage(MessagePtr  message)
	{
		//lock
		tbb::spin_mutex::scoped_lock lock(*m_Mutex);
		m_MessageQueue.push_back(message);
	}


	void MessageManager::SendImmediate(MessagePtr  message)
	{
		MessageTypeListenerMap::iterator message_type;
		message_type = m_MessageTypes.find(message->GetType());
		if(message_type == m_MessageTypes.end())
		{
			return;
		}
		MessageRegList::iterator msg_reg = message_type->second->m_MessageRegistrations.begin();
		while(msg_reg != message_type->second->m_MessageRegistrations.end())
		{
			(*msg_reg)->m_Callback->Fire(message);
				msg_reg++;
		}
	}

	bool MessageRegSortPredicate(const MessageReg* lhs, const MessageReg* rhs)
	{
		return lhs->m_Priority < rhs->m_Priority;
	}

	int MessageManager::RegisterForMessage(const MessageType &type, MessageFuncPtr callback, int priority)
	{
		MessageTypeListenerMap::iterator message_type;

		message_type = m_MessageTypes.find(type);
		if(message_type == m_MessageTypes.end())
		{
			//return 1;//Register error;
			AddMessageToSystem(type);
			message_type = m_MessageTypes.find(type);
		}

		MessageRegList::iterator msg_reg = message_type->second->m_MessageRegistrations.begin();
		while(msg_reg != message_type->second->m_MessageRegistrations.end())
		{

			//if((*msg_reg)->m_Callback == callback)
			if(*(*msg_reg)->m_Callback == *callback)
			{
				return 1;
			}
			msg_reg++;
		}
		MessageReg* new_reg = new MessageReg();
		new_reg->m_Callback = callback;
		//new_reg->m_ObjectID = object_id;
		new_reg->m_Priority = priority;

		message_type->second->m_MessageRegistrations.push_back(new_reg);
		message_type->second->m_MessageRegistrations.sort(MessageRegSortPredicate);
		//std::sort(message_type->second->m_MessageRegistrations.begin(),message_type->second->m_MessageRegistrations.end(),MessageSorter());
		return 0;
	}

	void MessageManager::UnregisterForMessage(const MessageType &type, MessageFuncPtr callback)
	{
		MessageTypeListenerMap::iterator message_type;

		message_type = m_MessageTypes.find(type);
		if(message_type == m_MessageTypes.end())
		{
			return;//Register error;
		}

		MessageRegList::iterator msg_reg = message_type->second->m_MessageRegistrations.begin();

		while(msg_reg != message_type->second->m_MessageRegistrations.end())
		{
			if(*(*msg_reg)->m_Callback == *callback)
			{
				delete (*msg_reg);
				msg_reg = message_type->second->m_MessageRegistrations.erase(msg_reg);

				return;
			}
			msg_reg++;
		}
	}

	// Updates the message handler and sends any messages than need to be sent
	void MessageManager::Update(float dt)
	{
		if(m_MessageQueue.size() == 0)
			return;


		//std::cout << "Messages:" << m_MessageQueue.size()<< std::endl;
		MessageQueue::iterator iter = m_MessageQueue.begin();

		while (iter !=  m_MessageQueue.end())
		{
			float delay = (*iter)->GetDeliverDelay();
			if(delay > 0)
			{
				delay -= dt;
				(*iter)->SetDeliverDelay(delay);
				iter++;
			}
			else
			{
				MessageTypeListenerMap::iterator message_type;
				message_type = m_MessageTypes.find((*iter)->GetType());

				if(message_type == m_MessageTypes.end())
				{
					AddMessageToSystem((*iter)->GetType());
					message_type = m_MessageTypes.find((*iter)->GetType());
					//iter++;
					//continue;
				}

				MessageRegList::iterator msg_reg = message_type->second->m_MessageRegistrations.begin();
				while(msg_reg != message_type->second->m_MessageRegistrations.end())
				{
					(*msg_reg)->m_Callback->Fire(*iter);
					msg_reg++;
				}
				//delete (*iter);
				iter = m_MessageQueue.erase(iter);
			}
		}
	}
}
