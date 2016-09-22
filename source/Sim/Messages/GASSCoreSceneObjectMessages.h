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

#pragma once

#include "Sim/GASSCommon.h"
#include "Core/MessageSystem/GASSBaseMessage.h"
#include "Core/MessageSystem/GASSIMessage.h"

/**
This file hold messages that should be posted to
SceneObjects. SceneObjectMessages are used to
enable communication between components hold by
a SceneObject. The user is free to extend this
set of messages but this file hold the "core"
messages that all components share.
If a scene object message is added and it's
of common interest it can be a candidate for
this file.
*/

namespace GASS
{
	class SimSceneManager;
	class ISceneManager;
	class IGeometryComponent;
	class CoreSceneManager;
	typedef GASS_SHARED_PTR<ISceneManager> SceneManagerPtr;
	typedef GASS_SHARED_PTR<IGeometryComponent> GeometryComponentPtr;
	typedef GASS_SHARED_PTR<CoreSceneManager> CoreSceneManagerPtr; 
	
	
	class SceneObjectMessage : public BaseMessage
	{
	protected:
		SceneObjectMessage(SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay)
		  {
		  }
	};
	typedef GASS_SHARED_PTR<SceneObjectMessage> SceneObjectMessagePtr;

	class SceneObjectEventMessage : public SceneObjectMessage
	{
	protected:
		SceneObjectEventMessage(SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectMessage(sender_id , delay)
		  {
		  }
	};
	typedef GASS_SHARED_PTR<SceneObjectEventMessage> SceneObjectEventMessagePtr;

	class SceneObjectRequestMessage : public SceneObjectMessage
	{
	protected:
		SceneObjectRequestMessage(SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectMessage(sender_id , delay)
		  {
		  }
	};
	typedef GASS_SHARED_PTR<SceneObjectRequestMessage> SceneObjectRequestMessagePtr;

	/**
	Change name of scene object
	*/

	class SceneObjectNameMessage : public SceneObjectRequestMessage
	{
	public:
		SceneObjectNameMessage(const std::string &name, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage(sender_id , delay), m_Name(name)
		  {

		  }
		  std::string GetName()const {return m_Name;}
	private:
		std::string m_Name;
	};
	typedef GASS_SHARED_PTR<SceneObjectNameMessage> SceneObjectNameMessagePtr;

	/**
		Message that can be used to show debug information about a object
		TODO: move this object to other place?
	*/
	class DebugComponentSettingsRequest : public SceneObjectRequestMessage
	{
	public:
		DebugComponentSettingsRequest(bool show_object_name, SenderID sender_id = -1, double delay= 0) :
		  SceneObjectRequestMessage( sender_id , delay),
			  m_ShowObjectName(show_object_name)
		  {
		  }
		  bool GetShowObjectName() const {return m_ShowObjectName;}
	private:
		bool m_ShowObjectName;
	};
	typedef GASS_SHARED_PTR<DebugComponentSettingsRequest> DebugComponentSettingsRequestPtr;


	/**
	This request indicate that a the scene node structure has changed.  This should 
	could be caught by the LocationComponent to reflect scene graph structure 
	in the graphics engine.
	*/

	class ParentChangedEvent : public SceneObjectEventMessage
	{
	public:
		ParentChangedEvent(SenderID sender_id = -1, double delay= 0) :
		  SceneObjectEventMessage( sender_id , delay)
		  {

		  }
	};
	typedef GASS_SHARED_PTR<ParentChangedEvent> ParentChangedEventPtr;

	/**
		Message sent after SceneObject is initialized (children included)
	*/
	class PostInitializedEvent : public SceneObjectEventMessage
	{
	public:
		PostInitializedEvent(SenderID sender_id = -1, double delay= 0) :   
		  SceneObjectEventMessage(sender_id , delay)
		  {

		  }
	};
	typedef GASS_SHARED_PTR<PostInitializedEvent> PostInitializedEventPtr;


	
}
