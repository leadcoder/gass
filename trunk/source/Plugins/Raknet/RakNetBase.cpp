/****************************************************************************
* This file is part of GASS.                                                *
* See http://sourceforge.net/projects/gass/                                 *
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


#include "RakPeerInterface.h"
#include "ReplicaManager.h"


#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/SimEngine.h"
#include "Sim/Systems/SimSystemManager.h"

#include "RakNetBase.h"
#include "RakNetNetworkSystem.h"
#include "RakNetReplicaMember.h"


namespace GASS
{
	RakNetBase::RakNetBase(ReplicaManager* manager): m_Manager(manager)
	{
		m_PartId = 0;
		m_OwnerSystemAddress = UNASSIGNED_SYSTEM_ADDRESS;
		m_Replica = NULL;
		m_AllowRemoteOwner = false;
	}

	RakNetBase::~RakNetBase()
	{
		if(m_Replica)
		{
			m_Manager->Destruct((Replica*)m_Replica, UNASSIGNED_SYSTEM_ADDRESS, true); // Forward the destruct message to all other systems but the sender
			m_Manager->DereferencePointer((Replica*)m_Replica);
			delete m_Replica;
		}
	}

	void RakNetBase::LocalInit(SceneObjectPtr object)
	{
		m_Owner = object;
		m_TemplateName = object->GetTemplateName();
		//m_PartId = object->GetPartId();

		m_Replica = new RakNetReplicaMember();
		m_Replica->SetParent(this);

		RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<RakNetNetworkSystem>();
		
		m_Replica->SetNetworkIDManager(raknet->GetNetworkIDManager());
		SceneObjectPtr object_root =  object->GetObjectUnderRoot();
		if(object_root)
		{
			RakNetNetworkComponentPtr root_net_obj = object_root->GetFirstComponent<RakNetNetworkComponent>();
			if(root_net_obj)
				m_PartOfId = root_net_obj->GetReplica()->GetReplica()->GetNetworkID();
		}
		SetOwnerSystemAddress(raknet->GetRakPeer()->GetInternalID());
		if (raknet->IsServer())
			m_Manager->Construct(m_Replica , false, UNASSIGNED_SYSTEM_ADDRESS, true);

		// For security, as a server disable these interfaces
		if (raknet->IsServer())
		{
			// For security, as a server disable all receives except REPLICA_RECEIVE_SERIALIZE
			// I could do this manually by putting if (isServer) return; at the top of all my receive functions too.
			m_Manager->DisableReplicaInterfaces(m_Replica, REPLICA_RECEIVE_DESTRUCTION | REPLICA_RECEIVE_SCOPE_CHANGE );
		}
		else
		{
			// For convenience and for saving bandwidth, as a client disable all sends except REPLICA_SEND_SERIALIZE
			// I could do this manually by putting if (isServer==false) return; at the top of all my send functions too.
			m_Manager->DisableReplicaInterfaces(m_Replica, REPLICA_SEND_CONSTRUCTION | REPLICA_SEND_DESTRUCTION | REPLICA_SEND_SCOPE_CHANGE );
		}
	}

	void RakNetBase::RemoteInit(RakNet::BitStream *inBitStream, RakNetTime timestamp, NetworkID networkID, SystemAddress senderId)
	{
		m_Replica = new RakNetReplicaMember();
		m_Replica->SetParent(this);
		RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<RakNetNetworkSystem>();
		
		m_Replica->SetNetworkIDManager(raknet->GetNetworkIDManager());
		// We must set the network ID of all remote objects
		m_Replica->SetNetworkID(networkID);
		// Tell the replica manager to create this as an object that originated on a remote node
		m_Manager->Construct(m_Replica, true, senderId, false);
		// Since SendConstruction is not called for copies and we were calling SetScope there, we need to call it here instead.
		m_Manager->SetScope(m_Replica, true, senderId, false);
		ReceiveConstruction(inBitStream);

		if (raknet->IsServer())
		{
			// For security, as a server disable all receives except REPLICA_RECEIVE_SERIALIZE
			// I could do this manually by putting if (isServer) return; at the top of all my receive functions too.
			m_Manager->DisableReplicaInterfaces(m_Replica, REPLICA_RECEIVE_DESTRUCTION | REPLICA_RECEIVE_SCOPE_CHANGE );
		}
		else
		{
			// For convenience and for saving bandwidth, as a client disable all sends except REPLICA_SEND_SERIALIZE
			// I could do this manually by putting if (isServer==false) return; at the top of all my send functions too.
			m_Manager->DisableReplicaInterfaces(m_Replica, REPLICA_SEND_CONSTRUCTION | REPLICA_SEND_DESTRUCTION | REPLICA_SEND_SCOPE_CHANGE );
		}

		// if this is a server and we receive a remotely created object, we must forward that creation
		// to all of our clients
		if (raknet->IsServer())
		{
			m_Manager->Construct(m_Replica, false, senderId, true);
		}

		if(m_TemplateName != "") //check is this a top object
		{
			//Create object based on template name
			
		}
	}

	ReplicaReturnResult RakNetBase::SendConstruction( RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags, RakNet::BitStream *outBitStream, bool *includeTimestamp )
	{
		// Don't send back to the owner of an object.
		// If we didn't prevent then the object would be created on the system that just sent it to us, then back again, forever in a feedback loop.
		//if (playerId==m_Owner)
		//	return REPLICA_PROCESSING_DONE;
		// This string was pre-registered in main with stringTable->AddString so we can send it with the string table and save bandwidth
		RakNet::StringTable::Instance()->EncodeString("RakNetBase", 255, outBitStream);
		// Write the owner when we construct the object, so we have it right away in order to prevent feedback loops
		RakNetBase::SendConstruction(outBitStream);
		//send part id
		return REPLICA_PROCESSING_DONE;
	}


	void RakNetBase::SendConstruction(RakNet::BitStream *outBitStream)
	{
		outBitStream->Write(m_OwnerSystemAddress);
		assert(m_Owner);
		outBitStream->Write(m_PartId);
		outBitStream->Write(m_PartOfId);
		//std::string name = m_Owner->GetName();
		std::string template_name = m_Owner->GetTemplateName();
		//RakNetNetworkManager::WriteString(name,outBitStream);
		RakNetNetworkSystem::WriteString(template_name,outBitStream);
	}

	AbstractProperty* RakNetBase::GetProperty(const std::string &prop_name)
	{
		m_Owner->GetComponents();
		IComponentContainer::ComponentIterator comp_iter = m_Owner->GetComponents();

		while(comp_iter.hasMoreElements())
		{
			BaseSceneComponentPtr comp = boost::shared_static_cast<BaseSceneComponent>(comp_iter.getNext());
			if(comp)
			{				
				RTTI* pRTTI = comp->GetRTTI();
				while(pRTTI)
				{
					std::list<AbstractProperty*>::iterator	iter = pRTTI->GetFirstProperty();
					while(iter != pRTTI->GetProperties()->end())
					{
						AbstractProperty * prop = (*iter);
						if(prop->GetName() == prop_name)
						{
							return prop;
						}
						iter++;
					}
					pRTTI = pRTTI->GetAncestorRTTI();
				}
			}
		}
		return NULL;
	}

	

	void RakNetBase::SerializeProperties(RakNet::BitStream *bit_stream)
	{
		RakNetNetworkComponentPtr nc = m_Owner->GetFirstComponent<RakNetNetworkComponent>();
		std::vector<std::string> attributes = nc->GetAttributes();
		SerialSaver ss(NULL,0);
		for(int i = 0 ;  i < attributes.size(); i++)
		{
			//AbstractProperty * prop = GetProperty(attributes[i]);
			//prop->Serialize(&ss);
		}
		unsigned long size=ss.getLength();
		unsigned char *buffer=new unsigned char[size];
		SerialSaver sv(buffer,size);

		for(int i = 0 ;  i < attributes.size(); i++)
		{
			//AbstractProperty * prop = GetProperty(attributes[i]);
			//prop->Serialize(&sv);
		}

		bit_stream->Write(size);
		if	(size > 0)
		{
			char* send_buf = (char*)(buffer);
			bit_stream->Write(send_buf,size);
		}
	}

	void RakNetBase::ReceiveConstruction(RakNet::BitStream *inBitStream)
	{
		inBitStream->Read(m_OwnerSystemAddress);
		inBitStream->Read(m_PartId);
		inBitStream->Read(m_PartOfId);
		m_TemplateName = RakNetNetworkSystem::ReadString(inBitStream);
	}

	ReplicaReturnResult  RakNetBase::SendDestruction(RakNet::BitStream *outBitStream, SystemAddress systemAddress, bool *includeTimestamp)
	{
		// Optional, nothing to send here.
		return REPLICA_PROCESSING_DONE;
	}

	ReplicaReturnResult RakNetBase::ReceiveDestruction(RakNet::BitStream *inBitStream, SystemAddress systemAddress, RakNetTime timestamp)
	{
		//printf("Remote object owned by %s:%i destroyed\n", rakPeer->PlayerIDToDottedIP(owner), owner.port);
		delete this;
		return REPLICA_PROCESSING_DONE;
	}

	ReplicaReturnResult RakNetBase::SendScopeChange(bool inScope, RakNet::BitStream *outBitStream, RakNetTime currentTime, SystemAddress systemAddress, bool *includeTimestamp)
	{
		outBitStream->Write(inScope);
		return REPLICA_PROCESSING_DONE;
	}

	ReplicaReturnResult RakNetBase::ReceiveScopeChange(RakNet::BitStream *inBitStream, SystemAddress systemAddress, RakNetTime timestamp)
	{
		return REPLICA_PROCESSING_DONE;
	}

	ReplicaReturnResult RakNetBase::Serialize(bool *sendTimestamp, RakNet::BitStream *outBitStream, RakNetTime lastSendTime, PacketPriority *priority, PacketReliability *reliability, RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags)
	{
		// Don't send back to the owner of an object.
		//if (playerId==m_Owner)
		//	return REPLICA_PROCESSING_DONE;
		//outBitStream->Write(testInteger);

		RakNetNetworkComponentPtr net_obj = m_Owner->GetFirstComponent<RakNetNetworkComponent>();
		net_obj->Serialize(sendTimestamp, outBitStream, lastSendTime, priority, reliability, currentTime, systemAddress, flags);
		return REPLICA_PROCESSING_DONE;
	}
	ReplicaReturnResult RakNetBase::Deserialize(RakNet::BitStream *inBitStream, RakNetTime timestamp, RakNetTime lastDeserializeTime, SystemAddress systemAddress )
	{
		//inBitStream->Read(m_DataToReceive);

		RakNetNetworkComponentPtr net_obj = m_Owner->GetFirstComponent<RakNetNetworkComponent>();
		net_obj->Deserialize(inBitStream, timestamp, lastDeserializeTime, systemAddress );
		// If this is a server
		RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<RakNetNetworkSystem>();
		if (raknet->IsServer())
		{
			// Synchronisation events should be forwarded to other clients
			m_Manager->SignalSerializeNeeded(m_Replica, systemAddress, true);
		}
		//raknet->GetReplicaManager()->SignalSerializeNeeded(this, playerId, true);
		return REPLICA_PROCESSING_DONE;
	}

	// Interface from NetworkIDGenerator.
	bool RakNetBase::IsMaster()
	{
		return true;//raknet->IsMaster(this);
	}					
}
