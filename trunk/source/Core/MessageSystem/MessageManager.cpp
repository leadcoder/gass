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
#include "Core/MessageSystem/Message.h"
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
	void MessageManager::AddMessageToSystem(int type)
	{
		MessageTypeMap::iterator message_type;

		message_type = m_MessageTypes.find(type);
		if(message_type == m_MessageTypes.end())
		{
			MessageType* new_type = new MessageType;
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
		MessageTypeMap::iterator message_type;
		message_type = m_MessageTypes.find(message->m_TypeID);
		if(message_type == m_MessageTypes.end())
		{
			return;
		}
		MessageRegList::iterator msg_reg = message_type->second->m_MessageRegistrations.begin();
		while(msg_reg != message_type->second->m_MessageRegistrations.end())
		{
			(*msg_reg)->m_Callback(message);
				msg_reg++;
		}
	}

	bool MessageRegSortPredicate(const MessageReg* lhs, const MessageReg* rhs)
	{
		return lhs->m_Priority < rhs->m_Priority;
	}

	int MessageManager::RegisterForMessage(int type, MessageFunc callback, int priority)
	{
		MessageTypeMap::iterator message_type;

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

			if((*msg_reg)->m_Callback.functor.func_ptr == callback.functor.func_ptr)
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

	void MessageManager::UnregisterForMessage(int type, MessageFunc callback)
	{

		MessageTypeMap::iterator message_type;

		message_type = m_MessageTypes.find(type);
		if(message_type == m_MessageTypes.end())
		{
			return;//Register error;
		}

		MessageRegList::iterator msg_reg = message_type->second->m_MessageRegistrations.begin();

		while(msg_reg != message_type->second->m_MessageRegistrations.end())
		{
			if((*msg_reg)->m_Callback.functor.func_ptr == callback.functor.func_ptr)
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
			if((*iter)->m_Timer > 0)
			{
				(*iter)->m_Timer -= dt;
				iter++;
			}
			else
			{
				MessageTypeMap::iterator message_type;
				message_type = m_MessageTypes.find((*iter)->m_TypeID);

				if(message_type == m_MessageTypes.end())
				{
					AddMessageToSystem((*iter)->m_TypeID);
					message_type = m_MessageTypes.find((*iter)->m_TypeID);
					//iter++;
					//continue;
				}

				MessageRegList::iterator msg_reg = message_type->second->m_MessageRegistrations.begin();
				while(msg_reg != message_type->second->m_MessageRegistrations.end())
				{
					(*msg_reg)->m_Callback(*iter);
					msg_reg++;
				}
				//delete (*iter);
				iter = m_MessageQueue.erase(iter);
			}
		}
	}
}
