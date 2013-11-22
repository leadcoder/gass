/****************************************************************************
* This file is part of GASS.                                                *
* See http://sourceforge.net/projects/gass/                                 *
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

#include "Sim/GASSScriptManager.h"
#include "Sim/GASSResourceGroup.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Core/Utils/GASSStringUtils.h"
#include "Core/Utils/GASSXMLUtils.h"
#include "Core/Utils/GASSException.h"
#include <tinyxml.h>
#include <angelscript.h>
//addons
#include "Sim/Utils/Script/scriptstdstring.h"
#include "Sim/Utils/Script/ScriptBuilder.h"
namespace GASS
{

	void MessageCallback(const asSMessageInfo *msg, void *param)
	{
		const char *type = "ERR ";
		if( msg->type == asMSGTYPE_WARNING ) 
			type = "WARN";
		else if( msg->type == asMSGTYPE_INFORMATION ) 
			type = "INFO";
		printf("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
	}

	// Function implementation with native calling convention
	void PrintString(std::string &str)
	{
		std::cout << str;
	}

	ScriptManager::ScriptManager(void) 
	{
		m_Engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		if( m_Engine == 0 )
		{
			GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR, "Failed to create script engine","ScriptManager::ScriptManager");
		}
		// The script compiler will write any compiler messages to the callback.
		m_Engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
	}

	ScriptManager::~ScriptManager(void)
	{
		// Release the engine
		m_Engine->Release();
	}

	void ScriptManager::Init()
	{
		RegisterStdString(m_Engine);
		int r = m_Engine->RegisterGlobalFunction("void Print(string &in)", asFUNCTION(PrintString), asCALL_CDECL); assert( r >= 0 );
		//r = m_Engine->RegisterGlobalFunction("uint GetSystemTime()", asFUNCTION(timeGetTime), asCALL_STDCALL); assert( r >= 0 );

		ScriptControllerPtr  controller = LoadScript("c:/temp/test.as");

		asIScriptFunction *my_func = controller->GetModule()->GetFunctionByDecl("void onTick()");
		r = ctx->Prepare(my_func);
		if( r < 0 ) 
		{
			cout << "Failed to prepare the context." << endl;
			return;
		}

		r = ctx->Execute();
	if( r != asEXECUTION_FINISHED )
	{
		// The execution didn't finish as we had planned. Determine why.
		if( r == asEXECUTION_ABORTED )
			std::cout << "The script was aborted before it could finish. Probably it timed out." << endl;
		else if( r == asEXECUTION_EXCEPTION )
		{
			std::cout << "The script ended with an exception." << endl;

			// Write some information about the script exception
			asIScriptFunction *func = ctx->GetExceptionFunction();
			std::cout << "func: " << func->GetDeclaration() << endl;
			std::cout << "modl: " << func->GetModuleName() << endl;
			std::cout << "sect: " << func->GetScriptSectionName() << endl;
			std::cout << "line: " << ctx->GetExceptionLineNumber() << endl;
			std::cout << "desc: " << ctx->GetExceptionString() << endl;
		}
		else
			std::cout << "The script ended for some unforeseen reason (" << r << ")." << endl;
		}
	}

	ScriptControllerPtr ScriptManager::LoadScript(const std::string &script)
	{
		int r;
		// Find the cached controller
		/*for( unsigned int n = 0; n < m_CachedScripts.size(); n++ )
		{
		if( m_CachedScripts[n]->module == script )
		return m_CachedScripts[n];
		}*/

		// No controller, check if the script has already been loaded
		asIScriptModule *mod = m_Engine->GetModule(script.c_str(), asGM_ONLY_IF_EXISTS);
		if( mod )
		{
			// We've already attempted loading the script before, but there is no controller
			return 0;
		}

		// Compile the script into the module
		CScriptBuilder builder;
		r = builder.StartNewModule(m_Engine, script.c_str());
		if( r < 0 )
			return 0;

		// If the script file doesn't exist, then there is no script controller for this type
		FILE *f;
		if( (f = fopen((script + ".as").c_str(), "r")) == 0 )
			return 0;
		fclose(f);

		// Let the builder load the script, and do the necessary pre-processing (include files, etc)
		r = builder.AddSectionFromFile((script + ".as").c_str());
		if( r < 0 )
			return 0;

		r = builder.BuildModule();
		if( r < 0 )
			return 0;

		// Cache the functions and methods that will be used
		ScriptControllerPtr ctrl(new ScriptController());
		m_ScriptControllers[script] = ctrl;
		//ctrl->module = script;
		// Find the class that implements the IController interface
		mod = m_Engine->GetModule(script.c_str(), asGM_ONLY_IF_EXISTS);
		asIObjectType *type = 0;
		return ctrl;
		/*int tc = mod->GetObjectTypeCount();
		for( int n = 0; n < tc; n++ )
		{
		bool found = false;
		type = mod->GetObjectTypeByIndex(n);
		int ic = type->GetInterfaceCount();
		for( int i = 0; i < ic; i++ )
		{
		if( strcmp(type->GetInterface(i)->GetName(), "IController") == 0 )
		{
		found = true;
		break;
		}
		}

		if( found == true )
		{
		ctrl->type = type;
		break;
		}
		}

		if( ctrl->type == 0 )
		{
		cout << "Couldn't find the controller class for the type '" << script << "'" << endl;
		controllers.pop_back();
		delete ctrl;
		return 0;
		}

		// Find the factory function
		// The game engine will pass in the owning CGameObj to the controller for storage
		string s = string(type->GetName()) + "@ " + string(type->GetName()) + "(CGameObj @)";
		ctrl->factoryFunc = type->GetFactoryByDecl(s.c_str());
		if( ctrl->factoryFunc == 0 )
		{
		cout << "Couldn't find the appropriate factory for the type '" << script << "'" << endl;
		controllers.pop_back();
		delete ctrl;
		return 0;
		}

		// Find the optional event handlers
		ctrl->onThinkMethod     = type->GetMethodByDecl("void OnThink()");
		ctrl->onMessageMethod   = type->GetMethodByDecl("void OnMessage(ref @msg, const CGameObj @sender)");

		// Add the cache as user data to the type for quick access
		type->SetUserData(ctrl);*/

	}
}




