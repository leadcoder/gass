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

#include "Core/PluginSystem/GASSDynamicModule.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSException.h"
#include <assert.h>

#ifdef WIN32
#define __STDCALL __stdcall
#else
#define __STDCALL
#endif

namespace GASS
{


	DynamicModule::DynamicModule(const std::string &module_name)
	{
		m_ModuleName = module_name;
	}



	void DynamicModule::Load()
	{
		// dlopen() does not add .so to the filename, like windows does for .dll
		//if (m_ModuleName.substr(m_ModuleName.length() - 3, 3) != ".so")
		//	m_ModuleName += ".so";
#ifdef WIN32
	SetErrorMode(0);
#endif

		m_ModuleHandle = (DYNLIB_HANDLE)DYNLIB_LOAD(m_ModuleName.c_str());

#ifndef WIN32
		char *errstr = dlerror();
		if (errstr != NULL)
			LogManager::getSingleton().stream() << "WARNING:A dynamic linking error occurred:"  << errstr;
#endif
		//assert(m_ModuleHandle && "Unable to load dynamic module");
		if (m_ModuleHandle == NULL)
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE, "Unable to load dynamic module:" + m_ModuleName, "DynamicModule::Load()");
		//OnLoadModule onLoadModule = (OnLoadModule)GetDynamicSymbol(m_ModuleHandle,"onLoadModule");
		//OnLoadModule onLoadModule = (OnLoadModule)DYNLIB_GETSYM(m_ModuleHandle,"onLoadModule");
		//assert(onLoadModule && "Unable to find onLoadModule function");
		//if(onLoadModule == NULL ) FileLog::Error("Unable to find onLoadModule function, in %s",module_name.c_str());
		//m_EnginePlugin = (EnginePlugin*) (onLoadModule());
		//assert(m_EnginePlugin);
		//do test call

	}

	typedef int (__STDCALL *FuncArg1)(void *);
	void DynamicModule::CallFunction(const std::string &func_name, void* arg1)
	{
		FuncArg1 onLoadModule = (FuncArg1)DYNLIB_GETSYM(m_ModuleHandle,func_name.c_str());
		if(onLoadModule)
		{
			(onLoadModule(arg1));
		}
	}

	void DynamicModule::Unload()
	{
		DYNLIB_UNLOAD(m_ModuleHandle);
	}

	DynamicModule::~DynamicModule()
	{

	}
}
