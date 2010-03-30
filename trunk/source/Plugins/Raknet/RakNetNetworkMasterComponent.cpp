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

#include "RakNetNetworkMasterComponent.h"
#include "Plugins/RakNet/RakNetNetworkSystem.h"
#include "Plugins/RakNet/RakNetMasterReplica.h"
#include "Plugins/RakNet/RakNetLocationTransferComponent.h"

#include "Plugins/Game/GameMessages.h"
#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Utils/Log.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
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
	RakNetNetworkMasterComponent::RakNetNetworkMasterComponent() : m_Replica(NULL)
	{

	}

	RakNetNetworkMasterComponent::~RakNetNetworkMasterComponent()
	{

	}

	void RakNetNetworkMasterComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("NetworkMasterComponent",new Creator<RakNetNetworkMasterComponent, IComponent>);
		RegisterVectorProperty<std::string>("Attributes", &RakNetNetworkMasterComponent::GetAttributes, &RakNetNetworkMasterComponent::SetAttributes);
	}

	void RakNetNetworkMasterComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetNetworkMasterComponent::OnUnload,UnloadComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetNetworkMasterComponent::OnLoad,LoadGameComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetNetworkMasterComponent::OnSerialize,NetworkSerializeMessage,0));
	}

	void RakNetNetworkMasterComponent::OnLoad(LoadGameComponentsMessagePtr message)
	{
		RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<RakNetNetworkSystem>();
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
		}
	}

	void RakNetNetworkMasterComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
	//	message->GetSimSceneManager()->GetScenarioScene()->UnregisterForMessage(SCENARIO_RM_ENTER_VEHICLE,TYPED_MESSAGE_FUNC(PlayerInputComponent::OnEnter,AnyMessage));
	}

	void RakNetNetworkMasterComponent::OnSerialize(NetworkSerializeMessagePtr message)
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
		raknet->GetReplicaManager()->SignalSerializeNeeded((Replica*)m_Replica->GetReplica(), UNASSIGNED_SYSTEM_ADDRESS, true);
		//Signal serialize
	}

	void RakNetNetworkMasterComponent::Serialize(bool *sendTimestamp, RakNet::BitStream *outBitStream, RakNetTime lastSendTime, PacketPriority *priority, PacketReliability *reliability, RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags)
	{
		int num_packs = m_SerializePackages.size();
		outBitStream->Write(num_packs);
		for(int i = 0 ; i < m_SerializePackages.size(); i++)
		{
			outBitStream->Write(m_SerializePackages[i]->Id);
			//outBitStream->Write(m_SerializePackages[i]->Size);
			int size = m_SerializePackages[i]->GetSize();
			outBitStream->Write((char*)m_SerializePackages[i].get(),size);
		}
		m_SerializePackages.clear();
	}

	void RakNetNetworkMasterComponent::Deserialize(RakNet::BitStream *inBitStream, RakNetTime timestamp, RakNetTime lastDeserializeTime, SystemAddress systemAddress )
	{
		int num_packs = 0;
		inBitStream->Read(num_packs);
		for(int i = 0 ; i < num_packs; i++)
		{
			//NetworkSerializeMessage::NetworkPackage package;
			//inBitStream->Read(package.Id);
			int id;
			inBitStream->Read(id);
			NetworkSerializeMessage::NetworkPackagePtr package;
			if(id == TRANSFORMATION_DATA)
				package = NetworkSerializeMessage::NetworkPackagePtr (new TransformationPackage(TRANSFORMATION_DATA));
			
			int size =package->GetSize(); 
			//inBitStream->Read(package.Size);
			//package.Data = boost::shared_ptr<char>(new char[package.Size]);
			inBitStream->Read((char*)package.get(),size);
			//m_SerializePackages.push_back(package);
			//Post messages
			GetSceneObject()->PostMessage(MessagePtr(new NetworkSerializeMessage(package)));
		}
	}
}

