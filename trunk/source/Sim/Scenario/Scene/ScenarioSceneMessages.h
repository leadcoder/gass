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

#ifndef SCENARIO_SCENE_MESSAGES_H
#define SCENARIO_SCENE_MESSAGES_H

#include "Sim/Common.h"
#include "Core/Math/Vector.h"
#include "Core/Math/Quaternion.h"
#include "Core/MessageSystem/BaseMessage.h"

namespace GASS
{
	class SceneObject;
	class ScenarioScene;
	typedef boost::shared_ptr<ScenarioScene> ScenarioScenePtr;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;

	/**
	Request message sent when loading a scenario scene
	It's up to each scene manager to catch this and load its stuff
	*/
	class LoadSceneManagersMessage : public BaseMessage
	{
	public:
		LoadSceneManagersMessage(ScenarioScenePtr scenario_scene, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_SS(scenario_scene)
		  {

		  }
		  ScenarioScenePtr GetScenarioScene() const {return m_SS;}
	private:
		ScenarioScenePtr m_SS;
	};
	typedef boost::shared_ptr<LoadSceneManagersMessage> LoadSceneManagersMessagePtr;


	class UnloadSceneManagersMessage : public BaseMessage
	{
	public:
		UnloadSceneManagersMessage(ScenarioScenePtr scenario_scene, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_SS(scenario_scene)
		  {

		  }
		  ScenarioScenePtr GetScenarioScene() const {return m_SS;}
	private:
		ScenarioScenePtr m_SS;
	};
	typedef boost::shared_ptr<UnloadSceneManagersMessage> UnloadSceneManagersMessagePtr;


	class ChangeCameraMessage : public BaseMessage
	{
	public:
		ChangeCameraMessage(SceneObjectPtr camera , SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Camera(camera)
		  {

		  }
		  SceneObjectPtr GetCamera() const {return m_Camera;}
	private:
		SceneObjectPtr m_Camera;
	};
	typedef boost::shared_ptr<ChangeCameraMessage> ChangeCameraMessagePtr;

	class RemoveSceneObjectMessage : public BaseMessage
	{
	public:
		RemoveSceneObjectMessage(SceneObjectPtr object , SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Object(object)
		  {

		  }
		  SceneObjectPtr GetSceneObject() const {return m_Object;}
	private:
		SceneObjectPtr m_Object;
	};
	typedef boost::shared_ptr<RemoveSceneObjectMessage> RemoveSceneObjectMessagePtr;


	class SpawnObjectFromTemplateMessage : public BaseMessage
	{
	public:
		SpawnObjectFromTemplateMessage(const std::string &template_name, 
			const Vec3 &position,
			const Quaternion &rotation,
			const Vec3 &velocity,
			SceneObjectPtr parent = SceneObjectPtr(),
			SenderID sender_id = -1, 
			double delay= 0) : 
		BaseMessage(sender_id , delay), 
			m_Tempalate(template_name),
			m_Position(position),
			m_Rotation(rotation),
			m_Velocity(velocity),
			m_Parent(parent)
		{

		}
		std::string GetTemplateName() const {return m_Tempalate;}
		Vec3 GetPosition() const {return m_Position;}
		Quaternion GetRotation() const {return m_Rotation;}
		Vec3 GetVelocity() const {return m_Velocity;}
		SceneObjectPtr GetParent() const {return m_Parent;}
	private:
		std::string m_Tempalate;
		Vec3 m_Position;
		Quaternion m_Rotation;
		Vec3 m_Velocity;
		SceneObjectPtr m_Parent;
	};

	typedef boost::shared_ptr<SpawnObjectFromTemplateMessage> SpawnObjectFromTemplateMessagePtr;

	class SceneObjectCreatedNotifyMessage : public BaseMessage
	{
	public:
		SceneObjectCreatedNotifyMessage(SceneObjectPtr object , SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Object(object)
		  {

		  }
		  SceneObjectPtr GetSceneObject() const {return m_Object;}
	private:
		SceneObjectPtr m_Object;
	};
	typedef boost::shared_ptr<SceneObjectCreatedNotifyMessage> SceneObjectCreatedNotifyMessagePtr;

	class SceneObjectRemovedNotifyMessage : public BaseMessage
	{
	public:
		SceneObjectRemovedNotifyMessage(SceneObjectPtr object , SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Object(object)
		  {

		  }
		  SceneObjectPtr GetSceneObject() const {return m_Object;}
	private:
		SceneObjectPtr m_Object;
	};
	typedef boost::shared_ptr<SceneObjectRemovedNotifyMessage> SceneObjectRemovedNotifyMessagePtr;

	class CameraChangedNotifyMessage : public BaseMessage
	{
	public:
		CameraChangedNotifyMessage(SceneObjectPtr camera , void* user_data, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Camera(camera),m_UserData(user_data)
		  {

		  }
		  SceneObjectPtr GetCamera() const {return m_Camera;}
		  void* GetUserData() const {return m_UserData;}
	private:
		SceneObjectPtr m_Camera;
		void *m_UserData;
	};
	typedef boost::shared_ptr<CameraChangedNotifyMessage> CameraChangedNotifyMessagePtr;


		///////Physics messages/////////////////
	
	class ActivatePhysicsMessage : public BaseMessage
	{
	public:
		ActivatePhysicsMessage(int activate, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay) ,
			  m_Activate(activate){}
		  int Activate() const {return m_Activate;}
	private:
		int m_Activate;
	};
	typedef boost::shared_ptr<ActivatePhysicsMessage> ActivatePhysicsMessagePtr;
}

#endif