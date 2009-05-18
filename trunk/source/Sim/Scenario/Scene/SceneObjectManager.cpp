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



#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Scenario/Scene/SceneManagerFactory.h"
#include "Sim/Scenario/Scene/ISceneManager.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/SimEngine.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/Message.h"
#include "Core/ComponentSystem/BaseObjectTemplateManager.h"

#include "Core/Utils/Log.h"
#include "tinyxml.h"

namespace GASS
{
	SceneObjectManager::SceneObjectManager(ScenarioScene* ss)
	{
		m_ScenarioScene = ss;
	}

	SceneObjectManager::~SceneObjectManager()
	{
		
	}
	
	MessageManager* SceneObjectManager::GetMessageManager()
	{
		return m_ScenarioScene->GetMessageManager();
	}
	
	bool SceneObjectManager::LoadFromFile(const std::string filename)
	{
		if(filename =="") return false;
		TiXmlDocument *xmlDoc = new TiXmlDocument(filename.c_str());
		if(!xmlDoc->LoadFile())
		{
			//Fatal error, cannot load
			Log::Warning("SceneObjectManager::Load() - Couldn't load: %s", filename.c_str());
			return false;
		}
		TiXmlElement *objects = xmlDoc->FirstChildElement("Objects");
		if(objects == NULL) Log::Error("Failed to get Object tag");
		
		TiXmlElement *object_elem = objects->FirstChildElement();
		//Loop through each template
		while(object_elem)
		{
			SceneObjectPtr obj = LoadSceneObject(object_elem);
			if(obj)
			{
				LoadObject(obj);
				
			}
			object_elem= object_elem->NextSiblingElement();
		}
		xmlDoc->Clear();
		//Delete our allocated document and return success ;)
		delete xmlDoc;
		return true;
	}

	void SceneObjectManager::LoadObject(SceneObjectPtr obj)
	{
		//add some default messages to objects, this could also be done in each component but we save some jobb
		//by doing it here,  maybee a custom message manager for objects should be created?
		obj->GetMessageManager()->AddMessageToSystem(ScenarioScene::SM_MESSAGE_LOAD_GFX_COMPONENTS);
		obj->GetMessageManager()->AddMessageToSystem(ScenarioScene::SM_MESSAGE_LOAD_PHYSICS_COMPONENTS);
		obj->GetMessageManager()->AddMessageToSystem(ScenarioScene::SM_MESSAGE_LOAD_USER_COMPONENTS);

		obj->SetSceneObjectManager(this);
		obj->OnCreate();
		m_SceneObjectVector.push_back(obj);
		//Send load message so that all scene manager can initilze it's components
		int from_id = (int)this;
		

		MessagePtr load_msg(new Message(ScenarioScene::SCENARIO_MESSAGE_LOAD_SCENE_OBJECT,from_id));
		load_msg->SetData("SceneObject",obj);
		m_ScenarioScene->GetMessageManager()->SendImmediate(load_msg);

		//Move this to user scene manager!!
		MessagePtr obj_msg(new Message(ScenarioScene::SM_MESSAGE_LOAD_USER_COMPONENTS,from_id));
		obj->GetMessageManager()->SendImmediate(obj_msg);
		//Pump initial messages around
		obj->GetMessageManager()->Update(0);
		//send load message for all child game object also?

		BaseObject::ComponentContainerVector children = obj->GetChildren();
		BaseObject::ComponentContainerVector::iterator iter = children.begin();
		for(;iter != children.end();iter++)
		{
			SceneObjectPtr child = boost::shared_static_cast<SceneObject>(*iter);
			LoadObject(child); 
		}

		//inform every one that object is loaded
		/*MessagePtr sys_load_ness(new Message(ScenarioScene::SCENARIO_MESSAGE_LOAD_SCENE_OBJECT,from_id));
		sys_load_ness->SetData("SceneObject",obj);
		SimEngine::Get().GetSystemManager()->GetMessageManager()->SendImmidiate(sys_load_ness);*/
	}

	SceneObjectPtr SceneObjectManager::LoadSceneObject(TiXmlElement *so_elem)
	{
		std::string so_name = so_elem->Value();
		SceneObjectPtr so = boost::shared_static_cast<SceneObject>(SimEngine::Get().GetSimObjectManager()->CreateFromTemplate(so_name));
		if(so)
		{
			XMLSerializePtr s_so = boost::shared_dynamic_cast<IXMLSerialize>(so);
			if(s_so)
				s_so->LoadXML(so_elem);
		}
		return so;
	}

	SceneObjectPtr SceneObjectManager::LoadFromTemplate(const std::string &go_template_name)
	{
		SceneObjectPtr go = boost::shared_static_cast<SceneObject>(SimEngine::Get().GetSimObjectManager()->CreateFromTemplate(go_template_name));
		if(go)
		{
			LoadObject(go);
		}
		return go;
	}

	void SceneObjectManager::SyncMessages(double delta_time)
	{
		for(int i =  0 ; i < m_SceneObjectVector.size();i++)
		{
			m_SceneObjectVector[i]->SyncMessages(delta_time);
		}
	}

	SceneObjectPtr SceneObjectManager::GetObjectByName(const std::string &name)
	{
		for(int i =  0 ; i < m_SceneObjectVector.size();i++)
		{
			if(m_SceneObjectVector[i]->GetName()== name)
				return m_SceneObjectVector[i];
		}
		SceneObjectPtr empty;
		return empty;
	}

	void SceneObjectManager::Clear()
	{
		//Send shutdonw message to all components
		for(int i =  0 ; i < m_SceneObjectVector.size();i++)
		{
			int from_id = (int)this;
			MessagePtr msg(new Message(ScenarioScene::SM_MESSAGE_UNLOAD_COMPONENTS,from_id));
			m_SceneObjectVector[i]->GetMessageManager()->SendImmediate(msg);
		}
		m_SceneObjectVector.clear();
	}
}
