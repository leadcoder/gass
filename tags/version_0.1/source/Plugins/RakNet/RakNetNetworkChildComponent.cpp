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

#include "RakNetNetworkChildComponent.h"
#include "Plugins/RakNet/RakNetNetworkSystem.h"
#include "Plugins/RakNet/RakNetChildReplica.h"
#include "Plugins/RakNet/RakNetMasterReplica.h"
#include "Plugins/RakNet/RakNetNetworkMasterComponent.h"
#include "Plugins/RakNet/RakNetLocationTransferComponent.h"

#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/Scenario/Scenario.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"

#include "Sim/Systems/Resource/IResourceSystem.h"
#include "Sim/SimEngine.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Scheduling/IRuntimeController.h"
#include "Sim/Systems/Input/ControlSettingsManager.h"
#include "Sim/Systems/Input/ControlSetting.h"
#include "Sim/Components/Graphics/ICameraComponent.h"


namespace GASS
{
	RakNetNetworkChildComponent::RakNetNetworkChildComponent() : m_Replica(NULL)
	{

	}

	RakNetNetworkChildComponent::~RakNetNetworkChildComponent()
	{
		

	}

	void RakNetNetworkChildComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("NetworkChildComponent",new Creator<RakNetNetworkChildComponent, IComponent>);
		RegisterVectorProperty<std::string>("Attributes", &RakNetNetworkChildComponent::GetAttributes, &RakNetNetworkChildComponent::SetAttributes);
	}

	void RakNetNetworkChildComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetNetworkChildComponent::OnUnload,UnloadComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetNetworkChildComponent::OnLoad,LoadNetworkComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetNetworkChildComponent::OnSerialize,NetworkSerializeMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetNetworkChildComponent::OnGotReplica,ComponentGotReplicaMessage,0));
		
	}

	void RakNetNetworkChildComponent::OnLoad(LoadNetworkComponentsMessagePtr message)
	{
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(RakNetNetworkChildComponent::OnNetworkPostUpdate,NetworkPostUpdateMessage,0));
		RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<RakNetNetworkSystem>();
		if(raknet->IsServer())
		{
			m_Replica = new RakNetChildReplica(raknet->GetReplicaManager());
			m_Replica->SetPartId(m_PartId);
			m_Replica->LocalInit(GetSceneObject());

			if(m_Attributes.size() > 0)
				SimEngine::GetPtr()->GetRuntimeController()->Register(this);
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
				SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(RakNetNetworkChildComponent::OnNewChildReplica,ChildReplicaCreatedMessage,0));
//					
			}
			else
			   m_Replica->SetOwner(GetSceneObject());
		}
	}

	void RakNetNetworkChildComponent::OnNewChildReplica(ChildReplicaCreatedMessagePtr message)
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
				GetSceneObject()->PostMessage(MessagePtr(new ComponentGotReplicaMessage(m_Replica)));
				//this is not allowed, post to finalize object
				//SimEngine::Get().GetSimSystemManager()->UnregisterForMessage(UNREG_TMESS(RakNetNetworkChildComponent::OnNewChildReplica,ChildReplicaCreatedMessage));
			}
		}
	}

	void RakNetNetworkChildComponent::OnGotReplica(ComponentGotReplicaMessagePtr message)
	{
		SimEngine::Get().GetSimSystemManager()->UnregisterForMessage(UNREG_TMESS(RakNetNetworkChildComponent::OnNewChildReplica,ChildReplicaCreatedMessage));
	}

	void RakNetNetworkChildComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
		RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<RakNetNetworkSystem>();
		if(raknet->IsServer())
		{
			delete m_Replica;
			m_Replica = NULL;
		}
	}

	void RakNetNetworkChildComponent::OnSerialize(NetworkSerializeMessagePtr message)
	{
		bool found = false;
		for(int i = 0 ; i < m_SerializePackages.size(); i++)
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
		
		RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<RakNetNetworkSystem>();
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
		int num_packs = m_SerializePackages.size();
		outBitStream->Write(num_packs);
		for(int i = 0 ; i < m_SerializePackages.size(); i++)
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
			if(package)
			{
				int size = package->GetSize();
				char* data_to_read = new char[size];
				inBitStream->Read(data_to_read,size);
				package->Assign(data_to_read);
				delete data_to_read ;
				GetSceneObject()->PostMessage(MessagePtr(new NetworkDeserializeMessage(NetworkAddress(systemAddress.binaryAddress,systemAddress.port),timestamp,package)));
			}
		}
		if(m_Replica && m_Attributes.size() > 0)
			m_Replica->DeserializeProperties(inBitStream);
	}


	void RakNetNetworkChildComponent::Update(double delta)
	{
		//check if attributes are changed
		if(m_Replica && m_Replica->HasPropertiesChanged())
		{
			//std::cout << "props has changed\n";
			RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<RakNetNetworkSystem>();
			SystemAddress address = UNASSIGNED_SYSTEM_ADDRESS;
			
			//Signal serialize
			raknet->GetReplicaManager()->SignalSerializeNeeded((Replica*)m_Replica, address, true);
		}
	}

	void RakNetNetworkChildComponent::OnNetworkPostUpdate(NetworkPostUpdateMessagePtr message)
	{
		//everything is sent!
		m_SerializePackages.clear();
	}

	TaskGroup RakNetNetworkChildComponent::GetTaskGroup() const 
	{
		return NETWORK_TASK_GROUP;
	}
}

