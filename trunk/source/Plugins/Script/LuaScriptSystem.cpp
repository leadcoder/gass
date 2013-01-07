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
#include "LuaScriptManager.h"

extern "C"
{
#ifdef WIN32
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#else
#include "lua5.1/lua.h"
#include "lua5.1/lualib.h"
#include "lua5.1/lauxlib.h"
#endif
	int luaopen_GASS(lua_State* L);
	/*namespace GASS
	{
		void swig_new_message_ptr(lua_State* L, GASS::MessagePtr* message);
	}*/
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

	void LuaScriptSystem::Init()
	{
		SimEngine::Get().GetRuntimeController()->Register(shared_from_this(),m_TaskNodeName);
		//LuaScriptManager* lsm = new LuaScriptManager();
	//	lua_State *L = lua_open();
		
		//luaopen_GASS(L);
	}

	/*void LuaScriptSystem::OnShutdown(MessagePtr message)
	{

	}*/

	/*void LuaScriptSystem::OnLuaStateMessage(LuaScriptStateMessagePtr message)
	{
		lua_State *state =  static_cast<lua_State*>(message->GetState());
		LuaScriptManager::Get().SetState(state);
		luaopen_GASS(state);
	}*/
}
