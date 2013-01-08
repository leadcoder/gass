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

#ifndef RAK_NET_MESSAGES_H
#define RAK_NET_MESSAGES_H
#include "Core/MessageSystem/GASSBaseMessage.h"
#include "Core/Math/GASSVector.h"

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
	typedef boost::shared_ptr<ReplicaCreatedMessage> ReplicaCreatedMessagePtr;*/

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
	typedef boost::shared_ptr<MasterReplicaCreatedEvent> MasterReplicaCreatedEventPtr;


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
	typedef boost::shared_ptr<ChildReplicaCreatedEvent > ChildReplicaCreatedEventPtr;

	class ComponentGotReplicaEvent : public SystemEventMessage
	{
	public:
		ComponentGotReplicaEvent(RakNetChildReplica* replica, SenderID sender_id = -1, double delay= 0) : 
		  SystemEventMessage(sender_id , delay), m_Replica(replica){}
		  RakNetChildReplica* GetReplica() const {return m_Replica;}
	private:
		RakNetChildReplica* m_Replica;
	};
	typedef boost::shared_ptr<ComponentGotReplicaEvent> ComponentGotReplicaEventPtr;


	class NetworkPostUpdateEvent : public SystemEventMessage
	{
	public:
		NetworkPostUpdateEvent(SenderID sender_id = -1, double delay= 0) : 
		  SystemEventMessage(sender_id , delay)
		  {

		  }
		
	};
	typedef boost::shared_ptr<NetworkPostUpdateEvent> NetworkPostUpdateEventPtr;


	
}
#endif
