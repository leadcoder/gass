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

#include "Sim/Common.h"
#include "Core/Math/Vector.h"
#include "Core/Reflection/BaseReflectionObject.h"

class TiXmlElement;

namespace GASS
{
	class ScenarioScene;
	class MessageManager;

	class GASSExport Scenario : public Reflection<Scenario, BaseReflectionObject>
	{
	public:
		Scenario();
		virtual ~Scenario();
		bool Load(const std::string &filename);
		std::string GetPath(){return m_ScenarioPath;}
		void OnUpdate(double delta_time);
		ScenarioScene* GetScene(int i) { return m_Scenes[i];}
		int GetNumScenes(){ return (int) m_Scenes.size();}
	protected:
		ScenarioScene* LoadScene(TiXmlElement *sm_elem);
		std::vector<ScenarioScene*> m_Scenes;
		MessageManager* m_ScenarioMessageManager;
		std::string m_ScenarioPath;
	};
}

