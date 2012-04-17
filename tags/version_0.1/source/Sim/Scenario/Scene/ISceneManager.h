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

namespace GASS
{
	class Scenario;
	typedef boost::shared_ptr<Scenario> ScenarioPtr;
	typedef boost::weak_ptr<Scenario> ScenarioWeakPtr;

	/**
		Interface that all scene managers must implement.
		A scene manager in GASS is responsible for handling 
		a certain part of a scene, for example graphics, 
		physics or sound. Therefore a scene manager is owned
		by a scenario scene.
	*/

	class GASSExport ISceneManager
	{
	public:
		virtual ~ISceneManager(){}
		virtual void OnCreate() = 0;

		virtual void Update(double delta_time) = 0;
		virtual std::string GetName() const = 0;
		virtual void SetName(const std::string &name) = 0;
		virtual ScenarioPtr GetScenario() const = 0;
		virtual void SetScenario(ScenarioPtr owner) = 0;
	};

	typedef boost::shared_ptr<ISceneManager> SceneManagerPtr;
}
