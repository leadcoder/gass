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

#include "Sim/GASS.h"

struct lua_State;
namespace GASS
{
	class LuaScriptManager  : public Singleton<LuaScriptManager>
	{
	public:
		LuaScriptManager();
		virtual ~LuaScriptManager();
		static LuaScriptManager* GetPtr();
		static LuaScriptManager& Get();
		void RegisterForSystemMessage(const std::string &lua_function,MessagePtr message_type);
		void OnMessage(MessagePtr message);
		void SetState(lua_State* state);
		lua_State* GetState() const {m_State;}
		//DebugPrintMessage ToDebugPrintMessage(MessagePtr message);
		 
	private:
		bool ExecuteMessageFunction(const std::string& handler_name, MessagePtr message);
		void PushNamedFunction(lua_State* L, const std::string& handler_name);

		
		typedef std::vector<std::string> str_vec;
		std::map<MessageType,str_vec> m_MessageRegMap;
		lua_State* m_State;
	};
}

