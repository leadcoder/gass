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

#ifndef LUA_SCRIPT_COMPONENT_H
#define LUA_SCRIPT_COMPONENT_H

#include "Sim/GASS.h"

struct lua_State;
class LuaScriptSceneObject;
namespace GASS
{
	class LuaScriptComponent :  public Reflection<LuaScriptComponent,BaseSceneComponent>
	{
	public:
		LuaScriptComponent();
		virtual ~LuaScriptComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
	private:
		void InitScript();
		void UpdateScript();

		void SetScriptFile(const std::string &script_file);
		std::string GetScriptFile() const;
		lua_State *m_State;
		std::string m_Script;
		LuaScriptSceneObject* m_ScriptWrapper;
	};
}
#endif
