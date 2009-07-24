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
#include "Core/ComponentSystem/BaseComponentContainerTemplateManager.h"
#include "Core/ComponentSystem/ComponentContainerFactory.h"

#include "Core/Utils/Log.h"
#include "tinyxml.h"

namespace GASS
{
	SceneObjectManager::SceneObjectManager(ScenarioScene* ss)
	{
		m_ScenarioScene = ss;
		m_Root = SceneObjectPtr( new SceneObject());
	}

	SceneObjectManager::~SceneObjectManager()
	{
		
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
	
		obj->SetSceneObjectManager(this);
		obj->OnCreate();
		
		if(!obj->GetParent()) // we are top level object
			m_Root->AddChild(obj);
	
		//Send load message so that all scene manager can initilze it's components
		MessagePtr load_msg(new Message(ScenarioScene::SCENARIO_NM_SCENE_OBJECT_CREATED));
		load_msg->SetData("SceneObject",obj);
		m_ScenarioScene->SendImmediate(load_msg);

		//Move this to user scene manager!!
		MessagePtr obj_msg(new Message(SceneObject::OBJECT_RM_LOAD_USER_COMPONENTS));
		obj->SendImmediate(obj_msg);
		//Pump initial messages around
		obj->SyncMessages(0);
		//send load message for all child game object also?

		IComponentContainer::ComponentContainerIterator children = obj->GetChildren();
		while(children.hasMoreElements())
		{
			SceneObjectPtr child = boost::shared_static_cast<SceneObject>(children.getNext());
			LoadObject(child); 
		}
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

	SceneObjectPtr SceneObjectManager::LoadFromTemplate(const std::string &go_template_name, SceneObjectPtr parent)
	{
		SceneObjectPtr go = boost::shared_static_cast<SceneObject>(SimEngine::Get().GetSimObjectManager()->CreateFromTemplate(go_template_name));
		if(go)
		{
			if(parent)
			{
				//go->SetParent(parent);
				parent->AddChild(go);
				
			}
			LoadObject(go);
		}
		return go;
	}

	void SceneObjectManager::SyncMessages(double delta_time)
	{
		m_Root->SyncMessages(delta_time);
	}

	void SceneObjectManager::Clear()
	{
		UnloadObject(m_Root);
	}

	void SceneObjectManager::UnloadObject(SceneObjectPtr obj)
	{
		
		MessagePtr msg(new Message(SceneObject::OBJECT_RM_UNLOAD_COMPONENTS));
		obj->SendImmediate(msg);

		//notify that this object is to be removed
		MessagePtr unload_msg(new Message(ScenarioScene::SCENARIO_NM_SCENE_OBJECT_REMOVED));
		unload_msg->SetData("SceneObject",obj);
		m_ScenarioScene->SendImmediate(unload_msg);
	}

	void SceneObjectManager::DeleteObject(SceneObjectPtr obj)
	{
		BaseComponentContainer::ComponentContainerIterator children = obj->GetChildren();
		while(children.hasMoreElements())
		{
			SceneObjectPtr child = boost::shared_static_cast<SceneObject>(children.getNext());
			DeleteObject(child);
			//update iteratrator
			children = obj->GetChildren();
		}

		UnloadObject(obj);

		if(obj->GetParent())
		{
			obj->GetParent()->RemoveChild(obj);
		}
	}
}
