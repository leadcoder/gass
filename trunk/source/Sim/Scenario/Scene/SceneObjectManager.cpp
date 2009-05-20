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
#include "Sim/Components/BaseSceneComponent.h"

#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/Message.h"
#include "Core/ComponentSystem/BaseObjectTemplateManager.h"
#include "Core/ComponentSystem/ComponentContainerFactory.h"

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

		//check if we want create object by template or type
		std::string so_name = so_elem->Value();
		SceneObjectPtr so;
		if(so_elem->Attribute("type"))
		{
			std::string object_type = so_elem->Attribute("type");
			so = boost::shared_static_cast<SceneObject>(ComponentContainerFactory::Get().Create(object_type));
		}
		else
		{
			so = boost::shared_static_cast<SceneObject>(SimEngine::Get().GetSimObjectManager()->CreateFromTemplate(so_name));
		}
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

	void SceneObjectManager::GetObjectsByClass(std::vector<SceneObjectPtr> &objects, const std::string &class_name)
	{
		for(int i =  0 ; i < m_SceneObjectVector.size();i++)
		{
			GetObjectByClass(m_SceneObjectVector[i],objects,class_name);
		}
	}

	void SceneObjectManager::GetObjectByClass(SceneObjectPtr obj, std::vector<SceneObjectPtr> &objects, const std::string &class_name)
	{
		if(obj->GetRTTI()->IsDerivedFrom(class_name))
			objects.push_back(obj);

		//Check all components
		BaseObject::ComponentVector components =  obj->GetComponents();
		BaseObject::ComponentVector::iterator comp_iter = components.begin();
		for(;comp_iter != components.end();comp_iter++)
		{
			BaseSceneComponentPtr comp = boost::shared_static_cast<BaseSceneComponent>(*comp_iter);
			if(comp->GetRTTI()->IsDerivedFrom(class_name))
			{				
				objects.push_back(obj);
				break;
			}
		}
		
		BaseObject::ComponentContainerVector children = obj->GetChildren();
		BaseObject::ComponentContainerVector::iterator iter = children.begin();
		for(;iter != children.end();iter++)
		{
			SceneObjectPtr child = boost::shared_static_cast<SceneObject>(*iter);
			GetObjectByClass(child, objects, class_name);
		}
	}

	SceneObjectPtr SceneObjectManager::GetObjectByName(const std::string &name)
	{
		SceneObjectPtr obj;

		for(int i =  0 ; i < m_SceneObjectVector.size();i++)
		{
			if(GetObjectByName(m_SceneObjectVector[i],name))
			{
				obj = m_SceneObjectVector[i];
				break;
			}
		}
		return obj;
	}


	SceneObjectPtr SceneObjectManager::GetObjectByName(SceneObjectPtr obj, const std::string &name)
	{
		SceneObjectPtr ret;
		if(obj->GetName()== name)
			return obj;

		//check children also
		BaseObject::ComponentContainerVector children = obj->GetChildren();
		BaseObject::ComponentContainerVector::iterator iter = children.begin();
		for(;iter != children.end();iter++)
		{
			SceneObjectPtr child = boost::shared_static_cast<SceneObject>(*iter);
			if(GetObjectByName(child,name))
			{
					ret = child;
					break;
			}
		}
		return ret;
	}

	void SceneObjectManager::Clear()
	{
		//Send shutdonw message to all components
		for(int i =  0 ; i < m_SceneObjectVector.size();i++)
		{
			UnloadObject(m_SceneObjectVector[i]);
		}
		m_SceneObjectVector.clear();
	}

	void SceneObjectManager::UnloadObject(SceneObjectPtr obj)
	{
		int from_id = (int)this;
		MessagePtr msg(new Message(ScenarioScene::SM_MESSAGE_UNLOAD_COMPONENTS,from_id));
		obj->GetMessageManager()->SendImmediate(msg);

		MessagePtr unload_msg(new Message(ScenarioScene::SCENARIO_MESSAGE_UNLOAD_SCENE_OBJECT,from_id));
		unload_msg->SetData("SceneObject",obj);
		m_ScenarioScene->GetMessageManager()->SendImmediate(unload_msg);
	}


	void SceneObjectManager::DeleteObject(SceneObjectPtr obj)
	{
		UnloadObject(obj);
		std::vector<SceneObjectPtr>::iterator iter = m_SceneObjectVector.begin();
		for(; iter != m_SceneObjectVector.end();iter++)
		{
			if(obj == *iter)
			{
				m_SceneObjectVector.erase(iter);
				return;
			}
		}
	}

}
