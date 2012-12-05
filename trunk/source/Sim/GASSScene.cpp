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



#include "Sim/GASSScene.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneManagerFactory.h"
#include "Sim/Interface/GASSISceneManager.h"
#include "Sim/Messages/GASSCoreSceneMessages.h"
#include "Sim/Messages/GASSGraphicsSceneMessages.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/Interface/GASSIResourceSystem.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"

#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSException.h"
#include "Core/System/GASSISystem.h"
#include "Core/ComponentSystem/GASSBaseComponentContainerTemplateManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Serialize/GASSIXMLSerialize.h"
#include "Core/Utils/GASSFilePath.h"
#include "tinyxml.h"

namespace GASS
{
	Scene::Scene() : m_StartPos(Vec3(0,0,0)),
		m_StartRot(Vec3(0,0,0)),
		m_SceneMessageManager(new MessageManager()),
		m_SceneLoaded(false),
		m_CreateCalled(false)
		
	{
		
	}

	Scene::~Scene()
	{
		//Unload();
	}

	void Scene::RegisterReflection()
	{
		RegisterProperty<Vec3>("StartPosition", &Scene::GetStartPos, &Scene::SetStartPos);
		RegisterProperty<Vec3>("StartRotation", &Scene::GetStartRot, &Scene::SetStartRot);
	}

	void Scene::Create()
	{
		m_SceneMessageManager->RegisterForMessage(typeid(RemoveSceneObjectMessage), TYPED_MESSAGE_FUNC(Scene::OnRemoveSceneObject,RemoveSceneObjectMessage),0);
		m_SceneMessageManager->RegisterForMessage(typeid(SpawnObjectFromTemplateMessage),TYPED_MESSAGE_FUNC(Scene::OnSpawnSceneObjectFromTemplate,SpawnObjectFromTemplateMessage),0);
		m_CreateCalled = true;
	}

	void Scene::Load(const std::string &name)
	{
		m_Name = name;
		if(!m_CreateCalled)
		{
			GASS_EXCEPT(Exception::ERR_INVALID_STATE,
				"You must call Create before using the scene class",
				"Scene::Load");
		}

		if(m_SceneLoaded)
		{
			Unload();
		}

		//Add all registered scene manangers to the scene
		std::vector<std::string> managers = SceneManagerFactory::GetPtr()->GetFactoryNames();
		for(size_t i = 0; i < managers.size();i++)
		{
			SceneManagerPtr sm = SceneManagerFactory::GetPtr()->Create(managers[i]);
			sm->SetScene(shared_from_this());
			sm->SetName(managers[i]);
			sm->OnCreate();
			m_SceneManagers.push_back(sm);
		}

		m_Root = SceneObjectPtr(new SceneObject());
		m_Root->SetName("Root");
		m_Root->Initialize(shared_from_this());

		SceneObjectPtr  scenery = SceneObjectPtr(new SceneObject());
		scenery->SetName("Scenery");
		scenery->SetID("SCENERY_ROOT");
		m_TerrainObjects = scenery;
		if(name != "")
		{
			FilePath scene_path(SimEngine::Get().GetScenePath().GetFullPath() + "/"  + name);

			ResourceSystemPtr rs = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<IResourceSystem>();
			if(rs == NULL)
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"No Resource Manager Found", "Scene::Load");

			rs->AddResourceLocation(scene_path,"GASSSceneResGroup","FileSystem",true);
			const FilePath filename = FilePath(scene_path.GetFullPath() + "/scene.xml");

			//Load scene specific templates, filename should probably be a scene parameter
			SimEngine::Get().GetSceneObjectTemplateManager()->Load(scene_path.GetFullPath() + "/templates.xml");

			TiXmlDocument *xmlDoc = new TiXmlDocument(filename.GetFullPath().c_str());
			if(!xmlDoc->LoadFile())
			{
				GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Couldn't load: " + filename.GetFullPath(), "Scene::Load");
			}

			TiXmlElement *scene = xmlDoc->FirstChildElement("Scene");
			if(scene == NULL)
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to get Scene tag", "Scene::Load");

			LoadXML(scene);
			xmlDoc->Clear();
			//Delete our allocated document
			delete xmlDoc;
			rs->LoadResourceGroup("GASSSceneResGroup");
		}
	
		MessagePtr enter_load_msg(new SceneAboutToLoadNotifyMessage(shared_from_this()));
		SimEngine::Get().GetSimSystemManager()->SendImmediate(enter_load_msg);
		MessagePtr scene_msg(new LoadSceneManagersMessage(shared_from_this()));
		//send load message
		SendImmediate(scene_msg);

		//load scene terrain instances
		if(name != "")
			scenery->LoadFromFile(GetSceneFolder().GetFullPath() + "/instances.xml");
		
		m_Root->AddChildSceneObject(scenery,true);
		MessagePtr system_msg(new SceneLoadedNotifyMessage(shared_from_this()));
		SimEngine::Get().GetSimSystemManager()->SendImmediate(system_msg);
		m_SceneLoaded = true;
	}


	FilePath Scene::GetSceneFolder() const
	{
		return FilePath(SimEngine::Get().GetScenePath().GetFullPath() + "/" + m_Name);
	}

	void Scene::Save(const FilePath &scene_path)
	{
		
		TiXmlDocument doc;  
		TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );  
		doc.LinkEndChild( decl ); 

		TiXmlElement * scene_elem = new TiXmlElement("Scene");  
		doc.LinkEndChild( scene_elem); 

		BaseReflectionObject::SaveProperties(scene_elem);

		TiXmlElement * sms_elem = new TiXmlElement("SceneManagerSettings");  
		scene_elem->LinkEndChild(sms_elem);

		SaveXML(sms_elem);

		const FilePath filename = FilePath(scene_path.GetFullPath() + "/Scene.xml");
		doc.SaveFile(filename.GetFullPath().c_str());
		
		if(SceneObjectPtr(m_TerrainObjects))
			SceneObjectPtr(m_TerrainObjects)->SaveToFile(scene_path.GetFullPath() + "/instances.xml");
		//Save scene specific object templates, filename should probably be a scene parameter
		//SimEngine::Get().GetSceneObjectTemplateManager()->Load(scene_path + "/templates.xml");
	}

	void Scene::SyncMessages(double delta_time) const
	{
		if(m_SceneLoaded)
		{
			m_SceneMessageManager->Update(delta_time);
			m_Root->SyncMessages(delta_time);
		}
	}

	void Scene::LoadXML(TiXmlElement *scene)
	{
		TiXmlElement *prop = scene->FirstChildElement("Properties");
		if(prop)
			BaseReflectionObject::LoadProperties(prop);
		else // fallback for old scenes
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to get Properties tag", "Scene::LoadXML");
		TiXmlElement *scene_manager = scene->FirstChildElement("SceneManagerSettings");
		if(scene_manager)
		{
			scene_manager = scene_manager->FirstChildElement();
			//Load scene manager settings
			while(scene_manager)
			{
				SceneManagerPtr sm = LoadSceneManager(scene_manager);
				scene_manager = scene_manager->NextSiblingElement();
			}
		}
		else
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to get SceneManagerSettings tag", "Scene::LoadXML");
	}

	void Scene::SaveXML(TiXmlElement *parent)
	{
		//Create
		TiXmlElement *scene = new TiXmlElement("Scene");
		parent->LinkEndChild(scene);

		TiXmlElement *prop = new TiXmlElement("Properties");
		scene->LinkEndChild(prop);

		BaseReflectionObject::SaveProperties(prop);

		TiXmlElement *sms_elem = new TiXmlElement("SceneManagerSettings");
		scene->LinkEndChild(sms_elem);

		for(int i  = 0 ; i < m_SceneManagers.size();i++)
		{
			SceneManagerPtr sm = m_SceneManagers[i];
			XMLSerializePtr serialize = boost::shared_dynamic_cast<IXMLSerialize>(sm);
			if(serialize)
				serialize->SaveXML(sms_elem);
		}
		//std::string scene_path = GetPath();
		//save instances at same place
	}

	void Scene::Unload()
	{
		if(m_SceneLoaded)
		{
			m_Root->OnDelete();
			m_Root.reset();
			//m_Root = SceneObjectPtr( new SceneObject());
			//m_Root->SetName("Root");
			//m_Root->Initialize(shared_from_this());
			//m_ObjectManager->Clear();
			
			MessagePtr scene_msg(new UnloadSceneManagersMessage(shared_from_this()));
			m_SceneMessageManager->SendImmediate(scene_msg);
			MessagePtr unload_msg(new SceneUnloadNotifyMessage(shared_from_this()));
			SimEngine::Get().GetSimSystemManager()->SendImmediate(unload_msg);
			m_SceneMessageManager->Clear(); 

			//recreate?
			

			ResourceSystemPtr rs = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<IResourceSystem>();
			if(rs == NULL)
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"No Resource Manager Found", "Scene::SaveXML");
			rs->RemoveResourceGroup("GASSSceneResGroup");
			m_SceneLoaded = false;
			m_SceneManagers.clear();

			m_SceneMessageManager = MessageManagerPtr(new MessageManager());
			m_SceneMessageManager->RegisterForMessage(typeid(RemoveSceneObjectMessage), TYPED_MESSAGE_FUNC(Scene::OnRemoveSceneObject,RemoveSceneObjectMessage),0);
			m_SceneMessageManager->RegisterForMessage(typeid(SpawnObjectFromTemplateMessage),TYPED_MESSAGE_FUNC(Scene::OnSpawnSceneObjectFromTemplate,SpawnObjectFromTemplateMessage),0);
		}
	}

	void Scene::Load()
	{
		
	}

	SceneManagerPtr Scene::LoadSceneManager(TiXmlElement *sm_elem)
	{
		SceneManagerPtr sm;
		std::string sm_name = sm_elem->Value();
		for(int i = 0; i < m_SceneManagers.size() ; i++)
		{
			if(m_SceneManagers[i]->GetName() == sm_name)
			{
				sm = m_SceneManagers[i];
				XMLSerializePtr serialize = boost::shared_dynamic_cast<IXMLSerialize>(sm);
				if(serialize)
					serialize->LoadXML(sm_elem);
				return sm;
			}
		}
		return sm;
	}

	SceneManagerPtr Scene::GetSceneManagerByName(const std::string &name) const
	{
		for(int i = 0; i < m_SceneManagers.size(); i++)
		{
			if(m_SceneManagers[i]->GetName() == name)
				return m_SceneManagers[i];
		}
		SceneManagerPtr empty;
		return empty;
	}

	void Scene::OnSpawnSceneObjectFromTemplate(SpawnObjectFromTemplateMessagePtr message)
	{
		std::string obj_template = message->GetTemplateName();
		SceneObjectPtr so = LoadObjectFromTemplate(obj_template,message->GetParent());
		if(so)
		{
			Vec3 pos = message->GetPosition();
			Quaternion rot = message->GetRotation();
			Vec3 vel = message->GetVelocity();
			int sender_id = (int) this;

			MessagePtr pos_msg(new PositionMessage(pos,sender_id));
			MessagePtr rot_msg(new RotationMessage(rot,sender_id));
			MessagePtr vel_msg(new PhysicsBodyMessage(PhysicsBodyMessage::VELOCITY,vel,sender_id));

			so->SendImmediate(pos_msg);
			so->SendImmediate(rot_msg);
			so->SendImmediate(vel_msg);
		}
	}

	SceneObjectPtr Scene::LoadObjectFromTemplate(const std::string &template_name, SceneObjectPtr parent)
	{
		SceneObjectPtr so = SimEngine::Get().CreateObjectFromTemplate(template_name);
		if(so)
		{
			if(parent)
			{
				parent->AddChildSceneObject(so,true);
			}
			else
				m_Root->AddChildSceneObject(so,true);
		}
		return so;
	}

	void Scene::OnRemoveSceneObject(RemoveSceneObjectMessagePtr message)
	{
		SceneObjectPtr so = message->GetSceneObject();
		if(so)
			so->GetParentSceneObject()->RemoveChildSceneObject(so);
	}

	SceneManagerIterator Scene::GetSceneManagers()
	{
		return SceneManagerIterator(m_SceneManagers.begin(),m_SceneManagers.end());
	}

	int Scene::RegisterForMessage(const MessageType &type, MessageFuncPtr callback, int priority )
	{
		return m_SceneMessageManager->RegisterForMessage(type, callback, priority);
	}

	void Scene::UnregisterForMessage(const MessageType &type, MessageFuncPtr callback)
	{
		m_SceneMessageManager->UnregisterForMessage(type, callback);
	}

	void Scene::PostMessage( MessagePtr message )
	{
		m_SceneMessageManager->PostMessage(message);
	}

	void Scene::SendImmediate( MessagePtr message )
	{
		m_SceneMessageManager->SendImmediate(message);
	}

	size_t Scene::GetQueuedMessages() const
	{
		size_t num = m_SceneMessageManager->GetQueuedMessages();
		num += m_Root->GetQueuedMessages();
		return num;
	}
	
	std::vector<std::string> Scene::GetScenes(const FilePath &path)
	{
		boost::filesystem::path boost_path(path.GetFullPath()); 

		std::vector<std::string> scene_names;
		if(boost::filesystem::exists(boost_path))  
		{
			boost::filesystem::directory_iterator end ;    
			for( boost::filesystem::directory_iterator iter(boost_path) ; iter != end ; ++iter )      
			{
				if (boost::filesystem::is_directory( *iter ) )      
				{   
					if(boost::filesystem::exists(boost::filesystem::path(iter->path().string() + "/scene.xml")))
					{
						std::string scene_name = iter->path().filename().generic_string();
						scene_names.push_back(scene_name);
						//m_Scenes.push_back(iter->path().string());
					}
				}
			}
		}
		return scene_names;
	}
}
