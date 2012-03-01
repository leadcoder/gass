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
#include "Sim/Scenario/Scenario.h"
#include "Sim/Scenario/Scene/SceneManagerFactory.h"
#include "Sim/Scenario/Scene/ISceneManager.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Scenario/Scene/Messages/CoreScenarioSceneMessages.h"
#include "Sim/Scenario/Scene/Messages/GraphicsScenarioSceneMessages.h"
#include "Sim/Scenario/Scene/Messages/GraphicsSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/PhysicsSceneObjectMessages.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Systems/Resource/IResourceSystem.h"
#include "Sim/Systems/SimSystemManager.h"

#include "Sim/SimEngine.h"

#include "Core/Utils/Log.h"
#include "Core/System/ISystem.h"
#include "Core/ComponentSystem/BaseComponentContainerTemplateManager.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Serialize/IXMLSerialize.h"
#include "Core/Utils/FilePath.h"
#include "tinyxml.h"

namespace GASS
{
	Scenario::Scenario() : m_StartPos(Vec3(0,0,0)),
		m_StartRot(Vec3(0,0,0)),
		m_ScenarioMessageManager(new MessageManager())
	{

	}

	Scenario::~Scenario()
	{
		if(m_ObjectManager)
		{
			m_ObjectManager->Clear();
			MessagePtr scenario_msg(new UnloadSceneManagersMessage(shared_from_this()));
			m_ScenarioMessageManager->SendImmediate(scenario_msg);
			MessagePtr unload_msg(new ScenarioUnloadNotifyMessage(shared_from_this()));
			SimEngine::Get().GetSimSystemManager()->SendImmediate(unload_msg);
			m_ScenarioMessageManager->Clear();

			ResourceSystemPtr rs = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<IResourceSystem>();
			if(rs == NULL)
				Log::Error("No Resource Manager Found");
			rs->RemoveResourceGroup("GASSScenario");
		}
	}

	void Scenario::RegisterReflection()
	{
		RegisterVectorProperty<std::string>("ScenarioResourceFolders", &GASS::Scenario::GetScenarioResourceFolders, &GASS::Scenario::SetScenarioResourceFolders);
		RegisterProperty<Vec3>("StartPosition", &Scenario::GetStartPos, &Scenario::SetStartPos);
		RegisterProperty<Vec3>("StartRotation", &Scenario::GetStartRot, &Scenario::SetStartRot);
		//RegisterProperty<double>("OrigoOffsetEast", &Scenario::GetOrigoOffsetEast, &Scenario::SetOrigoOffsetEast);
		//RegisterProperty<double>("OrigoOffsetNorth", &Scenario::GetOrigoOffsetNorth, &Scenario::SetOrigoOffsetNorth);
		//RegisterProperty<std::string>("Projection", &Scenario::GetProjection, &Scenario::SetProjection);
	}

	std::vector<std::string> Scenario::GetScenarioResourceFolders() const
	{
		return m_ResourceFolders;
	}

	void Scenario::SetScenarioResourceFolders(const std::vector<std::string> &folders)
	{
		m_ResourceFolders = folders;
	}

	bool Scenario::Load(const std::string &scenario_path)
	{
		m_ScenarioPath = scenario_path;
		//MessagePtr system_msg(new ScenarioAboutToLoadNotifyMessage(shared_from_this()));
		//SimEngine::Get().GetSimSystemManager()->SendImmediate(system_msg);

		ResourceSystemPtr rs = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<IResourceSystem>();
		if(rs == NULL)
			Log::Error("No Resource Manager Found");
		rs->AddResourceLocation(scenario_path,"GASSScenario","FileSystem",false);

		std::string filename = scenario_path + "/scenario.xml";
		//Load scenario specific templates, filename should probably be a scenario parameter
		
		SimEngine::Get().GetSimObjectManager()->Load(scenario_path + "/templates.xml");

		if(filename =="") return false;
		TiXmlDocument *xmlDoc = new TiXmlDocument(filename.c_str());
		if(!xmlDoc->LoadFile())
		{
			//Fatal error, cannot load
			Log::Warning("SystemManager::Load() - Couldn't load: %s", filename.c_str());
			return false;
		}
		TiXmlElement *scenario = xmlDoc->FirstChildElement("Scenario");
		if(scenario == NULL) 
			Log::Error("Failed to get Scenario tag");

		TiXmlElement *settings = scenario->FirstChildElement("ScenarioSettings");
		if(settings == NULL) 
			Log::Error("Failed to get ScenarioSettings tag");

		BaseReflectionObject::LoadProperties(settings);

		for(std::vector<std::string>::iterator iter = m_ResourceFolders.begin(); iter != m_ResourceFolders.end(); ++iter)
		{
			std::string location = scenario_path + "/" + *iter;
			rs->AddResourceLocation(location,"GASSScenario","FileSystem",false);
		}
		rs->LoadResourceGroup("GASSScenario");


		TiXmlElement *scene_elem = scenario->FirstChildElement("Scenes");

		OnCreate();

		if(scene_elem)
		{
			scene_elem = scene_elem->FirstChildElement();
			//Loop through each template
			while(scene_elem)
			{

				
				LoadXML(scene_elem);			
				scene_elem= scene_elem->NextSiblingElement();
			}
		}



		xmlDoc->Clear();
		//Delete our allocated document
		delete xmlDoc;

		Init();

		return true;
	}

	bool Scenario::Init()
	{
		//load all scenes
		OnLoad();
		return true;
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

		
		SaveXML(scenes_elem);
		

		std::string filename = scenario_path + "/scenario.xml";
		doc.SaveFile(filename.c_str());

		//Save scenario specific object templates, filename should probably be a scenario parameter
		//SimEngine::Get().GetSimObjectManager()->Load(scenario_path + "/templates.xml");
		return true;
	}


	void Scenario::OnUpdate(double delta_time)
	{
		m_ScenarioMessageManager->Update(delta_time);
		m_ObjectManager->SyncMessages(delta_time);
	}


	void Scenario::LoadXML(TiXmlElement *scene_elem)
	{
		TiXmlElement *settings = scene_elem->FirstChildElement("SceneSettings");
		if(settings == NULL)
			Log::Error("Failed to get SceneSettings tag");

		BaseReflectionObject::LoadProperties(settings);
		TiXmlElement *scenemanager = scene_elem->FirstChildElement("SceneManagers");

		if(scenemanager)
		{
			scenemanager = scenemanager->FirstChildElement();
			//Load scene manager settings
			while(scenemanager)
			{
				SceneManagerPtr sm = LoadSceneManager(scenemanager);
				scenemanager = scenemanager->NextSiblingElement();
			}
		}
	}
	
	
	void Scenario::SaveXML(TiXmlElement *parent)
	{
		//Create
		TiXmlElement *scenario = new TiXmlElement("Scenario");
		parent->LinkEndChild(scenario);

		
		BaseReflectionObject::SaveProperties(scenario);

		TiXmlElement *sms_elem = new TiXmlElement("SceneManagers");
		scenario->LinkEndChild(sms_elem);

		for(int i  = 0 ; i < m_SceneManagers.size();i++)
		{
			SceneManagerPtr sm = m_SceneManagers[i];
			XMLSerializePtr serialize = boost::shared_dynamic_cast<IXMLSerialize>(sm);
			if(serialize)
				serialize->SaveXML(sms_elem);
		}
		//std::string scenario_path = GetPath();
		//save instances at same place
		FilePath path (std::string(parent->GetDocument()->Value()));
		
		
		m_ObjectManager->SaveXML(path.GetPathNoFile() + "/instances.xml");
	}

	void Scenario::OnCreate()
	{
		m_ScenarioMessageManager->RegisterForMessage(typeid(RemoveSceneObjectMessage), TYPED_MESSAGE_FUNC(Scenario::OnRemoveSceneObject,RemoveSceneObjectMessage),0);
		m_ScenarioMessageManager->RegisterForMessage(typeid(SpawnObjectFromTemplateMessage),TYPED_MESSAGE_FUNC(Scenario::OnSpawnSceneObjectFromTemplate,SpawnObjectFromTemplateMessage),0);

		m_ObjectManager = SceneObjectManagerPtr(new SceneObjectManager(shared_from_this()));
		m_ObjectManager->Init();

		//Add all registered scene manangers to scene
		std::vector<std::string> managers = SceneManagerFactory::GetPtr()->GetFactoryNames();
		for(int i = 0; i < managers.size();i++)
		{
			SceneManagerPtr sm = SceneManagerFactory::GetPtr()->Create(managers[i]);
			sm->SetScenario(shared_from_this());
			sm->SetName(managers[i]);
			sm->OnCreate();
			m_SceneManagers.push_back(sm);
		}
	}

	void Scenario::OnLoad()
	{
		MessagePtr enter_load_msg(new ScenarioAboutToLoadNotifyMessage(shared_from_this()));
		SimEngine::Get().GetSimSystemManager()->SendImmediate(enter_load_msg);

		MessagePtr scenario_msg(new LoadSceneManagersMessage(shared_from_this()));
		//send load message
		SendImmediate(scenario_msg);

		//Create scene object instances from templates
		m_ObjectManager->LoadXML(m_ScenarioPath + "/instances.xml");

		MessagePtr system_msg(new ScenarioLoadedNotifyMessage(shared_from_this()));
		SimEngine::Get().GetSimSystemManager()->SendImmediate(system_msg);
	}

	

	SceneManagerPtr Scenario::LoadSceneManager(TiXmlElement *sm_elem)
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


	SceneManagerPtr Scenario::GetSceneManager(const std::string &name)
	{
		for(int i = 0; i < m_SceneManagers.size(); i++)
		{
			if(m_SceneManagers[i]->GetName() == name)
				return m_SceneManagers[i];
		}
		SceneManagerPtr empty;
		return empty;
	}

	void Scenario::OnSpawnSceneObjectFromTemplate(SpawnObjectFromTemplateMessagePtr message)
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

	void Scenario::OnRemoveSceneObject(RemoveSceneObjectMessagePtr message)
	{
		SceneObjectPtr so = message->GetSceneObject();
		if(so)
			GetObjectManager()->DeleteObject(so);
	}

	SceneManagerIterator Scenario::GetSceneManagers()
	{
		return SceneManagerIterator(m_SceneManagers.begin(),m_SceneManagers.end());
	}

	/*double Scenario::GetOrigoOffsetEast() const
	{
		return m_OffsetEast;
	}

	double Scenario::GetOrigoOffsetNorth() const
	{
		return m_OffsetNorth;
	}

	void Scenario::SetOrigoOffsetEast(double value)
	{
		m_OffsetEast = value;
	}

	void Scenario::SetOrigoOffsetNorth(double value) 
	{
		m_OffsetNorth = value;
	}

	void Scenario::SetProjection(const std::string &proj)
	{
		m_Projection = proj;
	}

	std::string Scenario::GetProjection() const
	{
		return m_Projection;
	}*/

	int Scenario::RegisterForMessage(const MessageType &type, MessageFuncPtr callback, int priority )
	{
		return m_ScenarioMessageManager->RegisterForMessage(type, callback, priority);
	}

	void Scenario::UnregisterForMessage(const MessageType &type, MessageFuncPtr callback)
	{
		m_ScenarioMessageManager->UnregisterForMessage(type, callback);
	}

	void Scenario::PostMessage( MessagePtr message )
	{
		m_ScenarioMessageManager->PostMessage(message);
	}

	void Scenario::SendImmediate( MessagePtr message )
	{
		m_ScenarioMessageManager->SendImmediate(message);
	}
}
