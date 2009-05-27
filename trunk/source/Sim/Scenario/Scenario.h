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

#include "Sim/Common.h"
#include "Core/Math/Vector.h"
#include "Core/Reflection/BaseReflectionObject.h"

class TiXmlElement;

namespace GASS
{
	class ScenarioScene;
	class MessageManager;

	typedef boost::shared_ptr<ScenarioScene> ScenarioScenePtr;
	typedef std::vector<ScenarioScenePtr> ScenarioSceneVector;

	class GASSExport Scenario : public Reflection<Scenario, BaseReflectionObject>
	{
	public:
		Scenario();
		virtual ~Scenario();
		bool Load(const std::string &filename);
		std::string GetPath(){return m_ScenarioPath;}
		void OnUpdate(double delta_time);
		ScenarioSceneVector GetScenarioScenes() { return m_Scenes;}
	protected:
		ScenarioScenePtr LoadScene(TiXmlElement *sm_elem);
		ScenarioSceneVector m_Scenes;
		std::string m_ScenarioPath;
	};
}

