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

#ifndef RAK_NET_MESSAGES_H
#define RAK_NET_MESSAGES_H

#include "Core/MessageSystem/GASSBaseMessage.h"
#include "Core/Math/GASSVector.h"
#include "Sim/Messages/GASSCoreSystemMessages.h"

namespace GASS
{
	/*class RakNetBase;
	class ReplicaCreatedMessage : public BaseMessage
	{
	public:
		ReplicaCreatedMessage(RakNetBase* replica, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), m_Replica(replica){}
		  RakNetBase* GetReplica() const {return m_Replica;}
	private:
		RakNetBase* m_Replica;
	};
	typedef GASS_SHARED_PTR<ReplicaCreatedMessage> ReplicaCreatedMessagePtr;*/

	class RakNetMasterReplica;
	class MasterReplicaCreatedEvent : public SystemEventMessage
	{
	public:
		MasterReplicaCreatedEvent(RakNetMasterReplica* replica, SenderID sender_id = -1, double delay= 0) : 
		  SystemEventMessage(sender_id , delay), m_Replica(replica){}
		  RakNetMasterReplica* GetReplica() const {return m_Replica;}
	private:
		RakNetMasterReplica* m_Replica;
	};
	typedef GASS_SHARED_PTR<MasterReplicaCreatedEvent> MasterReplicaCreatedEventPtr;


	class RakNetChildReplica;
	class ChildReplicaCreatedEvent : public SystemEventMessage
	{
	public:
		ChildReplicaCreatedEvent (RakNetChildReplica* replica, SenderID sender_id = -1, double delay= 0) : 
		  SystemEventMessage(sender_id , delay), m_Replica(replica){}
		  RakNetChildReplica* GetReplica() const {return m_Replica;}
	private:
		RakNetChildReplica* m_Replica;
	};
	typedef GASS_SHARED_PTR<ChildReplicaCreatedEvent > ChildReplicaCreatedEventPtr;

	class ComponentGotReplicaEvent : public SceneObjectEventMessage
	{
	public:
		ComponentGotReplicaEvent(RakNetChildReplica* replica, SenderID sender_id = -1, double delay= 0) : 
		  SceneObjectEventMessage(sender_id , delay), m_Replica(replica){}
		  RakNetChildReplica* GetReplica() const {return m_Replica;}
	private:
		RakNetChildReplica* m_Replica;
	};
	typedef GASS_SHARED_PTR<ComponentGotReplicaEvent> ComponentGotReplicaEventPtr;


	class NetworkPostUpdateEvent : public SystemEventMessage
	{
	public:
		NetworkPostUpdateEvent(SenderID sender_id = -1, double delay= 0) : 
		  SystemEventMessage(sender_id , delay)
		  {

		  }
		
	};
	typedef GASS_SHARED_PTR<NetworkPostUpdateEvent> NetworkPostUpdateEventPtr;


	
}
#endif
