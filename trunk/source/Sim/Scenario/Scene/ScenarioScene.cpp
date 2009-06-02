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
#include "Sim/Scenario/Scene/SceneManagerFactory.h"
#include "Sim/Scenario/Scene/ISceneManager.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/SimEngine.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/Message.h"
#include "Core/Utils/Log.h"
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
		//m_SceneMessageManager->AddMessageToSystem(SCENARIO_MESSAGE_LOAD_SCENE_MANAGERS);
		//m_SceneMessageManager->AddMessageToSystem(SCENARIO_MESSAGE_LOAD_SCENE_OBJECT);
		//m_SceneMessageManager->AddMessageToSystem(SCENARIO_MESSAGE_UPDATE);
		//m_SceneMessageManager->AddMessageToSystem(SM_MESSAGE_LOAD_GFX_COMPONENTS);
		//m_SceneMessageManager->AddMessageToSystem(SM_MESSAGE_LOAD_PHYSICS_COMPONENTS);
		//m_SceneMessageManager->AddMessageToSystem(SM_MESSAGE_LOAD_GAME_COMPONENTS);
	}



	ScenarioScene::~ScenarioScene()
	{
		m_ObjectManager->Clear();
		int from_id = (int)this;
		MessagePtr scenario_msg(new Message(SCENARIO_MESSAGE_UNLOAD_SCENE_MANAGERS,from_id));
		scenario_msg->SetData("ScenarioScene",this);
		m_SceneMessageManager->SendImmediate(scenario_msg);
		delete m_SceneMessageManager;
	}

	int ScenarioScene::RegisterForMessage( ScenarioMessage type, MessageFunc callback, int priority )
	{
		return m_SceneMessageManager->RegisterForMessage((int)type, callback, priority); 
	}

	void ScenarioScene::UnregisterForMessage(ScenarioMessage type, MessageFunc callback)
	{
		m_SceneMessageManager->UnregisterForMessage((int)type, callback);
	}

	void ScenarioScene::SendGlobalMessage( MessagePtr message )
	{
		m_SceneMessageManager->SendGlobalMessage(message);
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
		if(settings == NULL) Log::Error("Failed to get SceneSettings tag");
		BaseReflectionObject::LoadProperties(settings);

		TiXmlElement *scenemanager = scene_elem->FirstChildElement("SceneManagers");
		if(scenemanager)
		{
			scenemanager = scenemanager->FirstChildElement();
			//Loop through each template
			while(scenemanager)
			{
				SceneManagerPtr sm = LoadSceneManager(scenemanager);
				if(sm)
				{
					sm->SetOwner(this);
					sm->OnCreate();
					m_SceneManagers.push_back(sm);
				}
				scenemanager = scenemanager->NextSiblingElement();
			}
		}
	}

	void ScenarioScene::OnCreate()
	{


	}

	void ScenarioScene::OnLoad()
	{
		std::string scenario_path = m_Scenario->GetPath();

		int from_id = (int)this;
		MessagePtr enter_load_msg(new Message(SimSystemManager::SYSTEM_MESSAGE_SCENARIO_SCENE_ABOUT_TO_LOAD,from_id));
		enter_load_msg->SetData("ScenarioScene",this);
		SimEngine::Get().GetSystemManager()->SendImmediate(enter_load_msg);

		
		MessagePtr scenario_msg(new Message(SCENARIO_MESSAGE_LOAD_SCENE_MANAGERS,from_id));
		scenario_msg->SetData("ScenarioScene",this);
		//send load message
		SendImmediate(scenario_msg);
		
	
		// Load default camera ect
		/*SceneObject* scene_object = GetObjectManager()->LoadFromTemplate("FreeCameraObject");
		assert(scene_object);
		MessagePtr camera_msg(new Message(ScenarioScene::SCENARIO_MESSAGE_CHANGE_CAMERA,(int) this));
		camera_msg->SetData("CameraObject",scene_object);
		GetMessageManager()->SendImmediate(camera_msg);

		//move camera to spawn position
		MessagePtr pos_msg(new Message(SceneObject::OBJECT_MESSAGE_POSITION,(int) this));
		pos_msg->SetData("Position",GetStartPos());
		scene_object->SendImmediate(pos_msg);*/

		//Create game objects instances from templates
		m_ObjectManager->LoadFromFile(scenario_path + "/instances.xml");

		MessagePtr system_msg(new Message(SimSystemManager::SYSTEM_MESSAGE_SCENARIO_SCENE_LOADED,from_id));
		system_msg->SetData("ScenarioScene",this);
		SimEngine::Get().GetSystemManager()->SendImmediate(system_msg);
	}

	void ScenarioScene::OnUpdate(double delta_time)
	{
		int from_id = (int)this;
		boost::shared_ptr<ScenarioUpdateMessage> update_msg(new ScenarioUpdateMessage(SCENARIO_MESSAGE_UPDATE,from_id,delta_time));
		m_SceneMessageManager->SendImmediate(update_msg);
		m_SceneMessageManager->Update(delta_time);
		m_ObjectManager->SyncMessages(delta_time);
	}

	SceneManagerPtr ScenarioScene::LoadSceneManager(TiXmlElement *sm_elem)
	{
		std::string sm_name = sm_elem->Value();
		std::string sm_type = sm_elem->Attribute("type");
		SceneManagerPtr sm = SceneManagerFactory::Get().Create(sm_type);
		if(sm)
		{
			sm->SetName(sm_name);
			XMLSerializePtr serialize = boost::shared_dynamic_cast<IXMLSerialize>(sm);
			if(serialize)
				serialize->LoadXML(sm_elem);
		}
		return sm;
	}

	SceneManagerPtr ScenarioScene::GetSceneManager(const std::string &name)
	{
		for(int i = 0; m_SceneManagers.size(); i++)
		{
			if(m_SceneManagers[i]->GetName() == name)
				return m_SceneManagers[i];
		}
		SceneManagerPtr empty;
		return empty;
	}
}
