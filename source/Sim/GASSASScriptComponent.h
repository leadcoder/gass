/****************************************************************************
* This file is part of GASS.                                                *
* See https://github.com/leadcoder/gass										*
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

#ifndef AS_SCRIPT_COMPONENT_H
#define AS_SCRIPT_COMPONENT_H

#include "Sim/GASS.h"
class asIScriptObject;

namespace GASS
{
	GASS_FORWARD_DECL(ScriptController);

	class ASScriptComponent:  public Reflection<ASScriptComponent,BaseSceneComponent>
	{
	public:
		ASScriptComponent();
		static void RegisterReflection();
		void OnInitialize() override;
		void SceneManagerTick(double delta_time) override;
	private:
		void _SetScriptFile(const std::string &script_file);
		std::string _GetScriptFile() const;
		std::string m_Script;
		asIScriptObject *m_ScriptObject{nullptr};
		ScriptControllerPtr  m_Controller;
	};
}
#endif
