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

#include "LuaScriptComponent.h"
#include "LuaScriptSceneObject.h"


/* Deal with Microsoft's attempt at deprecating C standard runtime functions */
#if !defined(SWIG_NO_CRT_SECURE_NO_DEPRECATE) && defined(_MSC_VER) && !defined(_CRT_SECURE_NO_DEPRECATE)
# define _CRT_SECURE_NO_DEPRECATE
#endif

/* Deal with Microsoft's attempt at deprecating methods in the standard C++ library */
#if !defined(SWIG_NO_SCL_SECURE_NO_DEPRECATE) && defined(_MSC_VER) && !defined(_SCL_SECURE_NO_DEPRECATE)
# define _SCL_SECURE_NO_DEPRECATE
#endif


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
	int luaopen_GASS(lua_State* L);
}

/* The SWIG external runtime is generated by using.
swig -lua -externalruntime swigluarun.h
It contains useful function used by SWIG in its wrappering
SWIG_TypeQuery() SWIG_NewPointerObj()
*/

#define SWIG_LUA_FLAVOR_LUA 1
#define SWIG_LUA_TARGET SWIG_LUA_FLAVOR_LUA

#include "swigluarun.h"   // the SWIG external runtime


namespace GASS
{
	LuaScriptComponent::LuaScriptComponent() : m_State(0),
		m_ScriptWrapper(new LuaScriptSceneObject() )
	{
	}

	LuaScriptComponent::~LuaScriptComponent()
	{
		delete m_ScriptWrapper;
	}

	void LuaScriptComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("LuaScriptComponent",new Creator<LuaScriptComponent, IComponent>);
		RegisterProperty<std::string>("Script", &LuaScriptComponent::GetScriptFile, &LuaScriptComponent::SetScriptFile);
	}

	void LuaScriptComponent::OnInitialize()
	{
		m_ScriptWrapper->SetSceneObject(GetSceneObject());
		m_State = luaL_newstate();
		luaopen_base(m_State);
		luaopen_string(m_State);
		luaopen_math(m_State);

		//load gass classes
		luaopen_GASS(m_State);
		SetScriptFile(m_Script);
		
	}

	void LuaScriptComponent::SetScriptFile(const std::string &script_file)
	{
		m_Script  =script_file;

		if(m_State && m_Script != "")
		{
			if (luaL_loadfile(m_State, m_Script.c_str()) || lua_pcall(m_State, 0, 0, 0)) 
			{
				LogManager::getSingleton().stream() << "LUA_ERROR: cannot run lua file:" << lua_tostring(m_State, -1) << "\n";
			}
			else
				InitScript();
		}
	}

	std::string LuaScriptComponent::GetScriptFile() const
	{
		return m_Script;
	}


	// this code pushes a C++ pointer as well as the SWIG type onto the Lua stack
	bool push_pointer(lua_State*L, void* ptr, const char* type_name, int owned = 0) 
	{
		// task 1: get the object 'type' which is registered with SWIG
		// you need to call SWIG_TypeQuery() with the class name
		// (normally, just look in the wrapper file to get this)
		swig_type_info * pTypeInfo = SWIG_TypeQuery(L, type_name);
		if (pTypeInfo == 0)
			return false;   // error
		// task 2: push the pointer to the Lua stack
		// this requires a pointer & the type
		// the last param specifies if Lua is responsible for deleting the object
		SWIG_NewPointerObj(L, ptr, pTypeInfo, owned);
		return true;
	}

	LuaScriptSceneObject e;

	void LuaScriptComponent::UpdateScript()
	{
		
		int top;
		top = lua_gettop(m_State);  /* for later */
		

		lua_pushstring(m_State, "onUpdate");                                  /* function name */
		//lua_gettable(m_State, LUA_GLOBALSINDEX);               /* function to be called */
		//lua_rawgeti(m_State, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
		if (!lua_isfunction(m_State, -1)) {
			std::cout << "[C++] error: cannot find function 'onUpdate'\n";
			lua_settop(m_State, top);  // reset
			return;
		}


		// push the event object
		push_pointer(m_State, m_ScriptWrapper, "LuaScriptSceneObject *", 0);
		if (lua_pcall(m_State, 1, 0, 0) != 0)  /* call function with 1 arguments and no result */
		{
			std::cout << "[C++] error running function `onUpdate':"<<  lua_tostring(m_State, -1) << "\n";
			lua_settop(m_State, top);  // reset
			return;
		}
		lua_settop(m_State, top);  /* reset stack */
	}


	void LuaScriptComponent::_PushObjectWrapper()
	{
		swig_type_info* typeinfo = SWIG_TypeQuery( m_State, "LuaScriptSceneObject *" );
		if ( typeinfo == NULL )
		{   
			return;
		}   
		int own =1;
		SWIG_NewPointerObj( m_State, m_ScriptWrapper, typeinfo, own );
	}

	void LuaScriptComponent::InitScript()
	{
		lua_getglobal(m_State, "onInit");  /* function to be called */
		_PushObjectWrapper();
		// push the event object
		if (lua_pcall(m_State, 1, 0, 0) != 0)  /* call function with 1 arguments and no result */
		{
			std::cout << "[C++] error running function `onInit':"<<  lua_tostring(m_State, -1) << "\n";
			//lua_settop(m_State, top);  // reset
			return;
		}

	//	if (lua_pcall(L, 2, 1, 0) != 0)
	//		error(L, "error running function `f': %s",
	//		lua_tostring(L, -1));

		//m_ScriptWrapper->Reset();
		//int top;
		//top = lua_gettop(m_State);  for later 
		//
		//lua_pushstring(m_State, "onInit");                                  /* function name */
		////lua_gettable(m_State, LUA_GLOBALSINDEX);               /* function to be called */
		////lua_rawgeti(m_State, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
		//if (!lua_isfunction(m_State, -1)) {
		//	std::cout << "[C++] error: cannot find function 'onInit'\n";
		//	lua_settop(m_State, top);  // reset
		//	return;
		//}
		//// push the event object
		//push_pointer(m_State, m_ScriptWrapper, "LuaScriptSceneObject *", 0);
		//if (lua_pcall(m_State, 1, 0, 0) != 0)  /* call function with 1 arguments and no result */
		//{
		//	std::cout << "[C++] error running function `onInit':"<<  lua_tostring(m_State, -1) << "\n";
		//	lua_settop(m_State, top);  // reset
		//	return;
		//}
		//lua_settop(m_State, top);  /* reset stack */
	}

	
}