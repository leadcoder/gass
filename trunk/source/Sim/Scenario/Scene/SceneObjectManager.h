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
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Core/Math/Vector.h"

class TiXmlElement;

namespace GASS
{
	class MessageManager;
	class SceneObject;
	class ScenarioScene;

	typedef boost::shared_ptr<ScenarioScene> ScenarioScenePtr;
	typedef boost::weak_ptr<ScenarioScene> ScenarioSceneWeakPtr;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;

	/**
		The Scene object manager is owned by a scenario scene 
		and could be seen as a decomposition (or helper class)
		of the ScenarioScene class. 
		In future this class may be merged into the 
		scenario scene class.
	*/
	class GASSExport SceneObjectManager 
	{
		friend class ScenarioScene;
	public:
		SceneObjectManager(ScenarioScenePtr ss);
		virtual ~SceneObjectManager();
		bool LoadFromFile(const std::string filename);
		SceneObjectPtr LoadFromTemplate(const std::string &go_template_name, SceneObjectPtr parent = SceneObjectPtr());
		void SyncMessages(double delta_time);
		ScenarioScenePtr GetScenarioScene() const {return ScenarioScenePtr(m_ScenarioScene,boost::detail::sp_nothrow_tag());}
		void Clear();
		void DeleteObject(SceneObjectPtr obj);
		SceneObjectPtr GetSceneRoot() {return m_Root;}
	protected:
		//void GetObjectsByClass(SceneObjectPtr obj, std::vector<SceneObjectPtr> &objects, const std::string &class_name);
		void LoadObject(SceneObjectPtr obj);
		void UnloadObject(SceneObjectPtr obj);
		SceneObjectPtr LoadSceneObject(TiXmlElement *go_elem);
		void Load(TiXmlElement *scene_elem);
		//ISceneManager* LoadSceneManager(TiXmlElement *sm_elem);
		ScenarioSceneWeakPtr m_ScenarioScene;
		SceneObjectPtr m_Root;
	};
}

