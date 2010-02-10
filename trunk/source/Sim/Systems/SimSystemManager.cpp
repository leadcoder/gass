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

#include "Core/System/ISystem.h"
#include "Core/System/SystemFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/Utils/Log.h"
#include "Core/Serialize/IXMLSerialize.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/MessageSystem/BaseMessage.h"
#include "Sim/Systems/SimSystemManager.h"

#include "tinyxml.h"


namespace GASS
{
	SimSystemManager::SimSystemManager()
	{
		m_SystemMessageManager = new MessageManager();
	}

	SimSystemManager::~SimSystemManager()
	{
		delete m_SystemMessageManager;
	}
	
	void SimSystemManager::Init()
	{
		MessagePtr init_msg(new InitMessage());
		m_SystemMessageManager->SendImmediate(init_msg);
		//Log all systems loaded
		for(int i = 0 ;i< m_Systems.size(); i++)
		{
			Log::Print("%s Loaded",m_Systems[i]->GetName().c_str());
		}
	}	

	void SimSystemManager::Update(float delta_time)
	{
		m_SystemMessageManager->Update(delta_time);
	}

	int SimSystemManager::RegisterForMessage(const MessageType &type, MessageFuncPtr callback, int priority)
	{
		return m_SystemMessageManager->RegisterForMessage(type, callback, priority); 
	}

	void SimSystemManager::UnregisterForMessage(const MessageType &type,  MessageFuncPtr callback)
	{
		m_SystemMessageManager->UnregisterForMessage(type,  callback);
	}

	void SimSystemManager::PostMessage( MessagePtr message )
	{
		m_SystemMessageManager->PostMessage(message);
	}

	void SimSystemManager::SendImmediate( MessagePtr message )
	{
		m_SystemMessageManager->SendImmediate(message);
	}
}
