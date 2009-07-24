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
#include "Core/MessageSystem/Message.h"
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
		MessagePtr init_msg(new Message(SimSystemManager::SYSTEM_RM_INIT));
		m_SystemMessageManager->SendImmediate(init_msg);
	}	

	void SimSystemManager::Update(float delta_time)
	{
		m_SystemMessageManager->Update(delta_time);
	}

	int SimSystemManager::RegisterForMessage(SystemMessages type, MessageFunc callback, int priority)
	{
		return m_SystemMessageManager->RegisterForMessage((int)type, callback, priority); 
	}

	void SimSystemManager::UnregisterForMessage(SystemMessages type,  MessageFunc callback)
	{
		m_SystemMessageManager->UnregisterForMessage((int)type,  callback);
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
