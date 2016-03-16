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

#include "LuaScriptManager.h"

extern "C" {
#ifdef WIN32

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#else

#include "lua5.1/lua.h"
#include "lua5.1/lualib.h"
#include "lua5.1/lauxlib.h"
#endif
	/*namespace GASS
	{
		void swig_new_message_ptr(lua_State* L, GASS::MessagePtr* message);
	}*/
}

//#include "tolua++.h"
//#include "CEGUI.h"

//#include "ScriptingModules\LuaScriptModule\CEGUILuaFunctor.h"
//#include "ScriptingModules/LuaScriptModule/CEGUILua.h"
namespace GASS
{
	LuaScriptManager::LuaScriptManager() : m_State(NULL)
	{

	}

	LuaScriptManager::~LuaScriptManager()
	{

	}

	template<> LuaScriptManager* Singleton<LuaScriptManager>::m_Instance = 0;
	LuaScriptManager* LuaScriptManager::GetPtr(void)
	{
		assert(m_Instance);
		return m_Instance;
	}

	LuaScriptManager& LuaScriptManager::Get(void)
	{
		assert(m_Instance);
		return *m_Instance;
	}

	/*void LuaScriptManager::RegisterForSystemMessage(const std::string &lua_function,MessagePtr message_type)
	{
		MessageType mtype = message_type->GetType();
		m_MessageRegMap[mtype].push_back(lua_function);
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(mtype, MESSAGE_FUNC(LuaScriptManager::OnMessage),0);
	}

	void LuaScriptManager::OnMessage(MessagePtr message)
	{
		MessageType mtype = message->GetType();
		for(int i = 0 ; i < m_MessageRegMap[mtype].size(); i++)
		{
			std::string lua_func = m_MessageRegMap[mtype].at(i);
			ExecuteMessageFunction(lua_func ,message);
		}
	}*/

	bool LuaScriptManager::ExecuteMessageFunction(const std::string& handler_name, MessagePtr message)
	{
		//CEGUI::LuaScriptModule* scriptmod = dynamic_cast<CEGUI::LuaScriptModule*>( CEGUI::System::getSingleton().getScriptingModule());
		if(!m_State)
		{
			LogManager::getSingleton().stream() << "WARNING: No lua state present\n";
			return false;
		}
		const int err_idx = 0;
		int top = lua_gettop(m_State);

		//PushNamedFunction(d_state, handler_name.c_str());
		lua_getglobal(m_State, handler_name.c_str());
		if (!lua_isfunction(m_State,-1))
		{
			lua_settop(m_State,top);
			return false;
			//throw ScriptException("The Lua event handler: '"+handler_name+"' does not represent a Lua function");
		}

		// call it
		//int error = lua_call(d_state, 0, 0);
		//luaL_getmetatable(d_state, "SWIG.BaseMessage");
		//if (!lua_isnil(d_state, -1))


		//swig_new_message_ptr(m_State, &message);
		{
			// the class wasn't found

			// tolua_pushusertype(d_state, (void*)message.get(),"BaseMessage");
		}

		/*
		SWIGEXPORT void swig_new_message_ptr(lua_State* L, MessagePtr* message)
		{
		SWIG_NewPointerObj(L,message,SWIGTYPE_p_MessagePtr,0);
		}
		*/

		lua_call(m_State, 1, 0);
		// handle errors
		/*if (error)
		{
		std::string  errStr(lua_tostring(d_state,-1));
		lua_settop(d_state,top);
		//throw ScriptException("Unable to evaluate the Lua event handler: '" +
		//                     handler_name + "'\n\n" + errStr + "\n");
		}*/
		// retrieve result
		bool ret = lua_isboolean(m_State, -1) ? lua_toboolean(m_State, -1 ) : true;
		lua_settop(m_State,top);
		return ret;
	}

	void LuaScriptManager::PushNamedFunction(lua_State* L, const std::string& handler_name)
	{
		int top = lua_gettop(L);

		// do we have any dots in the handler name? if so we grab the function as a table field
		std::string::size_type i = handler_name.find_first_of('.');
		if (i!=std::string::npos)
		{
			// split the rest of the string up in parts seperated by '.'
			// TODO: count the dots and size the vector accordingly from the beginning.
			std::vector<std::string> parts;
			std::string::size_type start = 0;
			do
			{
				parts.push_back(handler_name.substr(start,i-start));
				start = i+1;
				i = handler_name.find_first_of('.',start);
			} while(i!=std::string::npos);

			// add last part
			parts.push_back(handler_name.substr(start));

			// first part is the global
			lua_getglobal(L, parts[0].c_str());
			if (!lua_istable(L,-1))
			{
				lua_settop(L,top);
				//throw ScriptException("Unable to get the Lua event handler: '"+handler_name+"' as first part is not a table");
			}

			// if there is more than two parts, we have more tables to go through
			std::vector<std::string>::size_type visz = parts.size();
			if (visz-- > 2) // avoid subtracting one later on
			{
				// go through all the remaining parts to (hopefully) have a valid Lua function in the end
				std::vector<std::string>::size_type vi = 1;
				while (vi<visz)
				{
					// push key, and get the next table
					lua_pushstring(L,parts[vi].c_str());
					lua_gettable(L,-2);
					if (!lua_istable(L,-1))
					{
						lua_settop(L,top);
						//throw ScriptException("Unable to get the Lua event handler: '"+handler_name+"' as part #"+PropertyHelper::uintToString(uint(vi+1))+" ("+parts[vi]+") is not a table");
					}
					// get rid of the last table and move on
					lua_remove(L,-2);
					vi++;
				}
			}

			// now we are ready to get the function to call ... phew :)
			lua_pushstring(L,parts[visz].c_str());
			lua_gettable(L,-2);
			lua_remove(L,-2); // get rid of the table
		}
		// just a regular global function
		else
		{
			lua_getglobal(L, handler_name.c_str());
		}

		// is it a function
		if (!lua_isfunction(L,-1))
		{
			lua_settop(L,top);
			//throw ScriptException("The Lua event handler: '"+handler_name+"' does not represent a Lua function");
		}
	}

	void LuaScriptManager::SetState(lua_State* state)
	{
		m_State = state;
	}

	/*DebugPrintMessage LuaScriptManager::ToDebugPrintMessage(MessagePtr message)
	{
	DebugPrintMessagePtr db_mess = boost::shared_dynamic_cast<DebugPrintMessage>(message);
	DebugPrintMessage ret = *db_mess.get();
	return ret;
	}*/
}
