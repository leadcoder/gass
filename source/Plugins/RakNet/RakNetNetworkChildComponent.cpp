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

#include "RakNetNetworkChildComponent.h"
#include "Plugins/RakNet/RakNetNetworkSystem.h"
#include "Plugins/RakNet/RakNetChildReplica.h"
#include "Plugins/RakNet/RakNetMasterReplica.h"
#include "Plugins/RakNet/RakNetPackageFactory.h"
#include "Plugins/RakNet/RakNetNetworkMasterComponent.h"
#include "Plugins/RakNet/RakNetNetworkSceneManager.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"

namespace GASS
{
	RakNetNetworkChildComponent::RakNetNetworkChildComponent() : m_Replica(NULL), m_PartId(0)
	{

	}

	RakNetNetworkChildComponent::~RakNetNetworkChildComponent()
	{


	}

	void RakNetNetworkChildComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("NetworkChildComponent",new Creator<RakNetNetworkChildComponent, Component>);
		RegisterProperty<std::vector<std::string> >("Attributes", &RakNetNetworkChildComponent::GetAttributes, &RakNetNetworkChildComponent::SetAttributes);
	}

	void RakNetNetworkChildComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetNetworkChildComponent::OnSerialize,NetworkSerializeRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetNetworkChildComponent::OnGotReplica,ComponentGotReplicaEvent,0));

		RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<RakNetNetworkSystem>();
		if(!raknet->IsActive())
			return;

		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(RakNetNetworkChildComponent::OnNetworkPostUpdate,NetworkPostUpdateEvent,0));
		if(raknet->IsServer())
		{
			m_Replica = new RakNetChildReplica(raknet->GetReplicaManager());
			m_Replica->SetPartId(m_PartId);
			m_Replica->LocalInit(GetSceneObject());

			if(m_Attributes.size() > 0)
			{
				GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<RaknetNetworkSceneManager>()->Register(shared_from_this());
			}
				//SimEngine::GetPtr()->GetRuntimeController()->Register(this);
		}
		else
		{
		/*	if(m_Replica) //top object
			{
				m_Replica->SetOwner(GetSceneObject());
			}*/
			RakNetNetworkMasterComponentPtr master = GetSceneObject()->GetObjectUnderRoot()->GetFirstComponentByClass<RakNetNetworkMasterComponent>();
			if(master && master->GetReplica())
				m_Replica = raknet->FindReplica(master->GetReplica()->GetNetworkID(),m_PartId);

			if(m_Replica== NULL) //replica not available jet, trig serach on new child replica messages
			{
				SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(RakNetNetworkChildComponent::OnNewChildReplica,ChildReplicaCreatedEvent,0));
//
			}
			else
			   m_Replica->SetOwner(GetSceneObject());
		}
	}

	void RakNetNetworkChildComponent::OnNewChildReplica(ChildReplicaCreatedEventPtr message)
	{
		RakNetChildReplica* replica = message->GetReplica();
		RakNetNetworkMasterComponentPtr top_comp = GetSceneObject()->GetObjectUnderRoot()->GetFirstComponentByClass<RakNetNetworkMasterComponent>();
		if(top_comp->GetReplica())
		{
			NetworkID part_of_id = top_comp->GetReplica()->GetNetworkID();
			int part_id = GetPartId();
			if(replica->GetPartId() == part_id && replica->GetPartOfId() == part_of_id)
			{
				m_Replica = replica;
				m_Replica->SetOwner(GetSceneObject());
				GetSceneObject()->PostEvent(ComponentGotReplicaEventPtr(new ComponentGotReplicaEvent(m_Replica)));
				//this is not allowed, post to finalize object
				//SimEngine::Get().GetSimSystemManager()->UnregisterForMessage(UNREG_TMESS(RakNetNetworkChildComponent::OnNewChildReplica,ChildReplicaCreatedEvent));
			}
		}
	}

	void RakNetNetworkChildComponent::OnGotReplica(ComponentGotReplicaEventPtr message)
	{
		SimEngine::Get().GetSimSystemManager()->UnregisterForMessage(UNREG_TMESS(RakNetNetworkChildComponent::OnNewChildReplica,ChildReplicaCreatedEvent));
	}

	void RakNetNetworkChildComponent::OnDelete()
	{
		RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<RakNetNetworkSystem>();
		if(raknet->IsServer())
		{
			delete m_Replica;
			m_Replica = NULL;
		}
	}

	void RakNetNetworkChildComponent::OnSerialize(NetworkSerializeRequestPtr message)
	{
		bool found = false;
		for(size_t i = 0 ; i < m_SerializePackages.size(); i++)
		{
			if(m_SerializePackages[i]->Id == message->GetPackage()->Id)
			{
				m_SerializePackages[i] = message->GetPackage();
				found = true;
				break;
			}
		}
		if(!found)
			m_SerializePackages.push_back(message->GetPackage());

		RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<RakNetNetworkSystem>();
		SystemAddress address;

		address.binaryAddress = message->GetAddress().m_Address;
		address.port  = message->GetAddress().m_Port;

		//Signal serialize
		raknet->GetReplicaManager()->SignalSerializeNeeded((Replica*)m_Replica, address, true);
		//clear all packages
		//m_SerializePackages.clear();
	}

	void RakNetNetworkChildComponent::Serialize(bool *sendTimestamp, RakNet::BitStream *outBitStream, RakNetTime lastSendTime, PacketPriority *priority, PacketReliability *reliability, RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags)
	{
		int num_packs = static_cast<int>(m_SerializePackages.size());
		outBitStream->Write(num_packs);
		for(size_t i = 0 ; i < m_SerializePackages.size(); i++)
		{
			*sendTimestamp = true; //always include time stamp!
			outBitStream->Write(m_SerializePackages[i]->Id);
			//outBitStream->Write(m_SerializePackages[i]->Size);
			int size = m_SerializePackages[i]->GetSize();
			outBitStream->Write((char*)m_SerializePackages[i].get(),size);
		}

		if(m_Replica && m_Attributes.size() > 0)
			m_Replica->SerializeProperties(outBitStream);
	}

	void RakNetNetworkChildComponent::Deserialize(RakNet::BitStream *inBitStream, RakNetTime timestamp, RakNetTime lastDeserializeTime, SystemAddress systemAddress )
	{
		int num_packs = 0;
		inBitStream->Read(num_packs);
		for(int i = 0 ; i < num_packs; i++)
		{
			//NetworkSerializeMessage::NetworkPackage package;
			//inBitStream->Read(package.Id);
			int id;
			inBitStream->Read(id);

			NetworkPackagePtr package = PackageFactory::Get().Create(id);
			if (package)
			{
				int size = package->GetSize();
				char* data_to_read = new char[size];
				inBitStream->Read(data_to_read, size);
				package->Assign(data_to_read);
				delete[] data_to_read;
				GetSceneObject()->PostRequest(NetworkDeserializeRequestPtr(new NetworkDeserializeRequest(NetworkAddress(systemAddress.binaryAddress,systemAddress.port),timestamp,package)));
			}
		}
		if(m_Replica && m_Attributes.size() > 0)
			m_Replica->DeserializeProperties(inBitStream);
	}


	void RakNetNetworkChildComponent::SceneManagerTick(double delta)
	{
		//check if attributes are changed
		if(m_Replica && m_Replica->HasPropertiesChanged())
		{
			//std::cout << "props has changed\n";
			RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<RakNetNetworkSystem>();
			SystemAddress address = UNASSIGNED_SYSTEM_ADDRESS;

			//Signal serialize
			raknet->GetReplicaManager()->SignalSerializeNeeded((Replica*)m_Replica, address, true);
		}
	}

	void RakNetNetworkChildComponent::OnNetworkPostUpdate(NetworkPostUpdateEventPtr message)
	{
		//everything is sent!
		m_SerializePackages.clear();
		if(m_Replica)
			m_Replica->ProcessMessages();
	}

}

