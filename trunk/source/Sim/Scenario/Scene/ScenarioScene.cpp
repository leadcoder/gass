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
	ScenarioScene::ScenarioScene():
		m_StartPos(Vec3(0,0,0)),
		m_StartRot(Vec3(0,0,0)),
		m_RT90Origo(Vec2(0,0)),
		m_Up(0,1,0),
		m_North(0,0,-1),
		m_East(1,0,0)
	{
		m_Scenario = NULL;
		m_SceneMessageManager = new MessageManager();
		m_ObjectManager = SceneObjectManagerPtr(new SceneObjectManager(this));
	}



	ScenarioScene::~ScenarioScene()
	{
		m_ObjectManager->Clear();
		MessagePtr scenario_msg(new UnloadSceneManagersMessage(this));
		m_SceneMessageManager->SendImmediate(scenario_msg);
		delete m_SceneMessageManager;
	}

	int ScenarioScene::RegisterForMessage( ScenarioMessage type, MessageFuncPtr callback, int priority )
	{
		return m_SceneMessageManager->RegisterForMessage((int)type, callback, priority); 
	}

	void ScenarioScene::UnregisterForMessage(ScenarioMessage type, MessageFuncPtr callback)
	{
		m_SceneMessageManager->UnregisterForMessage((int)type, callback);
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
			//Try to create all registered scene manangers

			std::vector<std::string> managers = SceneManagerFactory::GetPtr()->GetFactoryNames();

			for(int i = 0; i < managers.size();i++)
			{
				SceneManagerPtr sm = SceneManagerFactory::GetPtr()->Create(managers[i]);
				m_SceneManagers.push_back(sm);
				sm->SetOwner(this);
				sm->SetName(managers[i]);
			}
			scenemanager = scenemanager->FirstChildElement();
			//Load scene manager settings
			while(scenemanager)
			{
				SceneManagerPtr sm = LoadSceneManager(scenemanager);
				/*if(sm)
				{
					m_SceneManagers.push_back(sm);
				}*/
				scenemanager = scenemanager->NextSiblingElement();
			}
			for(int i = 0; i < m_SceneManagers.size();i++)
			{
				m_SceneManagers[i]->OnCreate();
			}
		}
	}

	void ScenarioScene::OnCreate()
	{
		RegisterForMessage(SCENARIO_MESSAGE_CLASS(ScenarioScene::OnRemoveSceneObject,RemoveSceneObjectMessage,0));
		RegisterForMessage(SCENARIO_MESSAGE_CLASS(ScenarioScene::OnSpawnSceneObjectFromTemplate,SpawnObjectFromTemplateMessage,0));
	}

	void ScenarioScene::OnLoad()
	{
		std::string scenario_path = m_Scenario->GetPath();

		MessagePtr enter_load_msg(new ScenarioSceneAboutToLoadNotifyMessage(this));
		SimEngine::Get().GetSystemManager()->SendImmediate(enter_load_msg);

		
		MessagePtr scenario_msg(new LoadSceneManagersMessage(this));
		//send load message
		SendImmediate(scenario_msg);
		
	
		// Load default camera ect
		/*SceneObject* scene_object = GetObjectManager()->LoadFromTemplate("FreeCameraObject");
		assert(scene_object);
		MessagePtr camera_msg(new Message(ScenarioScene::SCENARIO_RM_CHANGE_CAMERA,(int) this));
		camera_msg->SetData("CameraObject",scene_object);
		GetMessageManager()->SendImmediate(camera_msg);

		//move camera to spawn position
		MessagePtr pos_msg(new Message(SceneObject::OBJECT_RM_POSITION,(int) this));
		pos_msg->SetData("Position",GetStartPos());
		scene_object->SendImmediate(pos_msg);*/

		//Create game objects instances from templates
		m_ObjectManager->LoadFromFile(scenario_path + "/instances.xml");

		MessagePtr system_msg(new ScenarioSceneLoadedNotifyMessage(this));
		SimEngine::Get().GetSystemManager()->SendImmediate(system_msg);
	}

	void ScenarioScene::OnUpdate(double delta_time)
	{
		//boost::shared_ptr<ScenarioUpdateMessage> update_msg(new ScenarioUpdateMessage(SCENARIO_RM_UPDATE,delta_time));
		//m_SceneMessageManager->SendImmediate(update_msg);
		m_SceneMessageManager->Update(delta_time);
		m_ObjectManager->SyncMessages(delta_time);
	}

	SceneManagerPtr ScenarioScene::LoadSceneManager(TiXmlElement *sm_elem)
	{
		SceneManagerPtr sm;
		std::string sm_name = sm_elem->Value();
		std::string sm_type = sm_elem->Attribute("type");
		for(int i = 0; i < m_SceneManagers.size() ; i++)
		{
			if(m_SceneManagers[i]->GetName() == sm_type)
			{
				sm =m_SceneManagers[i];
				XMLSerializePtr serialize = boost::shared_dynamic_cast<IXMLSerialize>(sm);
				if(serialize)
					serialize->LoadXML(sm_elem);
				return sm;

			}
		}
		return sm;
		/*SceneManagerPtr sm = GetSceneManager(sm_type);
		if(sm)
		{
			sm->SetName(sm_name);
			XMLSerializePtr serialize = boost::shared_dynamic_cast<IXMLSerialize>(sm);
			if(serialize)
				serialize->LoadXML(sm_elem);
		}*/
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
		SceneObjectPtr so = GetObjectManager()->LoadFromTemplate(obj_template);
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
}
