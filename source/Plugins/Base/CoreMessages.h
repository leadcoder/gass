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

#ifndef CORE_MESSAGES_H
#define CORE_MESSAGES_H
#include "Sim/GASSCommon.h"
#include "Core/MessageSystem/GASSBaseMessage.h"
#include "Sim/GASSSceneObject.h"


namespace GASS
{
	/*class UpdateWaypointListMessage : public BaseMessage
	{
	public:
		UpdateWaypointListMessage(SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay){}
	private:
	};
	typedef GASS_SHARED_PTR<UpdateWaypointListMessage> UpdateWaypointListMessagePtr;*/

	class WaypointListUpdatedMessage : public SceneObjectEventMessage
	{
	public:
		WaypointListUpdatedMessage(const std::vector<Vec3> &wps, SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectEventMessage(sender_id , delay) , m_Waypoints(wps){}
		  std::vector<Vec3> m_Waypoints;
	private:
		
	};
	typedef GASS_SHARED_PTR<WaypointListUpdatedMessage> WaypointListUpdatedMessagePtr;


	class TriggerEnterMessage : public SceneObjectEventMessage
	{
	public:
		TriggerEnterMessage(SceneObjectPtr obj, SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectEventMessage(sender_id , delay) , m_Trigger(obj){}
		  SceneObjectPtr GetTrigger() {return m_Trigger;}
	private:
		SceneObjectPtr  m_Trigger;
	};
	typedef GASS_SHARED_PTR<TriggerEnterMessage> TriggerEnterMessagePtr;


	class TriggerExitMessage : public SceneObjectEventMessage
	{
	public:
		TriggerExitMessage (SceneObjectPtr obj, SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectEventMessage(sender_id , delay) , m_Trigger(obj){}
		  SceneObjectPtr GetTrigger() {return m_Trigger;}
	private:
		SceneObjectPtr  m_Trigger;
		
	};
	typedef GASS_SHARED_PTR<TriggerExitMessage > TriggerExitMessagePtr;


}
#endif
