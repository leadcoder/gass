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



#include "Sim/Scene/GASSScene.h"
#include "Sim/Scene/GASSScene.h"
#include "Sim/Scene/GASSSceneManagerFactory.h"
#include "Sim/Scene/GASSISceneManager.h"
#include "Sim/Scene/GASSSceneObjectManager.h"
#include "Sim/Scene/GASSCoreSceneMessages.h"
#include "Sim/Scene/GASSGraphicsSceneMessages.h"
#include "Sim/Scene/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Scene/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Scene/GASSSceneObject.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "Sim/Systems/Resource/GASSIResourceSystem.h"
#include "Sim/Systems/GASSSimSystemManager.h"
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
		RegisterProperty<double>("OrigoOffsetEast", &Scene::GetOrigoOffsetEast, &Scene::SetOrigoOffsetEast);
		RegisterProperty<double>("OrigoOffsetNorth", &Scene::GetOrigoOffsetNorth, &Scene::SetOrigoOffsetNorth);
		RegisterProperty<std::string>("Projection", &Scene::GetProjection, &Scene::SetProjection);
	}


	void Scene::Create()
	{
		m_SceneMessageManager->RegisterForMessage(typeid(RemoveSceneObjectMessage), TYPED_MESSAGE_FUNC(Scene::OnRemoveSceneObject,RemoveSceneObjectMessage),0);
		m_SceneMessageManager->RegisterForMessage(typeid(SpawnObjectFromTemplateMessage),TYPED_MESSAGE_FUNC(Scene::OnSpawnSceneObjectFromTemplate,SpawnObjectFromTemplateMessage),0);

		m_ObjectManager = SceneObjectManagerPtr(new SceneObjectManager(shared_from_this()));
		m_ObjectManager->Init();

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
		m_CreateCalled = true;
	}



	void Scene::Load(const std::string &scene_path)
	{
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

		m_ScenePath = scene_path;
		ResourceSystemPtr rs = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<IResourceSystem>();
		if(rs == NULL)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"No Resource Manager Found", "Scene::Load");

		rs->AddResourceLocation(scene_path,"GASSScene","FileSystem",true);
		const std::string filename = scene_path + "/scene.xml";

		//Load scene specific templates, filename should probably be a scene parameter
		SimEngine::Get().GetSimObjectManager()->Load(scene_path + "/templates.xml");

		TiXmlDocument *xmlDoc = new TiXmlDocument(filename.c_str());
		if(!xmlDoc->LoadFile())
		{
			GASS_EXCEPT(Exception::ERR_CANNOT_READ_FILE,"Couldn't load: " + filename, "Scene::Load");
		}

		TiXmlElement *scene = xmlDoc->FirstChildElement("Scene");
		if(scene == NULL)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to get Scene tag", "Scene::Load");

		LoadXML(scene);

		xmlDoc->Clear();
		//Delete our allocated document
		delete xmlDoc;
		rs->LoadResourceGroup("GASSScene");
		Load();
	}

	void Scene::Save(const std::string &scene_path)
	{
		m_ScenePath = scene_path;
		TiXmlDocument doc;  
		TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );  
		doc.LinkEndChild( decl ); 

		TiXmlElement * scene_elem = new TiXmlElement("Scene");  
		doc.LinkEndChild( scene_elem); 

		BaseReflectionObject::SaveProperties(scene_elem);

		TiXmlElement * sms_elem = new TiXmlElement("SceneManagerSettings");  
		scene_elem->LinkEndChild(sms_elem);

		SaveXML(sms_elem);

		std::string filename = scene_path + "/Scene.xml";
		doc.SaveFile(filename.c_str());
		//Save scene specific object templates, filename should probably be a scene parameter
		//SimEngine::Get().GetSimObjectManager()->Load(scene_path + "/templates.xml");
	}


	void Scene::OnUpdate(double delta_time)
	{
		if(m_SceneLoaded)
		{
			m_SceneMessageManager->Update(delta_time);
			m_ObjectManager->SyncMessages(delta_time);
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
		FilePath path (std::string(parent->GetDocument()->Value()));

		m_ObjectManager->SaveXML(path.GetPathNoFile() + "/instances.xml");
	}

	void Scene::Unload()
	{
		if(m_SceneLoaded)
		{
			m_ObjectManager->Clear();
			MessagePtr scene_msg(new UnloadSceneManagersMessage(shared_from_this()));
			m_SceneMessageManager->SendImmediate(scene_msg);
			MessagePtr unload_msg(new SceneUnloadNotifyMessage(shared_from_this()));
			SimEngine::Get().GetSimSystemManager()->SendImmediate(unload_msg);
			m_SceneMessageManager->Clear();
			ResourceSystemPtr rs = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<IResourceSystem>();
			if(rs == NULL)
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"No Resource Manager Found", "Scene::SaveXML");
			rs->RemoveResourceGroup("GASSScene");
			m_SceneLoaded = false;
		}
	}

	void Scene::Load()
	{
		MessagePtr enter_load_msg(new SceneAboutToLoadNotifyMessage(shared_from_this()));
		SimEngine::Get().GetSimSystemManager()->SendImmediate(enter_load_msg);

		MessagePtr scene_msg(new LoadSceneManagersMessage(shared_from_this()));
		//send load message
		SendImmediate(scene_msg);

		//Create scene object instances from templates
		if(m_ScenePath != "")
			m_ObjectManager->LoadXML(m_ScenePath + "/instances.xml");

		MessagePtr system_msg(new SceneLoadedNotifyMessage(shared_from_this()));
		SimEngine::Get().GetSimSystemManager()->SendImmediate(system_msg);

		m_SceneLoaded = true;
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


	SceneManagerPtr Scene::GetSceneManager(const std::string &name)
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
		SceneObjectPtr so = GetObjectManager()->LoadFromTemplate(obj_template,message->GetParent());
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

	void Scene::OnRemoveSceneObject(RemoveSceneObjectMessagePtr message)
	{
		SceneObjectPtr so = message->GetSceneObject();
		if(so)
			GetObjectManager()->DeleteObject(so);
	}

	SceneManagerIterator Scene::GetSceneManagers()
	{
		return SceneManagerIterator(m_SceneManagers.begin(),m_SceneManagers.end());
	}

	double Scene::GetOrigoOffsetEast() const
	{
		return m_OffsetEast;
	}

	double Scene::GetOrigoOffsetNorth() const
	{
		return m_OffsetNorth;
	}

	void Scene::SetOrigoOffsetEast(double value)
	{
		m_OffsetEast = value;
	}

	void Scene::SetOrigoOffsetNorth(double value) 
	{
		m_OffsetNorth = value;
	}

	void Scene::SetProjection(const std::string &proj)
	{
		m_Projection = proj;
	}

	std::string Scene::GetProjection() const
	{
		return m_Projection;
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
}