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
		for(int i = 0; i < m_Scenes.size();i++)
		{
			m_Scenes[i]->Shutdown();
		}
	}

	bool Scenario::Load(const std::string &scenario_path)
	{
		m_ScenarioPath = scenario_path;

		MessagePtr system_msg(new ScenarioAboutToLoadNotifyMessage(shared_from_this()));
		SimEngine::Get().GetSimSystemManager()->SendImmediate(system_msg);

		ResourceSystemPtr rs = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<IResourceSystem>();
		if(rs == NULL)
			Log::Error("No Resource Manager Found");
		rs->AddResourceLocation(scenario_path,"GASSScenario","FileSystem",true);
		rs->LoadResourceGroup("GASSScenario");

		std::string filename = scenario_path + "/scenario.xml";

		//Load scenario specific templates, filename should probably be a scenario parameter
		SimEngine::Get().GetSimObjectManager()->Load(scenario_path + "/templates.xml");


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
					m_Scenes.push_back(scene);
				}
				scene_elem= scene_elem->NextSiblingElement();
			}
		}
		xmlDoc->Clear();
		//Delete our allocated document and return success ;)
		delete xmlDoc;

		

		//load all scenes
		for(int i = 0; i < m_Scenes.size();i++)
		{
			m_Scenes[i]->OnLoad();
		}
		return 1;
	}


	bool Scenario::Save(const std::string &scenario_path)
	{
		m_ScenarioPath = scenario_path;
		

		TiXmlDocument doc;  
		TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );  
		doc.LinkEndChild( decl ); 

		TiXmlElement * scenario_elem = new TiXmlElement("Scenario");  
		doc.LinkEndChild( scenario_elem); 

		TiXmlElement * ss_elem = new TiXmlElement( "ScenarioSettings" );  
		scenario_elem->LinkEndChild(ss_elem);

		BaseReflectionObject::SaveProperties(ss_elem);

		TiXmlElement * scenes_elem = new TiXmlElement("Scenes");  
		scenario_elem->LinkEndChild(scenes_elem);

		for(int i = 0; i < m_Scenes.size();i++)
		{
			m_Scenes[i]->SaveXML(scenes_elem);
		}

		std::string filename = scenario_path + "/scenario.xml";
		doc.SaveFile(filename.c_str());

		//Save scenario specific object templates, filename should probably be a scenario parameter
		//SimEngine::Get().GetSimObjectManager()->Load(scenario_path + "/templates.xml");
		return true;
	}

	ScenarioScenePtr Scenario::LoadScene(TiXmlElement *scene_elem)
	{
		std::string scene_name = scene_elem->Value();
		
		ScenarioScenePtr scene(new ScenarioScene(shared_from_this()));
		scene->OnCreate();
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

	ScenarioSceneIterator Scenario::GetScenarioScenes()
	{
		return ScenarioSceneIterator(m_Scenes.begin(),m_Scenes.end());
	}

	void Scenario::AddScenarioScene(ScenarioScenePtr scene)
	{
		m_Scenes.push_back(scene);
	}
}
