/****************************************************************************
* This file is part of GASS.                                                *
* See https://github.com/leadcoder/gass                                     *
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
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


#include "Sim/GASSCommon.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSResourceLocation.h"
#include "Sim/GASSResource.h"
#include "Sim/Messages/GASSCoreSystemMessages.h"

namespace tinyxml2
{
	class XMLElement;
}

class asIScriptEngine;
class asIScriptModule;
class asIScriptContext;
class asIScriptFunction;

namespace GASS
{
	/**
		ScriptManager
	*/
	class GASSExport ScriptController
	{
	public:
		ScriptController(asIScriptModule *mod) : m_Module(mod),
		m_UpdateFunction(NULL),
		m_InitFunction(NULL),
		m_FactoryFunction(NULL)
		{}
		virtual ~ScriptController(){}
		asIScriptModule* GetModule() const{return m_Module;}
		asIScriptFunction *m_InitFunction;
		asIScriptFunction *m_UpdateFunction;
		asIScriptFunction *m_ShutdownFunction;
		asIScriptFunction *m_FactoryFunction;
	private:
		asIScriptModule *m_Module;
	};
	typedef GASS_SHARED_PTR<ScriptController> ScriptControllerPtr;

	/**
		ScriptManager
	*/
	class GASSExport ScriptManager : public GASS_ENABLE_SHARED_FROM_THIS<ScriptManager>, public IMessageListener
	{
	public:
		ScriptManager();
		virtual ~ScriptManager();
		void Init();
		ScriptControllerPtr LoadScript(const std::string &script, const std::string &init_func_arg);
		void UnloadScript(const std::string &script);
		asIScriptEngine *GetEngine() const {return m_Engine;}
		void ReturnContextToPool(asIScriptContext *ctx);
		asIScriptContext *PrepareContextFromPool(asIScriptFunction *func);
		int ExecuteCall(asIScriptContext *ctx);
	private:
		void OnScriptEvent(ScriptEventPtr message);
		asIScriptEngine *m_Engine;
		std::map<std::string,ScriptControllerPtr> m_ScriptControllers;

		// Our pool of script contexts. This is used to avoid allocating
		// the context objects all the time. The context objects are quite
		// heavy weight and should be shared between function calls.
		std::vector<asIScriptContext *> m_Contexts;

	};
	typedef GASS_SHARED_PTR<ScriptManager> ScriptManagerPtr;


	/**
		Cast helper
	*/

	template<class A, class B>
	B* refCast(A* a)
	{
		// If the handle already is a null handle, then just return the null handle
		if( !a ) return 0;
		// Now try to dynamically cast the pointer to the wanted type
		B* b = dynamic_cast<B*>(a);
		if( b != 0 )
		{
			// Since the cast was made, we need to increase the ref counter for the returned handle
			//b->addref();
		}
		return b;
	}

}
