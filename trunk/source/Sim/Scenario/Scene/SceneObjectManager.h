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
	class ISceneManager;
	class MessageManager;
	class SceneObject;

	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;

	class GASSExport SceneObjectManager 
	{
		friend class ScenarioScene;
	public:
		SceneObjectManager(ScenarioScene* ss);
		virtual ~SceneObjectManager();
		bool LoadFromFile(const std::string filename);
		MessageManager* GetMessageManager();
		SceneObjectPtr GetObjectByName(const std::string &name);
		SceneObjectPtr LoadFromTemplate(const std::string &go_template_name);
		unsigned int GetNumObjects() {return (unsigned int) m_SceneObjectVector.size();}
		SceneObjectPtr GetObject(unsigned int index) {return m_SceneObjectVector[index];}
		void SyncMessages(double delta_time);
		ScenarioScene* GetScenarioScene() const {return m_ScenarioScene;}
		void GetObjectsByClass(std::vector<SceneObjectPtr> &objects, const std::string &class_name);
		void Clear();
		void DeleteObject(SceneObjectPtr obj);
	protected:
		void GetObjectByClass(SceneObjectPtr obj, std::vector<SceneObjectPtr> &objects, const std::string &class_name);
		SceneObjectPtr GetObjectByName(SceneObjectPtr obj, const std::string &name);
		void GetObjectsByClass(SceneObjectPtr obj, std::vector<SceneObjectPtr> &objects, const std::string &class_name);
		void LoadObject(SceneObjectPtr obj);
		void UnloadObject(SceneObjectPtr obj);
		SceneObjectPtr LoadSceneObject(TiXmlElement *go_elem);
		void Load(TiXmlElement *scene_elem);
		ISceneManager* LoadSceneManager(TiXmlElement *sm_elem);
		ScenarioScene* m_ScenarioScene;
		std::vector<SceneObjectPtr> m_SceneObjectVector;
	};
}

