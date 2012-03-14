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
#include "Sim/Scenario/Scenario.h"
#include "Core/Math/Vector.h"

class TiXmlElement;

namespace GASS
{
	class MessageManager;
	class SceneObject;
	class Scenario;

	typedef boost::shared_ptr<Scenario> ScenarioPtr;
	typedef boost::weak_ptr<Scenario> ScenarioWeakPtr;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;

	/**
		The Scene object manager is owned by a scenario 
		and could be seen as a decomposition (or helper class)
		of the Scenario class.
		In future this class may be merged into the
		scenario scene class.
	*/
	class GASSExport SceneObjectManager : public boost::enable_shared_from_this<SceneObjectManager>
	{
		friend class Scenario;
	public:
		SceneObjectManager(ScenarioPtr scenario);
		virtual ~SceneObjectManager();

		void LoadXML(const std::string &filename);
		void SaveXML(const std::string &filename);

		SceneObjectPtr LoadFromTemplate(const std::string &go_template_name, SceneObjectPtr parent = SceneObjectPtr());
		void SyncMessages(double delta_time);
		ScenarioPtr GetScenario() const {return ScenarioPtr(m_Scenario,boost::detail::sp_nothrow_tag());}
		void Clear();
		void Init();
		void DeleteObject(SceneObjectPtr obj);
		SceneObjectPtr GetSceneRoot() {return m_Root;}
		void LoadObject(SceneObjectPtr obj);
	protected:
		void LoadXML(TiXmlElement *parent);
		void SaveXML(TiXmlElement *parent) const;
		//void GetObjectsByClass(SceneObjectPtr obj, std::vector<SceneObjectPtr> &objects, const std::string &class_name);
		void UnloadObject(SceneObjectPtr obj);
		SceneObjectPtr LoadSceneObjectXML(TiXmlElement *go_elem);
		void Load(TiXmlElement *scene_elem);
		//ISceneManager* LoadSceneManager(TiXmlElement *sm_elem);
		ScenarioWeakPtr m_Scenario;
		SceneObjectPtr m_Root;
	};
}

