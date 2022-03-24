/****************************************************************************
* This file is part of GASS.                                                *
* See https://github.com/leadcoder/gass                                     *
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
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

#ifndef CORE_SYSTEM_MESSAGES_H
#define CORE_SYSTEM_MESSAGES_H

#include "Sim/GASSCommon.h"
#include "Core/MessageSystem/GASSBaseMessage.h"
#include "Core/MessageSystem/GASSIMessage.h"

namespace GASS
{
	class Scene;
	class SceneObject;
	class ResourceLocation;
	class ResourceGroup;
	
	using ScenePtr = std::shared_ptr<Scene>;
	using SceneObjectPtr = std::shared_ptr<SceneObject>;
	using ResourceLocationPtr = std::shared_ptr<ResourceLocation>;
	using ResourceGroupPtr = std::shared_ptr<ResourceGroup>;
	
	
	/**
	Base class for all system messages
	*/
	class SystemMessage : public BaseMessage
	{
	protected:
		SystemMessage(SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay)
		  {
		  }
	};
	using SystemMessagePtr = std::shared_ptr<SystemMessage>;
	
	//////////////////////////
	//request message section
	////////////////////////

	class SystemRequestMessage : public SystemMessage
	{
	protected:
		SystemRequestMessage(SenderID sender_id = -1, double delay= 0) : 
		  SystemMessage(sender_id , delay)
		  {

		  }
	};
	using SystemRequestMessagePtr = std::shared_ptr<SystemRequestMessage>;

	

	//@deprecated	
	class TimeStepRequest : public SystemRequestMessage
	{
	public:
		TimeStepRequest  (const double &step, SenderID sender_id = -1, double delay= 0) : SystemRequestMessage(sender_id , delay), m_TimeStep(step)
		{

		}
		double GetTimeStep() const {return m_TimeStep;}
	private:
		double m_TimeStep;
	};
	using TimeStepRequestPtr = std::shared_ptr<TimeStepRequest>;


	enum SimEventType
	{
		SET_PAUSE,
		SET_RESUME,
		SET_START,
		SET_STOP
	};

	


	//////////////////////////
	//event message section
	////////////////////////
	class SystemEventMessage : public SystemMessage
	{
	protected:
		SystemEventMessage(SenderID sender_id = -1, double delay= 0) : 
		  SystemMessage(sender_id , delay)
		  {

		  }
	};
	using SystemEventMessagePtr = std::shared_ptr<SystemEventMessage>;


	/**
		This message is posted by SimEngine when simulation state is changed.
	*/

	class SimEvent : public SystemEventMessage
	{
	public:
		SimEvent  (SimEventType event_type, SenderID sender_id = -1, double delay= 0) : SystemEventMessage(sender_id , delay), m_EventType(event_type)
		{

		}
		SimEventType GetEventType()const {return m_EventType;}
	private:
		SimEventType m_EventType;
	};
	using SimEventPtr = std::shared_ptr<SimEvent>;


	/**
		This message is posted by the Scene class before the scenes are loaded.
	*/
	
	class PreSceneCreateEvent : public SystemEventMessage
	{
	public:
		PreSceneCreateEvent(ScenePtr scene, SenderID sender_id = -1, double delay= 0) :
		  SystemEventMessage(sender_id , delay) ,
			  m_Scene(scene){}

		  ScenePtr GetScene() const {return m_Scene;}
	private:
		ScenePtr m_Scene;
	};

	using PreSceneCreateEventPtr = std::shared_ptr<PreSceneCreateEvent>;

	/**
		This message is posted by the Scene class after a scene is created.
	*/
	class PostSceneCreateEvent : public SystemEventMessage
	{
	public:
		PostSceneCreateEvent(ScenePtr scene,SenderID sender_id = -1, double delay= 0) :
		  SystemEventMessage(sender_id , delay) ,
			  m_Scene(scene){}

		  ScenePtr GetScene() const {return m_Scene;}
	private:
		ScenePtr m_Scene;
	};
	using PostSceneCreateEventPtr = std::shared_ptr<PostSceneCreateEvent>;


	/**
		This message is posted by the Scene class after a scene is loaded from file.
		This event is always posted after PostSceneCreateEvent (you must have a scene 
		instance before you can call the load function on the scene...).
	*/
	class PostSceneLoadEvent : public SystemEventMessage
	{
	public:
		PostSceneLoadEvent(ScenePtr scene,SenderID sender_id = -1, double delay= 0) :
		  SystemEventMessage(sender_id , delay) ,
			  m_Scene(scene){}

		  ScenePtr GetScene() const {return m_Scene;}
	private:
		ScenePtr m_Scene;
	};
	using PostSceneLoadEventPtr = std::shared_ptr<PostSceneLoadEvent>;


	class SceneUnloadedEvent : public SystemEventMessage
	{
	public:
		SceneUnloadedEvent(ScenePtr scene, SenderID sender_id = -1, double delay= 0) :
		  SystemEventMessage(sender_id , delay) ,
			  m_Scene(scene){}

		  ScenePtr GetScene() const {return m_Scene;}
	private:
		ScenePtr m_Scene;
	};
	using SceneUnloadedEventPtr = std::shared_ptr<SceneUnloadedEvent>;

	class TimeStepDoneEvent : public SystemEventMessage
	{
	public:
		TimeStepDoneEvent(SenderID sender_id = -1, double delay= 0) : SystemEventMessage(sender_id , delay)
		{
		}
	};
	using TimeStepDoneEventPtr = std::shared_ptr<TimeStepDoneEvent>;

	class InputSystemLoadedEvent : public SystemEventMessage
	{
	public:
		InputSystemLoadedEvent(SenderID sender_id = -1, double delay= 0) : SystemEventMessage(sender_id , delay)
		{
		}
	};
	using InputSystemLoadedEventPtr = std::shared_ptr<InputSystemLoadedEvent>;


	class GraphicsSystemLoadedEvent : public SystemEventMessage
	{
	public:
		GraphicsSystemLoadedEvent(SenderID sender_id = -1, double delay= 0) : SystemEventMessage(sender_id , delay)
		{
		}
	};
	using GraphicsSystemLoadedEventPtr = std::shared_ptr<GraphicsSystemLoadedEvent>;
	
	class PhysicsSystemLoadedEvent : public SystemEventMessage
	{
	public:
		PhysicsSystemLoadedEvent(SenderID sender_id = -1, double delay= 0) : SystemEventMessage(sender_id , delay)
		{
		}
	};
	using PhysicsSystemLoadedEventPtr = std::shared_ptr<PhysicsSystemLoadedEvent>;

	class ResourceGroupCreatedEvent: public SystemEventMessage
	{
	public:
		ResourceGroupCreatedEvent(ResourceGroupPtr group, SenderID sender_id = -1, double delay= 0) : SystemEventMessage(sender_id , delay), m_Group(group)
		{
		}
		ResourceGroupPtr GetGroup() const{return m_Group;}
	private:
		ResourceGroupPtr m_Group;
	};
	using ResourceGroupCreatedEventPtr = std::shared_ptr<ResourceGroupCreatedEvent>;

	class ResourceGroupRemovedEvent: public SystemEventMessage
	{
	public:
		ResourceGroupRemovedEvent(ResourceGroupPtr group, SenderID sender_id = -1, double delay= 0) : SystemEventMessage(sender_id , delay), m_Group(group)
		{
		}
		ResourceGroupPtr GetGroup() const{return m_Group;}
	private:
		ResourceGroupPtr m_Group;
	};
	using ResourceGroupRemovedEventPtr = std::shared_ptr<ResourceGroupRemovedEvent>;

	class ResourceGroupReloadEvent : public SystemEventMessage
	{
	public:
		ResourceGroupReloadEvent(ResourceGroupPtr group, SenderID sender_id = -1, double delay = 0) : SystemEventMessage(sender_id, delay), m_Group(group)
		{
		}
		ResourceGroupPtr GetGroup() const { return m_Group; }
	private:
		ResourceGroupPtr m_Group;
	};
	using ResourceGroupReloadEventPtr = std::shared_ptr<ResourceGroupReloadEvent>;

	class ResourceLocationAddedEvent  : public SystemEventMessage
	{
	public:
		ResourceLocationAddedEvent(ResourceLocationPtr location, SenderID sender_id = -1, double delay= 0) : SystemEventMessage(sender_id , delay), m_Location(location)
		{
		}
		ResourceLocationPtr GetLocation() const{return m_Location;}
	private:
		ResourceLocationPtr m_Location;
	};
	using ResourceLocationAddedEventPtr = std::shared_ptr<ResourceLocationAddedEvent>;

	class ResourceLocationRemovedEvent : public SystemEventMessage
	{
	public:
		ResourceLocationRemovedEvent(ResourceLocationPtr location, SenderID sender_id = -1, double delay= 0) : SystemEventMessage(sender_id , delay), m_Location(location)
		{
		}
		ResourceLocationPtr GetLocation() const{return m_Location;}
	private:
		ResourceLocationPtr m_Location;
	};
	using ResourceLocationRemovedEventPtr = std::shared_ptr<ResourceLocationRemovedEvent>;

	class ScriptEvent : public SystemEventMessage
	{
	public:
		ScriptEvent(const std::string &message, SenderID sender_id = -1, double delay= 0) :
		  SystemEventMessage(sender_id , delay),
			  m_Message(message)  { }
		  std::string GetMessage()const {return m_Message;}
	private:
		std::string m_Message;
	};
	using ScriptEventPtr = std::shared_ptr<ScriptEvent>;


}
#endif
