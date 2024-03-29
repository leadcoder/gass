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

#include "RakNetMasterReplica.h"

#include "Sim/GASSSceneObjectTemplateManager.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSScene.h"
#include "RakNetNetworkMasterComponent.h"
#include "RakNetNetworkSystem.h"
#include "Sim/Messages/GASSPlatformMessages.h"

namespace GASS
{
	RakNetMasterReplica::RakNetMasterReplica(ReplicaManager* manager): m_Manager(manager)
	{
		m_OwnerSystemAddress = UNASSIGNED_SYSTEM_ADDRESS;
		m_AllowRemoteOwner = false;
	}

	RakNetMasterReplica::~RakNetMasterReplica()
	{
		m_Manager->Destruct(this, UNASSIGNED_SYSTEM_ADDRESS, true); // Forward the destruct message to all other systems but the sender
		m_Manager->DereferencePointer(this);
	}

	void RakNetMasterReplica::LocalInit(SceneObjectPtr object)
	{
		SetOwner(object);
		m_TemplateName = object->GetTemplateName();

		RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<RakNetNetworkSystem>();

		SetNetworkIDManager(raknet->GetNetworkIDManager());

		SetOwnerSystemAddress(raknet->GetRakPeer()->GetInternalID());


		// For security, as a server disable these interfaces
		if (raknet->IsServer())
		{
			m_Manager->Construct(this, false, UNASSIGNED_SYSTEM_ADDRESS, true);
			// For security, as a server disable all receives except REPLICA_RECEIVE_SERIALIZE
			// I could do this manually by putting if (isServer) return; at the top of all my receive functions too.
			m_Manager->DisableReplicaInterfaces(this, REPLICA_RECEIVE_DESTRUCTION | REPLICA_RECEIVE_SCOPE_CHANGE );
		}
		else
		{
			// For convenience and for saving bandwidth, as a client disable all sends except REPLICA_SEND_SERIALIZE
			// I could do this manually by putting if (isServer==false) return; at the top of all my send functions too.
			m_Manager->DisableReplicaInterfaces(this, REPLICA_SEND_CONSTRUCTION | REPLICA_SEND_DESTRUCTION | REPLICA_SEND_SCOPE_CHANGE );
		}
	}

	void RakNetMasterReplica::RemoteInit(RakNet::BitStream *inBitStream, RakNetTime /*timestamp*/, NetworkID network_id, SystemAddress senderId)
	{
		RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<RakNetNetworkSystem>();

		SetNetworkIDManager(raknet->GetNetworkIDManager());
		// We must set the network ID of all remote objects
		SetNetworkID(network_id);
		// Tell the replica manager to create this as an object that originated on a remote node
		m_Manager->Construct(this, true, senderId, false);
		// Since SendConstruction is not called for copies and we were calling SetScope there, we need to call it here instead.
		m_Manager->SetScope(this, true, senderId, false);
		ReceiveConstruction(inBitStream);

		if (raknet->IsServer())
		{
			// For security, as a server disable all receives except REPLICA_RECEIVE_SERIALIZE
			// I could do this manually by putting if (isServer) return; at the top of all my receive functions too.
			m_Manager->DisableReplicaInterfaces(this, REPLICA_RECEIVE_DESTRUCTION | REPLICA_RECEIVE_SCOPE_CHANGE );
			//std::cout << "RakNetMasterReplica::RemoteInit - server " << std::endl;
		}
		else
		{
			// For convenience and for saving bandwidth, as a client disable all sends except REPLICA_SEND_SERIALIZE
			// I could do this manually by putting if (isServer==false) return; at the top of all my send functions too.
			m_Manager->DisableReplicaInterfaces(this, REPLICA_SEND_CONSTRUCTION | REPLICA_SEND_DESTRUCTION | REPLICA_SEND_SCOPE_CHANGE );
			//std::cout << "RakNetMasterReplica::RemoteInit - client" << std::endl;
		}

		// if this is a server and we receive a remotely created object, we must forward that creation
		// to all of our clients
		if (raknet->IsServer())
		{
			m_Manager->Construct(this, false, senderId, true);
		}
	}

	ReplicaReturnResult RakNetMasterReplica::SendConstruction( RakNetTime /*currentTime*/, SystemAddress /*systemAddress*/, unsigned int &/*flags*/, RakNet::BitStream *outBitStream, bool* /*includeTimestamp*/ )
	{
		// Don't send back to the owner of an object.
		// If we didn't prevent then the object would be created on the system that just sent it to us, then back again, forever in a feedback loop.
		//if (playerId==m_Owner)
		//	return REPLICA_PROCESSING_DONE;
		// This string was pre-registered in main with stringTable->AddString so we can send it with the string table and save bandwidth
		RakNet::StringTable::Instance()->EncodeString("RakNetMasterReplica", 255, outBitStream);
		// Write the owner when we construct the object, so we have it right away in order to prevent feedback loops
		RakNetMasterReplica::SendConstruction(outBitStream);
		//send part id
		return REPLICA_PROCESSING_DONE;
	}


	void RakNetMasterReplica::SendConstruction(RakNet::BitStream *outBitStream)
	{
		outBitStream->Write(m_OwnerSystemAddress);
		assert(m_Owner);

		//std::string name = m_Owner->GetName();
		std::string template_name = m_Owner->GetTemplateName();
		//RakNetNetworkManager::WriteString(name,outBitStream);
		RakNetNetworkSystem::WriteString(template_name,outBitStream);
	}

	IProperty* RakNetMasterReplica::GetProperty(const std::string &prop_name) const
	{
		m_Owner->GetComponents();
		auto comp_iter = m_Owner->GetComponents();

		while(comp_iter.hasMoreElements())
		{
			auto comp = GASS_STATIC_PTR_CAST<Component>(comp_iter.getNext());
			if(comp)
			{
				RTTI* p_rtti = comp->GetRTTI();
				while(p_rtti)
				{
					auto	iter = p_rtti->GetFirstProperty();
					while(iter != p_rtti->GetProperties()->end())
					{
						IProperty * prop = (*iter);
						if(prop->GetName() == prop_name)
						{
							return prop;
						}
						++iter;
					}
					p_rtti = p_rtti->GetAncestorRTTI();
				}
			}
		}
		return nullptr;
	}

	void RakNetMasterReplica::SerializeProperties(RakNet::BitStream *bit_stream)
	{
		RakNetNetworkMasterComponentPtr nc = m_Owner->GetFirstComponentByClass<RakNetNetworkMasterComponent>();
		std::vector<std::string> attributes = nc->GetAttributes();
		SerialSaver ss(nullptr,0);
		//for(size_t i = 0 ;  i < attributes.size(); i++)
		//{
			//IProperty * prop = GetProperty(attributes[i]);
			//prop->Serialize(&ss);
		//}
		unsigned long size=ss.GetLength();
		auto *buffer=new unsigned char[size];
		SerialSaver sv(buffer,size);

		//for(int i = 0 ;  i < attributes.size(); i++)
		//{
			//IProperty * prop = GetProperty(attributes[i]);
			//prop->Serialize(&sv);
		//}

		bit_stream->Write(size);
		if	(size > 0)
		{
			char* send_buf = (char*)(buffer);
			bit_stream->Write(send_buf,size);
		}
	}

	void RakNetMasterReplica::ReceiveConstruction(RakNet::BitStream *inBitStream)
	{
		inBitStream->Read(m_OwnerSystemAddress);
		m_TemplateName = RakNetNetworkSystem::ReadString(inBitStream);
	}

	ReplicaReturnResult  RakNetMasterReplica::SendDestruction(RakNet::BitStream * /*outBitStream*/, SystemAddress /*systemAddress*/, bool * /*includeTimestamp*/)
	{
		// Optional, nothing to send here.
		return REPLICA_PROCESSING_DONE;
	}

	ReplicaReturnResult RakNetMasterReplica::ReceiveDestruction(RakNet::BitStream * /*inBitStream*/, SystemAddress /*systemAddress*/, RakNetTime /*timestamp*/)
	{
		SceneObjectPtr obj(m_Owner);
		if(obj) //remove object
		{
			obj->SendRemoveRequest(0);
		}

		delete this;
		return REPLICA_PROCESSING_DONE;
	}

	ReplicaReturnResult RakNetMasterReplica::SendScopeChange(bool inScope, RakNet::BitStream *outBitStream, RakNetTime /*currentTime*/, SystemAddress /*systemAddress*/, bool * /*includeTimestamp*/)
	{
		outBitStream->Write(inScope);
		return REPLICA_PROCESSING_DONE;
	}

	ReplicaReturnResult RakNetMasterReplica::ReceiveScopeChange(RakNet::BitStream * /*inBitStream*/, SystemAddress /*systemAddress*/, RakNetTime /*timestamp*/)
	{
		return REPLICA_PROCESSING_DONE;
	}

	ReplicaReturnResult RakNetMasterReplica::Serialize(bool *sendTimestamp, RakNet::BitStream *outBitStream, RakNetTime lastSendTime, PacketPriority *priority, PacketReliability *reliability, RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags)
	{
		// Don't send back to the owner of an object.
		//if (playerId==m_Owner)
		//	return REPLICA_PROCESSING_DONE;
		//outBitStream->Write(testInteger);

		RakNetNetworkMasterComponentPtr net_obj = m_Owner->GetFirstComponentByClass<RakNetNetworkMasterComponent>();
		net_obj->Serialize(sendTimestamp, outBitStream, lastSendTime, priority, reliability, currentTime, systemAddress, flags);
		return REPLICA_PROCESSING_DONE;
	}

	ReplicaReturnResult RakNetMasterReplica::Deserialize(RakNet::BitStream *inBitStream, RakNetTime timestamp, RakNetTime lastDeserializeTime, SystemAddress systemAddress )
	{
		//inBitStream->Read(m_DataToReceive);
		if(m_Owner)
		{
			RakNetNetworkMasterComponentPtr net_obj = m_Owner->GetFirstComponentByClass<RakNetNetworkMasterComponent>();
			net_obj->Deserialize(inBitStream, timestamp, lastDeserializeTime, systemAddress );
			// If this is a server
			RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<RakNetNetworkSystem>();
			if (raknet->IsServer())
			{
				// Synchronisation events should be forwarded to other clients
				//m_Manager->SignalSerializeNeeded(this, systemAddress, true);
			}
		}
		//raknet->GetReplicaManager()->SignalSerializeNeeded(this, playerId, true);
		return REPLICA_PROCESSING_DONE;
	}
	//handle RPC
	/*int RakNetMasterReplica::EnterObject(const char *str, RakNet::AutoRPC* networkCaller)
	{
		if(m_Owner)
		{
			RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<RakNetNetworkSystem>();
			if (networkCaller==0)
			{
				raknet->GetRPC()->SetRecipientObject(GetNetworkID());
				raknet->GetRPC()->Call("RakNetMasterReplica::EnterObject", str);
				raknet->GetRPC()->SetRecipientObject(UNASSIGNED_NETWORK_ID);
				std::cout << "EnterObject called from server" <<std::endl;
			}
			else
			{
				if(std::string(str)  == std::string(raknet->GetRakPeer()->GetInternalID().ToString()))
				{
					MessagePtr enter_msg(new EnterVehicleMessage());
					m_Owner->PostMessage(enter_msg);
				}
				std::cout << "EnterObject called from client, target address:" << std::string(str) << " client address:" <<  std::string(raknet->GetRakPeer()->GetInternalID().ToString()) <<std::endl;
			}
		}
		//printf("object member a=%i memberVariable=%i sender=%s\n", a, memberVariable, networkCaller==0 ? "N/A" : networkCaller->GetLastSenderAddress().ToString());
		return 1;
	}*/
}
