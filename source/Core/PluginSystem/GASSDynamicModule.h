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

#ifndef DYNAMICMODULE_HH
#define DYNAMICMODULE_HH

#include "Core/Common.h"
#include <string>

namespace GASS
{
	typedef void* ModuleHandle;


	/** \addtogroup GASSCore
	*  @{
	*/
	/** \addtogroup Plugin
	*  @{
	*/

	/**
		Class used to handle dynamic modules (dll under windows and so for linux)
	*/

	class GASSCoreExport DynamicModule
	{
	public:
		DynamicModule(const std::string &module_name);
		virtual ~DynamicModule();
		void Load();
		void Unload();
		void CallFunction(const std::string &func_name, void* arg1);
		inline const std::string &GetModuleName(){return m_ModuleName;}
	protected:
		std::string m_ModuleName;
		DYNLIB_HANDLE m_ModuleHandle;
	};
}

#endif // #ifndef DYNAMICMODULE_HH
