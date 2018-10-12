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
	
	typedef GASS_SHARED_PTR<Scene> ScenePtr;
	typedef GASS_SHARED_PTR<SceneObject> SceneObjectPtr;
	typedef GASS_SHARED_PTR<ResourceLocation> ResourceLocationPtr;
	typedef GASS_SHARED_PTR<ResourceGroup> ResourceGroupPtr;
	
	
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
	typedef GASS_SHARED_PTR<SystemMessage> SystemMessagePtr;
	
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
	typedef GASS_SHARED_PTR<SystemRequestMessage> SystemRequestMessagePtr;

	/**
	Message that can be posted by anyone to request that a new debug messages should be visualized during one frame.
	*/



	class DebugPrintRequest : public SystemRequestMessage
	{
	public:
		DebugPrintRequest(const std::string &text, SenderID sender_id = -1, double delay= 0) :
		  SystemRequestMessage(sender_id , delay),
			  m_Text(text)  { }
		  std::string GetText()const {return m_Text;}

	private:
		std::string m_Text;

	};
	typedef GASS_SHARED_PTR<DebugPrintRequest> DebugPrintRequestPtr;

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
	typedef GASS_SHARED_PTR<TimeStepRequest> TimeStepRequestPtr;


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
	typedef GASS_SHARED_PTR<SystemEventMessage> SystemEventMessagePtr;


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
	typedef GASS_SHARED_PTR<SimEvent> SimEventPtr;


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

	typedef GASS_SHARED_PTR<PreSceneCreateEvent> PreSceneCreateEventPtr;

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
	typedef GASS_SHARED_PTR<PostSceneCreateEvent> PostSceneCreateEventPtr;


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
	typedef GASS_SHARED_PTR<PostSceneLoadEvent> PostSceneLoadEventPtr;


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
	typedef GASS_SHARED_PTR<SceneUnloadedEvent> SceneUnloadedEventPtr;

	class TimeStepDoneEvent : public SystemEventMessage
	{
	public:
		TimeStepDoneEvent(SenderID sender_id = -1, double delay= 0) : SystemEventMessage(sender_id , delay)
		{
		}
	};
	typedef GASS_SHARED_PTR<TimeStepDoneEvent > TimeStepDoneEventPtr;

	class InputSystemLoadedEvent : public SystemEventMessage
	{
	public:
		InputSystemLoadedEvent(SenderID sender_id = -1, double delay= 0) : SystemEventMessage(sender_id , delay)
		{
		}
	};
	typedef GASS_SHARED_PTR<InputSystemLoadedEvent> InputSystemLoadedEventPtr;


	class GraphicsSystemLoadedEvent : public SystemEventMessage
	{
	public:
		GraphicsSystemLoadedEvent(SenderID sender_id = -1, double delay= 0) : SystemEventMessage(sender_id , delay)
		{
		}
	};
	typedef GASS_SHARED_PTR<GraphicsSystemLoadedEvent> GraphicsSystemLoadedEventPtr;
	
	class PhysicsSystemLoadedEvent : public SystemEventMessage
	{
	public:
		PhysicsSystemLoadedEvent(SenderID sender_id = -1, double delay= 0) : SystemEventMessage(sender_id , delay)
		{
		}
	};
	typedef GASS_SHARED_PTR<PhysicsSystemLoadedEvent> PhysicsSystemLoadedEventPtr;

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
	typedef GASS_SHARED_PTR<ResourceGroupCreatedEvent> ResourceGroupCreatedEventPtr;

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
	typedef GASS_SHARED_PTR<ResourceGroupRemovedEvent> ResourceGroupRemovedEventPtr;

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
	typedef GASS_SHARED_PTR<ResourceGroupReloadEvent> ResourceGroupReloadEventPtr;

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
	typedef GASS_SHARED_PTR<ResourceLocationAddedEvent> ResourceLocationAddedEventPtr;

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
	typedef GASS_SHARED_PTR<ResourceLocationRemovedEvent> ResourceLocationRemovedEventPtr;

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
	typedef GASS_SHARED_PTR<ScriptEvent> ScriptEventPtr;


}
#endif
