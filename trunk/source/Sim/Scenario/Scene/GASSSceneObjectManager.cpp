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



#include "Sim/Scenario/Scene/GASSSceneObjectManager.h"
#include "Sim/Scenario/Scene/GASSSceneManagerFactory.h"
#include "Sim/Scenario/Scene/GASSISceneManager.h"
#include "Sim/Scenario/Scene/GASSSceneObject.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Components/GASSBaseSceneComponent.h"

#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/ComponentSystem/GASSBaseComponentContainerTemplateManager.h"
#include "Core/ComponentSystem/GASSComponentContainerFactory.h"

#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSException.h"
#include "tinyxml.h"

namespace GASS
{
	SceneObjectManager::SceneObjectManager(ScenarioPtr scenario) : m_Scenario (scenario)
	{
		

	}

	SceneObjectManager::~SceneObjectManager()
	{

	}

	void SceneObjectManager::LoadXML(const std::string &filename)
	{
		if(filename =="") 
			GASS_EXCEPT(Exception::ERR_INVALIDPARAMS,"No filename provided", "SceneObjectManager::LoadXML");
		
		TiXmlDocument *xmlDoc = new TiXmlDocument(filename.c_str());

		if(!xmlDoc->LoadFile())
		{
			//Fatal error, cannot load
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Couldn't load: " +  filename, "SceneObjectManager::LoadXML");
		}

		//LoadXML((TiXmlElement*)xmlDoc);
		TiXmlElement *objects = xmlDoc->FirstChildElement("Objects");
		if(objects == NULL) 
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to get Object tag", "SceneObjectManager::LoadXML");

		TiXmlElement *object_elem = objects->FirstChildElement();
		//Loop through each template
		while(object_elem)
		{
			SceneObjectPtr obj = LoadSceneObjectXML(object_elem);
			if(obj)
			{
				LoadObject(obj);
			}
			object_elem= object_elem->NextSiblingElement();
		}
		xmlDoc->Clear();
		//Delete our allocated document and return success ;)
		delete xmlDoc;
	}

	void SceneObjectManager::SaveXML(const std::string &filename)
	{
		TiXmlDocument doc;
		TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
		doc.LinkEndChild( decl );

		//SaveXML((TiXmlElement*)&doc);
		TiXmlElement *som_elem = new TiXmlElement("Objects");
		doc.LinkEndChild(som_elem);
		IComponentContainer::ComponentContainerIterator iter = m_Root->GetChildren();
		while(iter.hasMoreElements())
		{
			SceneObjectPtr child = boost::shared_static_cast<SceneObject>(iter.getNext());
			child->SaveXML(som_elem);
		}

		doc.SaveFile(filename.c_str());
	}

	void SceneObjectManager::LoadXML(TiXmlElement *parent)
	{
		TiXmlElement *objects = parent->FirstChildElement("Objects");
		if(objects == NULL)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to get Objects tag", "SceneObjectManager::LoadXML");
	
		TiXmlElement *object_elem = objects->FirstChildElement();
		//Loop through each template
		while(object_elem)
		{
			SceneObjectPtr obj = LoadSceneObjectXML(object_elem);
			if(obj)
			{
				LoadObject(obj);
			}
			object_elem= object_elem->NextSiblingElement();
		}
	}

	void SceneObjectManager::SaveXML(TiXmlElement *parent) const
	{
		TiXmlElement *som_elem = new TiXmlElement("Objects");
		parent->LinkEndChild(som_elem);
		IComponentContainer::ComponentContainerIterator iter = m_Root->GetChildren();
		while(iter.hasMoreElements())
		{
			SceneObjectPtr child = boost::shared_static_cast<SceneObject>(iter.getNext());
			child->SaveXML(som_elem);
		}
	}

	void SceneObjectManager::LoadObject(SceneObjectPtr obj)
	{
		obj->SetSceneObjectManager(shared_from_this());
		obj->OnCreate();

		if(!obj->GetParent()) // we are top level object
			m_Root->AddChild(obj);

		//Send load message so that all scene manager can initilze it's components

		MessagePtr load_msg(new SceneObjectCreatedNotifyMessage(obj));
		ScenarioPtr scenario = GetScenario();
		scenario->SendImmediate(load_msg);

		//Pump initial messages around
		obj->SyncMessages(0,false);
		//send load message for all child game object also?

		IComponentContainer::ComponentContainerIterator children = obj->GetChildren();
		while(children.hasMoreElements())
		{
			SceneObjectPtr child = boost::shared_static_cast<SceneObject>(children.getNext());
			LoadObject(child);
		}
	}


	SceneObjectPtr SceneObjectManager::LoadSceneObjectXML(TiXmlElement *so_elem)
	{
		//check if we want create object by template or type
		std::string so_name = so_elem->Value();
		SceneObjectPtr so;
		if(so_elem->Attribute("from_template"))
		{
			std::string template_name = so_elem->Attribute("from_template");
			so = boost::shared_static_cast<SceneObject>(SimEngine::Get().GetSimObjectManager()->CreateFromTemplate(template_name));
		}
		else
		{
			so = boost::shared_static_cast<SceneObject>(ComponentContainerFactory::Get().Create(so_name));
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
		DeleteObject(m_Root);
		m_Root.reset();
		Init();
	}

	void SceneObjectManager::Init()
	{
        m_Root = SceneObjectPtr( new SceneObject());
		m_Root->SetName("Root");
		m_Root->SetSceneObjectManager(shared_from_this());
	}

	void SceneObjectManager::UnloadObject(SceneObjectPtr obj)
	{
		MessagePtr msg(new UnloadComponentsMessage());
		obj->SendImmediate(msg);

		//notify that this object is to be removed
		MessagePtr unload_msg(new SceneObjectRemovedNotifyMessage(obj));
		if(GetScenario())
			GetScenario()->SendImmediate(unload_msg);
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
