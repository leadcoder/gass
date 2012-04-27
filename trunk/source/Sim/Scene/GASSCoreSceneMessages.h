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

#ifndef SCENE_MESSAGES_H
#define SCENE_MESSAGES_H

#include "Sim/GASSCommon.h"
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/MessageSystem/GASSBaseMessage.h"

namespace GASS
{
	class SceneObject;
	class Scene;
	typedef boost::shared_ptr<Scene> ScenePtr;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;


	//*********************************************************
	// ALL MESSAGES IN THIS SECTION CAN BE POSTED BY USER
	//*********************************************************
	
	/**
	Message used to remove a scene object from the scene
	*/
	class RemoveSceneObjectMessage : public BaseMessage
	{
	public:
		/**
		Constructor
		@param object The object to remove
		*/
		RemoveSceneObjectMessage(SceneObjectPtr object , SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Object(object)
		  {

		  }
		  SceneObjectPtr GetSceneObject() const {return m_Object;}
	private:
		SceneObjectPtr m_Object;
	};
	typedef boost::shared_ptr<RemoveSceneObjectMessage> RemoveSceneObjectMessagePtr;


	/**
	Message used to spawn a scene object from from template in the scene
	*/
	class SpawnObjectFromTemplateMessage : public BaseMessage
	{
	public:
		/**
		Constructor
		@param template_name Name of the template to use for  the new object
		@param position Position of the new object
		@param rotation Rotation of the new object
		@param veclocity Veclocity of the new object
		@param parent Optional parent object, otherwise this object is attached to the scene root
		*/
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

	
	//*********************************************************
	// ALL MESSAGES BELOW SHOULD ONLY BE POSTED GASS INTERNALS
	//*********************************************************


	/**
	Message sent by the scene class while loading a 
	new scene.
	It's up to each scene manager to catch this and load it's 
	dependencies
	*/
	class LoadSceneManagersMessage : public BaseMessage
	{
	public:
		LoadSceneManagersMessage(ScenePtr scene, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Scene(scene)
		  {

		  }
		  ScenePtr GetScene() const {return m_Scene;}
	private:
		ScenePtr m_Scene;
	};
	typedef boost::shared_ptr<LoadSceneManagersMessage> LoadSceneManagersMessagePtr;


	/**
	Message sent by the scene class 
	while unloading a scene.
	It's up to each scene manager to catch this free 
	it's resources.
	
	*/
	class UnloadSceneManagersMessage : public BaseMessage
	{
	public:
		UnloadSceneManagersMessage(ScenePtr scene, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Scene(scene)
		  {

		  }
		  ScenePtr GetScene() const {return m_Scene;}
	private:
		ScenePtr m_Scene;
	};
	typedef boost::shared_ptr<UnloadSceneManagersMessage> UnloadSceneManagersMessagePtr;


	/**
		Message sent by SceneObjectManager after scene object is created and added to the scene 
	*/
	class SceneObjectCreatedNotifyMessage : public BaseMessage
	{
	public:
		/**
		Constructor
		@param object Pointer to the new object
		*/
		SceneObjectCreatedNotifyMessage(SceneObjectPtr object , SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Object(object)
		  {

		  }
		  SceneObjectPtr GetSceneObject() const {return m_Object;}
	private:
		SceneObjectPtr m_Object;
	};
	typedef boost::shared_ptr<SceneObjectCreatedNotifyMessage> SceneObjectCreatedNotifyMessagePtr;


	/**
		Message sent by SceneObjectManager after scene object is removed from the scene 
	*/
	class SceneObjectRemovedNotifyMessage : public BaseMessage
	{
	public:
		/**
		Constructor
		@param object Pointer to the object that has been removed
		*/
		SceneObjectRemovedNotifyMessage(SceneObjectPtr object , SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Object(object)
		  {

		  }
		  SceneObjectPtr GetSceneObject() const {return m_Object;}
	private:
		SceneObjectPtr m_Object;
	};
	typedef boost::shared_ptr<SceneObjectRemovedNotifyMessage> SceneObjectRemovedNotifyMessagePtr;
	
}

#endif