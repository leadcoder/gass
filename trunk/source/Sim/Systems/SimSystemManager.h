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
		enum
		{
			SYSTEM_MESSAGE_INIT,
			SYSTEM_MESSAGE_UPDATE,
			SYSTEM_MESSAGE_SHUTDOWN,
			SYSTEM_MESSAGE_GFX_SM_LOADED,
			SYSTEM_MESSAGE_CREATE_RENDER_WINDOW,
			SYSTEM_MESSAGE_MAIN_WINDOW_CREATED,
			SYSTEM_MESSAGE_WINDOW_MOVED_OR_RESIZED,
			SYSTEM_MESSAGE_SCENARIO_SCENE_LOADED,
			SYSTEM_MESSAGE_SCENARIO_SCENE_ABOUT_TO_LOAD,
			SYSTEM_MESSAGE_DEBUG_PRINT,
			SYSTEM_MESSAGE_USER,
		};
	public:
		SimSystemManager();
		virtual ~SimSystemManager();
		MessageManager* GetMessageManager() {return m_SystemMessageManager;}
		void Init();
		void Update(float delta_time);		
	private:
		MessageManager* m_SystemMessageManager;
	};
}