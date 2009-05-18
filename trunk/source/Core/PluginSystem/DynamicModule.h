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

#include "Core/Common.h"
#include <string>

namespace GASS 
{
	/*class GASSCoreExport EnginePlugin 
	{
	public:
		EnginePlugin();
		virtual ~EnginePlugin();
		virtual void InitPlugin()=0;
		virtual std::string GetType()=0;
	};*/

	//typedef EnginePlugin*(*OnLoadModule)();
	//typedef void(*OnUnloadModule)();
	typedef void* ModuleHandle;

	class GASSCoreExport DynamicModule
	{
	public:
		DynamicModule(const std::string &module_name);
		virtual ~DynamicModule();
		bool Load();
		void Unload();
		inline const std::string &GetModuleName(){return m_ModuleName;}
		//inline EnginePlugin* GetEnginePlugin(){return m_EnginePlugin;}
	protected:
		std::string m_ModuleName;
		//ModuleHandle m_ModuleHandle;
		DYNLIB_HANDLE m_ModuleHandle;
		//EnginePlugin* m_EnginePlugin;
	};
}

