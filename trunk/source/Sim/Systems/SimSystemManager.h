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

namespace GASS
{
	class MessageManager;
	class GASSExport SimSystemManager : public BaseSystemManager
	{
	public:
		//Divided messages in two catagories, notify and request
		//Messages with prefix SYSTEM_RM, is a request message
		//Messages with prefix SYSTEM_NM, is a notify message
		enum SystemMessages
		{
			//-----------------Request section-------------
			SYSTEM_RM_INIT,
			SYSTEM_RM_SHUTDOWN,
			SYSTEM_RM_CREATE_RENDER_WINDOW,
			SYSTEM_RM_DEBUG_PRINT,

			//--------------------Notify section------------------------
			SYSTEM_NM_GFX_SM_LOADED,
			SYSTEM_NM_MAIN_WINDOW_CREATED,
			SYSTEM_NM_WINDOW_MOVED_OR_RESIZED,
			SYSTEM_NM_SCENARIO_SCENE_LOADED,
			SYSTEM_NM_SCENARIO_SCENE_ABOUT_TO_LOAD
		};
	public:
		SimSystemManager();
		virtual ~SimSystemManager();
		//MessageManager* GetMessageManager() {return m_SystemMessageManager;}
		void Init();

		int RegisterForMessage(SystemMessages type,  MessageFunc callback, int priority = 0);
		void UnregisterForMessage(SystemMessages type,  MessageFunc callback);
		void PostMessage(MessagePtr message);
		void SendImmediate(MessagePtr message);
		void Update(float delta_time);		
	private:
		MessageManager* m_SystemMessageManager;
	};
}