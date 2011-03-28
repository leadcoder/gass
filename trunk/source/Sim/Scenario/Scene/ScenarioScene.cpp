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



#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/ScenarioSceneMessages.h"
#include "Sim/Scenario/Scene/SceneManagerFactory.h"
#include "Sim/Scenario/Scene/ISceneManager.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Systems/SimSystemMessages.h"
#include "Sim/SimEngine.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Utils/Log.h"
#include "Core/Math/Quaternion.h"
#include "Core/Serialize/IXMLSerialize.h"
#include "tinyxml.h"

namespace GASS
{
	ScenarioScene::ScenarioScene(ScenarioPtr scenario):
		m_StartPos(Vec3(0,0,0)),
		m_StartRot(Vec3(0,0,0)),
		m_RT90Origo(Vec2(0,0)),
		m_Up(0,1,0),
		m_North(0,0,-1),
		m_East(1,0,0),
		m_Scenario(scenario),
		m_SceneMessageManager(new MessageManager()),
		m_InstancesFile("instances.xml")
	{

	}

	ScenarioScene::~ScenarioScene()
	{
	}


	void ScenarioScene::Shutdown()
	{
		Log::Print("Scenario scene shutdown started for:%s",GetName().c_str());


		m_ObjectManager->Clear();

		MessagePtr scenario_msg(new UnloadSceneManagersMessage(shared_from_this()));
		m_SceneMessageManager->SendImmediate(scenario_msg);

		MessagePtr unload_msg(new ScenarioSceneUnloadNotifyMessage(shared_from_this()));
		SimEngine::Get().GetSimSystemManager()->SendImmediate(unload_msg);

		m_SceneMessageManager->Clear();
	}

	int ScenarioScene::RegisterForMessage(const MessageType &type, MessageFuncPtr callback, int priority )
	{
		return m_SceneMessageManager->RegisterForMessage(type, callback, priority);
	}

	void ScenarioScene::UnregisterForMessage(const MessageType &type, MessageFuncPtr callback)
	{
		m_SceneMessageManager->UnregisterForMessage(type, callback);
	}

	void ScenarioScene::PostMessage( MessagePtr message )
	{
		m_SceneMessageManager->PostMessage(message);
	}

	void ScenarioScene::SendImmediate( MessagePtr message )
	{
		m_SceneMessageManager->SendImmediate(message);
	}

	void ScenarioScene::RegisterReflection()
	{
		RegisterProperty<Vec3>("StartPosition", &GASS::ScenarioScene::GetStartPos, &GASS::ScenarioScene::SetStartPos);
		RegisterProperty<Vec3>("StartRotation", &GASS::ScenarioScene::GetStartRot, &GASS::ScenarioScene::SetStartRot);
		RegisterProperty<std::string>("SceneUp", &GASS::ScenarioScene::GetUpVector, &GASS::ScenarioScene::SetUpVector);
		RegisterProperty<std::string>("SceneEast", &GASS::ScenarioScene::GetEastVector, &GASS::ScenarioScene::SetEastVector);
		RegisterProperty<std::string>("SceneNorth", &GASS::ScenarioScene::GetNorthVector, &GASS::ScenarioScene::SetNorthVector);
		RegisterProperty<std::string>("Instances", &GASS::ScenarioScene::GetInstancesFile, &GASS::ScenarioScene::SetInstancesFile);
		RegisterProperty<Vec2>("SetRT90Origo", &GASS::ScenarioScene::GetRT90Origo, &GASS::ScenarioScene::SetRT90Origo);
		
	}


	void ScenarioScene::SetRT90Origo(const Vec2& origo) 
	{
		m_RT90Origo=origo;
	}
	

	void ScenarioScene::SetInstancesFile(const std::string &value)
	{
		m_InstancesFile = value;
	}

	std::string ScenarioScene::GetInstancesFile() const
	{
		return m_InstancesFile;
	}

	void ScenarioScene::SetUpVector(const std::string &value)
	{
		m_Up = GetVector(value);
	}

	std::string ScenarioScene::GetUpVector() const
	{
		return GetVector(m_Up);
	}

	void ScenarioScene::SetEastVector(const std::string &value)
	{
		m_East = GetVector(value);
	}

	std::string ScenarioScene::GetEastVector() const
	{
		return GetVector(m_East);
	}

	void ScenarioScene::SetNorthVector(const std::string &value)
	{
		m_North = GetVector(value);
	}

	std::string ScenarioScene::GetNorthVector() const
	{
		return GetVector(m_North);
	}

	Vec3 ScenarioScene::GetVector(const std::string &str) const
	{
		if(str == "x" || str == "X")
		{
			return Vec3(1,0,0);
		}
		else if(str == "-x" || str == "-X")
		{
			return Vec3(-1,0,0);
		}
		else if(str == "y" || str == "Y")
		{
			return Vec3(0,1,0);
		}
		else if(str == "-y" || str == "-Y")
		{
			return Vec3(0,-1,0);
		}
		else if(str == "z" || str == "Z")
		{
			return Vec3(0,0,1);
		}
		else if(str == "-z" || str == "-Z")
		{
			return Vec3(0,0,-1);
		}
		return Vec3(0,0,0);
	}
	std::string  ScenarioScene::GetVector(const Vec3 &vec) const
	{
		if(vec.x > 0)
			return std::string("x");
		else if(vec.x < 0)
			return std::string("-x");
		if(vec.y > 0)
			return std::string("y");
		else if(vec.y < 0)
			return std::string("-y");
		if(vec.z > 0)
			return std::string("z");
		else if(vec.z < 0)
			return std::string("-z");
		return std::string("undefined");
	}

	Vec3 ScenarioScene::RT90ToGASSPosition(const Vec3 &pos)
	{
		Vec3 new_pos;
		new_pos.y = pos.z;
		new_pos.x = pos.x - m_RT90Origo.x;
		new_pos.z = m_RT90Origo.y - pos.y;
		return new_pos;
	}

	Vec3 ScenarioScene::GASSToRT90Position(const Vec3 &pos)
	{
		Vec3 new_pos;
		new_pos.z = pos.y;
		new_pos.x = pos.x + m_RT90Origo.x;
		new_pos.y = m_RT90Origo.y - pos.z;
		return new_pos;
	}


	void ScenarioScene::LoadXML(TiXmlElement *scene_elem)
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

	void ScenarioScene::SaveXML(TiXmlElement *parent)
	{
		//Create
		TiXmlElement *scene_elem = new TiXmlElement("ScenarioScene");
		parent->LinkEndChild(scene_elem);


		TiXmlElement *settings_elem = new TiXmlElement("SceneSettings");
		scene_elem->LinkEndChild(settings_elem);

		BaseReflectionObject::SaveProperties(settings_elem);

		TiXmlElement *sms_elem = new TiXmlElement("SceneManagers");
		scene_elem->LinkEndChild(sms_elem);

		for(int i  = 0 ; i < m_SceneManagers.size();i++)
		{
			SceneManagerPtr sm = m_SceneManagers[i];
			XMLSerializePtr serialize = boost::shared_dynamic_cast<IXMLSerialize>(sm);
			if(serialize)
				serialize->SaveXML(sms_elem);
		}
		std::string scenario_path = ScenarioPtr(m_Scenario)->GetPath();
		m_ObjectManager->SaveXML(scenario_path + "/"  + m_InstancesFile);
	}

	void ScenarioScene::OnCreate()
	{
		RegisterForMessage(typeid(RemoveSceneObjectMessage), TYPED_MESSAGE_FUNC(ScenarioScene::OnRemoveSceneObject,RemoveSceneObjectMessage),0);
		RegisterForMessage(typeid(SpawnObjectFromTemplateMessage),TYPED_MESSAGE_FUNC(ScenarioScene::OnSpawnSceneObjectFromTemplate,SpawnObjectFromTemplateMessage),0);

		m_ObjectManager = SceneObjectManagerPtr(new SceneObjectManager(shared_from_this()));
		m_ObjectManager->Init();

		//Add all registered scene manangers to scene
		std::vector<std::string> managers = SceneManagerFactory::GetPtr()->GetFactoryNames();
		for(int i = 0; i < managers.size();i++)
		{
			SceneManagerPtr sm = SceneManagerFactory::GetPtr()->Create(managers[i]);
			sm->SetScenarioScene(shared_from_this());
			sm->SetName(managers[i]);
			sm->OnCreate();
			m_SceneManagers.push_back(sm);
		}
	}

	void ScenarioScene::OnLoad()
	{
		std::string scenario_path = ScenarioPtr(m_Scenario)->GetPath();

		MessagePtr enter_load_msg(new ScenarioSceneAboutToLoadNotifyMessage(shared_from_this()));
		SimEngine::Get().GetSimSystemManager()->SendImmediate(enter_load_msg);

		MessagePtr scenario_msg(new LoadSceneManagersMessage(shared_from_this()));
		//send load message
		SendImmediate(scenario_msg);

		//Create scene object instances from templates
		m_ObjectManager->LoadXML(scenario_path + "/"  + m_InstancesFile);

		MessagePtr system_msg(new ScenarioSceneLoadedNotifyMessage(shared_from_this()));
		SimEngine::Get().GetSimSystemManager()->SendImmediate(system_msg);
	}

	void ScenarioScene::OnUpdate(double delta_time)
	{
		m_SceneMessageManager->Update(delta_time);
		m_ObjectManager->SyncMessages(delta_time);
	}

	SceneManagerPtr ScenarioScene::LoadSceneManager(TiXmlElement *sm_elem)
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

	SceneManagerPtr ScenarioScene::GetSceneManager(const std::string &name)
	{
		for(int i = 0; i < m_SceneManagers.size(); i++)
		{
			if(m_SceneManagers[i]->GetName() == name)
				return m_SceneManagers[i];
		}
		SceneManagerPtr empty;
		return empty;
	}

	void ScenarioScene::OnSpawnSceneObjectFromTemplate(SpawnObjectFromTemplateMessagePtr message)
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

	void ScenarioScene::OnRemoveSceneObject(RemoveSceneObjectMessagePtr message)
	{
		SceneObjectPtr so = message->GetSceneObject();
		if(so)
			GetObjectManager()->DeleteObject(so);

	}

	SceneManagerIterator ScenarioScene::GetSceneManagers()
	{
		return SceneManagerIterator(m_SceneManagers.begin(),m_SceneManagers.end());
	}
}
