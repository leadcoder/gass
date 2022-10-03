/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                        *
*                                                                           *
* Copyright (c) 2008-2015 GASS team. See Contributors.txt for details.      *
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

#include "RakNetNetworkMasterComponent.h"
#include "Plugins/RakNet4/RakNetNetworkSystem.h"
#include "Plugins/RakNet4/RakNetMasterReplica.h"
#include "Plugins/RakNet4/RakNetNetworkChildComponent.h"
#include "Plugins/RakNet4/RakNetLocationTransferComponent.h"

#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"



#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"

#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/Interface/GASSICameraComponent.h"


namespace GASS
{
	RakNetNetworkMasterComponent::RakNetNetworkMasterComponent() : m_Replica(NULL)
	{

	}

	RakNetNetworkMasterComponent::~RakNetNetworkMasterComponent()
	{
		
	}

	void RakNetNetworkMasterComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("NetworkMasterComponent",new Creator<RakNetNetworkMasterComponent, Component>);
		RegisterProperty<std::vector<std::string> >("Attributes", &RakNetNetworkMasterComponent::GetAttributes, &RakNetNetworkMasterComponent::SetAttributes);
	}

	void RakNetNetworkMasterComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetNetworkMasterComponent::OnSerialize,NetworkSerializeRequest,0));
	
		RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<RakNetNetworkSystem>();
		if(!raknet->IsActive())
			return;

		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(RakNetNetworkMasterComponent::OnNetworkPostUpdate,NetworkPostUpdateEvent,0));
		int id = 0;
		GeneratePartID(GetSceneObject(), id);
		
		if(raknet->IsServer())
		{
			m_Replica = new RakNetMasterReplica(raknet->GetReplicaManager());
			m_Replica->LocalInit(GetSceneObject());
		}
		else
		{
			if(m_Replica) //top object
			{
				m_Replica->SetOwner(GetSceneObject());
			}
			else
			{
				LogManager::getSingleton().stream() << "WARNING: No replica found for RakNetNetworkMasterComponent, have you created a client unit?";
			}

		}
	}

	void RakNetNetworkMasterComponent::GeneratePartID(SceneObjectPtr obj, int &id)
	{
		RakNetNetworkChildComponentPtr comp =  obj->GetFirstComponentByClass<RakNetNetworkChildComponent>();
		if(comp)
			comp->SetPartId(id);
		ComponentContainer::ComponentContainerIterator cc_iter = obj->GetChildren();
		while(cc_iter.hasMoreElements())
		{
			SceneObjectPtr child = STATIC_PTR_CAST<SceneObject>(cc_iter.getNext());
			GeneratePartID(child,++id);
		}
	}

	void RakNetNetworkMasterComponent::OnDelete()
	{
		RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<RakNetNetworkSystem>();
		if(raknet->IsServer())
		{
			delete m_Replica;
			m_Replica = NULL;
		}
	}

	void RakNetNetworkMasterComponent::OnSerialize(NetworkSerializeRequestPtr message)
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

		SystemAddress address;
		address.binaryAddress = message->GetAddress().m_Address;
		address.port  = message->GetAddress().m_Port;
		
		RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<RakNetNetworkSystem>();
		//Signal serialize
		raknet->GetReplicaManager()->SignalSerializeNeeded((Replica*)m_Replica, address, true);
	}

	void RakNetNetworkMasterComponent::Serialize(bool *sendTimestamp, RakNet::BitStream *outBitStream, RakNet::Time lastSendTime, PacketPriority *priority, PacketReliability *reliability, RakNet::Time currentTime, RakNet::SystemAddress systemAddress, unsigned int &flags)
	{
		int num_packs = static_cast<int>(m_SerializePackages.size());
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

	void RakNetNetworkMasterComponent::Deserialize(RakNet::BitStream *inBitStream, RakNet::Time timestamp, RakNet::Time lastDeserializeTime, RakNet::SystemAddress systemAddress )
	{
		int num_packs = 0;
		inBitStream->Read(num_packs);
		//std::cout << "RakNetNetworkMasterComponent::Deserialize packages:"<<  num_packs << std::endl;
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
				GetSceneObject()->PostRequest(NetworkDeserializeRequestPtr(new NetworkDeserializeRequest(NetworkAddress(systemAddress.binaryAddress,systemAddress.port),timestamp,package)));

			}
			//NetworkPackagePtr package;
			/*if(id == TRANSFORMATION_DATA)
			{
				//package = NetworkSerializeMessage::NetworkPackagePtr (new TransformationPackage(TRANSFORMATION_DATA));
				TransformationPackagePtr tp(new TransformationPackage(TRANSFORMATION_DATA));
				int size = tp->GetSize();
				char* data_to_read = new char[size];
				inBitStream->Read(data_to_read,size);
				TransformationPackage* ptp = tp.get();
				*ptp = *(TransformationPackage*)data_to_read;
				//package = tp;
				//GetSceneObject()->SendImmediate(MessagePtr(new NetworkSerializeMessage(tp)));
				GetSceneObject()->PostMessage(MessagePtr(new NetworkSerializeMessage(tp)));
				delete data_to_read;
			}*/
			
			//int size =package->GetSize(); 
			//inBitStream->Read((char*)package.get(),size);
			
			//Post messages
			//GetSceneObject()->PostMessage(MessagePtr(new NetworkSerializeMessage(package)));
		}
	}


	void RakNetNetworkMasterComponent::OnNetworkPostUpdate(NetworkPostUpdateEventPtr message)
	{
		//everything is sent!
		m_SerializePackages.clear();

		if(m_Replica)
			m_Replica->ProcessMessages();

	}
}

