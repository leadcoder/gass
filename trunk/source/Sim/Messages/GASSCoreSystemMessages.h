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

#ifndef CORE_SYSTEM_MESSAGES_H
#define CORE_SYSTEM_MESSAGES_H

#include "Sim/GASSCommon.h"
#include "Core/MessageSystem/GASSBaseMessage.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSQuaternion.h"

namespace GASS
{
	class Scene;
	class SceneObject;
	class ResourceLocation;
	class ResourceGroup;
	
	typedef boost::shared_ptr<Scene> ScenePtr;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::shared_ptr<ResourceLocation> ResourceLocationPtr;
	typedef boost::shared_ptr<ResourceGroup> ResourceGroupPtr;
	
	
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
	typedef boost::shared_ptr<SystemMessage> SystemMessagePtr;
	
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
	typedef boost::shared_ptr<SystemRequestMessage> SystemRequestMessagePtr;

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
	typedef boost::shared_ptr<DebugPrintRequest> DebugPrintRequestPtr;

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
	typedef boost::shared_ptr<TimeStepRequest> TimeStepRequestPtr;


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
	typedef boost::shared_ptr<SystemEventMessage> SystemEventMessagePtr;


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

	typedef boost::shared_ptr<PreSceneCreateEvent> PreSceneCreateEventPtr;

	/**
		This message is posted by the Scene class after a scene is loaded.
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
	typedef boost::shared_ptr<PostSceneCreateEvent> PostSceneCreateEventPtr;


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
	typedef boost::shared_ptr<SceneUnloadedEvent> SceneUnloadedEventPtr;

	class TimeStepDoneEvent : public SystemEventMessage
	{
	public:
		TimeStepDoneEvent(SenderID sender_id = -1, double delay= 0) : SystemEventMessage(sender_id , delay)
		{
		}
	private:
	};
	typedef boost::shared_ptr<TimeStepDoneEvent > TimeStepDoneEventPtr;

	class InputSystemLoadedEvent : public SystemEventMessage
	{
	public:
		InputSystemLoadedEvent(SenderID sender_id = -1, double delay= 0) : SystemEventMessage(sender_id , delay)
		{
		}
	private:
	};
	typedef boost::shared_ptr<InputSystemLoadedEvent> InputSystemLoadedEventPtr;


	class GraphicsSystemLoadedEvent : public SystemEventMessage
	{
	public:
		GraphicsSystemLoadedEvent(SenderID sender_id = -1, double delay= 0) : SystemEventMessage(sender_id , delay)
		{
		}
	private:
	};
	typedef boost::shared_ptr<GraphicsSystemLoadedEvent> GraphicsSystemLoadedEventPtr;
	
	class PhysicsSystemLoadedEvent : public SystemEventMessage
	{
	public:
		PhysicsSystemLoadedEvent(SenderID sender_id = -1, double delay= 0) : SystemEventMessage(sender_id , delay)
		{
		}
	private:
	};
	typedef boost::shared_ptr<PhysicsSystemLoadedEvent> PhysicsSystemLoadedEventPtr;

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
	typedef boost::shared_ptr<ResourceGroupCreatedEvent> ResourceGroupCreatedEventPtr;

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
	typedef boost::shared_ptr<ResourceGroupRemovedEvent> ResourceGroupRemovedEventPtr;

	class ResourceLocationCreatedEvent : public SystemEventMessage
	{
	public:
		ResourceLocationCreatedEvent(ResourceLocationPtr location, SenderID sender_id = -1, double delay= 0) : SystemEventMessage(sender_id , delay), m_Location(location)
		{
		}
		ResourceLocationPtr GetLocation() const{return m_Location;}
	private:
		ResourceLocationPtr m_Location;
	};
	typedef boost::shared_ptr<ResourceLocationCreatedEvent> ResourceLocationCreatedEventPtr;

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
	typedef boost::shared_ptr<ResourceLocationRemovedEvent> ResourceLocationRemovedEventPtr;

	



}
#endif
