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

#pragma once


#include "Sim/GASSCommon.h"
#include "Sim/GASSResourceLocation.h"
#include "Sim/GASSResource.h"
class TiXmlElement;
class asIScriptEngine;

namespace GASS
{
	/**
		ScriptManager
	*/
	class GASSExport ScriptController
	{
	public:
		ScriptController(){}
		virtual ~ScriptController(){}
	private:
	};
	typedef SPTR<ScriptController> ScriptControllerPtr;

	/**
		ScriptManager
	*/
	class GASSExport ScriptManager
	{
	public:
		ScriptManager();
		virtual ~ScriptManager();
		void Init();
		ScriptControllerPtr LoadScript(const std::string &script);
		asIScriptEngine *GetEngine() const {return m_Engine;}
	private:
		asIScriptEngine *m_Engine;
		std::map<std::string,ScriptControllerPtr> m_ScriptControllers;
	};
	typedef SPTR<ScriptManager> ScriptManagerPtr;
}
