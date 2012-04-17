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

#ifndef CORE_MESSAGES_H
#define CORE_MESSAGES_H

#include "Core/MessageSystem/BaseMessage.h"
#include "Sim/Scenario/Scene/SceneObject.h"


namespace GASS
{
	class CoreSceneManager;
	typedef boost::shared_ptr<CoreSceneManager> CoreSceneManagerPtr;
	class LoadCoreComponentsMessage : public BaseMessage
	{
	public:
		LoadCoreComponentsMessage(CoreSceneManagerPtr core_scene_manager, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_CoreSceneManager(core_scene_manager){}
		  CoreSceneManagerPtr GetCoreSceneManager() const {return m_CoreSceneManager;}
	private:
		CoreSceneManagerPtr m_CoreSceneManager;
	};
	typedef boost::shared_ptr<LoadCoreComponentsMessage> LoadCoreComponentsMessagePtr;


	class UpdateWaypointListMessage : public BaseMessage
	{
	public:
		UpdateWaypointListMessage(SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay){}
	private:
	};
	typedef boost::shared_ptr<UpdateWaypointListMessage> UpdateWaypointListMessagePtr;

	class WaypointListUpdatedMessage : public BaseMessage
	{
	public:
		WaypointListUpdatedMessage(const std::vector<Vec3> &wps, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay) , m_Waypoints(wps){}
		  std::vector<Vec3> m_Waypoints;
	private:
		
	};
	typedef boost::shared_ptr<WaypointListUpdatedMessage> WaypointListUpdatedMessagePtr;


	class TriggerEnterMessage : public BaseMessage
	{
	public:
		TriggerEnterMessage(SceneObjectPtr obj, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay) , m_Trigger(obj){}
		  SceneObjectPtr  m_Trigger;
	private:
	};
	typedef boost::shared_ptr<TriggerEnterMessage> TriggerEnterMessagePtr;


	class TriggerExitMessage : public BaseMessage
	{
	public:
		TriggerExitMessage (SceneObjectPtr obj, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay) , m_Trigger(obj){}
		  SceneObjectPtr  m_Trigger;
	private:
		
	};
	typedef boost::shared_ptr<TriggerExitMessage > TriggerExitMessagePtr;


}
#endif
