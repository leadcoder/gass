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

#include <boost/enable_shared_from_this.hpp>
#include "Sim/Common.h"
#include "Core/Math/Vector.h"
#include "Core/Reflection/BaseReflectionObject.h"
#include "Core/Utils/Iterators.h"

class TiXmlElement;

namespace GASS
{
	class ScenarioScene;
	class MessageManager;

	typedef boost::shared_ptr<ScenarioScene> ScenarioScenePtr;
	typedef std::vector<ScenarioScenePtr> ScenarioSceneVector;
	typedef VectorIterator<ScenarioSceneVector> ScenarioSceneIterator;

	/**
		A scenario in GASS is divided in scenarios-scenes. The scenario class is
		therefore only a container of scenario-scenes and the actual scenario 
		functionality is capsulated in its scenario-scenes,
		By dividing the scenario in scenes the user can have different 
		representations of the same scenario, for instance one visual representation
		and one infrared. In another application the user might want to have a 
		separate scene for the menu-system or divided the scenario in different zones
		each represented by it's own scenario-scene.
		See ScenarioScene class for more information about scenario scenes. 

	*/

	class GASSExport Scenario : public Reflection<Scenario, BaseReflectionObject>, public boost::enable_shared_from_this<Scenario>
	{
	public:
		Scenario();
		virtual ~Scenario();
		/**
		Load a new scenario from path
		*/
		bool Load(const std::string &scenario_parh);

		/**
		Save scenario to path
		*/
		bool Save(const std::string &name);
		/**
		Get path to current loaded scenario
		@param absolute or relative path to the scenario
		*/
		std::string GetPath(){return m_ScenarioPath;}
		
		/**
		Update the scenario
		@param delta_time time scince lat update
		*/
		void OnUpdate(double delta_time);

		/**
		Get iterator to all scenario scenes.
		*/

		ScenarioSceneIterator GetScenarioScenes();

		/**
			Add scene to scenario, 
		*/
		void AddScenarioScene(ScenarioScenePtr scene);
	protected:
		ScenarioScenePtr LoadScene(TiXmlElement *sm_elem);
		ScenarioSceneVector m_Scenes;
		std::string m_ScenarioPath;
	};
	typedef boost::shared_ptr<Scenario> ScenarioPtr;
	typedef boost::weak_ptr<Scenario> ScenarioWeakPtr;
}

