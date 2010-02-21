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



#include "Sim/Scenario/Scenario.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneManagerFactory.h"
#include "Sim/Scenario/Scene/ISceneManager.h"
#include "Sim/Systems/Resource/IResourceSystem.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/SimEngine.h"

#include "Core/Utils/Log.h"
#include "Core/System/ISystem.h"
#include "Core/ComponentSystem/BaseComponentContainerTemplateManager.h"
#include "tinyxml.h"

namespace GASS
{
	Scenario::Scenario()
	{

	}

	Scenario::~Scenario()
	{


	
	}

	bool Scenario::Load(const std::string &scenario_path)
	{
		m_ScenarioPath = scenario_path;

		//MessagePtr system_msg(new ScenarioAboutToLoadNotifyMessage(shared_from_this()));
		//SimEngine::Get().GetSimSystemManager()->SendImmediate(system_msg);

		ResourceSystemPtr rs = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<IResourceSystem>();
		if(rs == NULL)
			Log::Error("No Resource Manager Found");
		rs->AddResourceLocation(scenario_path,"GASSScenario","FileSystem",true);
		rs->LoadResourceGroup("GASSScenario");

		std::string filename = scenario_path + "/scenario.xml";

		if(filename =="") return false;
		TiXmlDocument *xmlDoc = new TiXmlDocument(filename.c_str());
		if(!xmlDoc->LoadFile())
		{
			//Fatal error, cannot load
			Log::Warning("SystemManager::Load() - Couldn't load: %s", filename.c_str());
			return 0;
		}
		TiXmlElement *scenario = xmlDoc->FirstChildElement("Scenario");
		if(scenario == NULL) Log::Error("Failed to get Scenario tag");

		TiXmlElement *settings = scenario->FirstChildElement("ScenarioSettings");
		if(settings == NULL) Log::Error("Failed to get ScenarioSettings tag");

		BaseReflectionObject::LoadProperties(settings);

		TiXmlElement *scene_elem = scenario->FirstChildElement("Scenes");

		if(scene_elem)
		{
			scene_elem = scene_elem->FirstChildElement();
			//Loop through each template
			while(scene_elem)
			{
				ScenarioScenePtr scene = LoadScene(scene_elem);
				if(scene)
				{
					scene->SetOwner(this);
					scene->OnCreate();
					m_Scenes.push_back(scene);
				}
				scene_elem= scene_elem->NextSiblingElement();
			}
		}
		xmlDoc->Clear();
		//Delete our allocated document and return success ;)
		delete xmlDoc;

		//Load scenario specific game object templates, filename should probably be a scenario parameter
		SimEngine::Get().GetSimObjectManager()->Load(scenario_path + "/templates.xml");

		//load all scenes
		for(int i = 0; i < m_Scenes.size();i++)
		{
			m_Scenes[i]->OnLoad();
		}
	
		return 1;
	}

	ScenarioScenePtr Scenario::LoadScene(TiXmlElement *scene_elem)
	{
		std::string scene_name = scene_elem->Value();
		
		ScenarioScenePtr scene(new ScenarioScene());
		if(scene)
		{
			scene->SetName(scene_name);
			scene->LoadXML(scene_elem);			
		}
		return scene;
	}

	void Scenario::OnUpdate(double delta_time)
	{
		for(int i = 0; i < m_Scenes.size();i++)
		{
			m_Scenes[i]->OnUpdate(delta_time);
		}
	}
}
