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

#include "LuaScriptSystem.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/System/ISystemManager.h"
#include "Core/ComponentSystem/BaseComponentContainerTemplateManager.h"
#include "Core/Utils/Log.h"
#include "Core/System/SystemFactory.h"
#include "Sim/Scenario/Scene/SceneManagerFactory.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/SimEngine.h"


#include "Sim/SimEngine.h"
#include "Sim/Systems/SimSystemManager.h"
#include "LuaScriptManager.h"



extern "C" 
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
	int luaopen_GASS(lua_State* L);
	namespace GASS
	{
		void swig_new_message_ptr(lua_State* L, GASS::MessagePtr* message);
	}
}


namespace GASS
{
	LuaScriptSystem::LuaScriptSystem()
	{

	}

	LuaScriptSystem::~LuaScriptSystem()
	{

	}

	void LuaScriptSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("LuaScriptSystem",new GASS::Creator<LuaScriptSystem, ISystem>);
	}

	void LuaScriptSystem::OnCreate()
	{
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(LuaScriptSystem::OnInit,InitMessage,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(LuaScriptSystem::OnLuaStateMessage,LuaScriptStateMessage,0));
	}

	void LuaScriptSystem::OnInit(InitMessagePtr message)
	{
		LuaScriptManager* lsm = new LuaScriptManager();
		
		 /*lua_State *L = lua_open();
		 //lua_close(L);
		 luaopen_GASS(L);
		 PostMessage(MessagePtr(new LuaStateMessage(L));*/
	}

	void LuaScriptSystem::OnShutdown(MessagePtr message)
	{

	}

	void LuaScriptSystem::OnLuaStateMessage(LuaScriptStateMessagePtr message)
	{
		lua_State *state =  static_cast<lua_State*>(message->GetState());
		luaopen_GASS(state);
	}
}
