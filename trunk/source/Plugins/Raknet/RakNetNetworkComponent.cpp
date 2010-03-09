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

#include "RakNetNetworkComponent.h"
#include "Plugins/RakNet/RakNetNetworkSystem.h"
#include "Plugins/RakNet/RakNetBase.h"

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
	RakNetNetworkComponent::RakNetNetworkComponent() : m_Replica(NULL)
	{

	}

	RakNetNetworkComponent::~RakNetNetworkComponent()
	{

	}

	void RakNetNetworkComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("NetworkComponent",new Creator<RakNetNetworkComponent, IComponent>);
		RegisterVectorProperty<std::string>("Attributes", &RakNetNetworkComponent::GetAttributes, &RakNetNetworkComponent::SetAttributes);
	}

	void RakNetNetworkComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetNetworkComponent::OnUnload,UnloadComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetNetworkComponent::OnLoad,LoadGameComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetNetworkComponent::OnSerialize,NetworkSerializeMessage,0));
	}

	void RakNetNetworkComponent::OnLoad(LoadGameComponentsMessagePtr message)
	{
		RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<RakNetNetworkSystem>();
		if(raknet->IsServer())
		{
			m_Replica = new RakNetBase(raknet->GetReplicaManager());
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
				//Get part of id
				SceneObjectPtr root = GetSceneObject()->GetObjectUnderRoot();
				//while(root->GetParentSceneObject())
				//	root = root->GetParentSceneObject();
				RakNetNetworkComponentPtr top_comp = root->GetFirstComponent<RakNetNetworkComponent>();
				NetworkID part_of_id = top_comp->GetReplica()->GetPartOfId();
				int part_id = GetPartId();
				m_Replica = raknet->FindReplica(part_of_id,part_id);
				if(!m_Replica) //not available...jet
				{
					SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(RakNetNetworkComponent::OnNewReplica,ReplicaCreatedMessage,0));
//					GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetNetworkComponent::OnReplicaCreated,ReplicaCreatedMessage,0));
				}
				else
					m_Replica->SetOwner(GetSceneObject());
			}
		}
	}

	void RakNetNetworkComponent::OnNewReplica(ReplicaCreatedMessagePtr message)
	{
		RakNetBase* replica = message->GetReplica();
		SceneObjectPtr root = GetSceneObject()->GetObjectUnderRoot();
		RakNetNetworkComponentPtr top_comp = root->GetFirstComponent<RakNetNetworkComponent>();
		NetworkID part_of_id = top_comp->GetReplica()->GetPartOfId();
		int part_id = GetPartId();
		if(replica->GetPartId() == part_id && replica->GetPartOfId() == part_of_id)
		{
			m_Replica = replica;
			m_Replica->SetOwner(GetSceneObject());
			SimEngine::Get().GetSimSystemManager()->UnregisterForMessage(UNREG_TMESS(RakNetNetworkComponent::OnNewReplica,ReplicaCreatedMessage));
		}
	}


	void RakNetNetworkComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
	//	message->GetSimSceneManager()->GetScenarioScene()->UnregisterForMessage(SCENARIO_RM_ENTER_VEHICLE,TYPED_MESSAGE_FUNC(PlayerInputComponent::OnEnter,AnyMessage));
	}

	void RakNetNetworkComponent::OnSerialize(NetworkSerializeMessagePtr message)
	{
		bool found = false;
		for(int i = 0 ; i < m_SerializePackages.size(); i++)
		{
			if(m_SerializePackages[i].Id = message->GetPackage().Id)
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

	void RakNetNetworkComponent::Serialize(bool *sendTimestamp, RakNet::BitStream *outBitStream, RakNetTime lastSendTime, PacketPriority *priority, PacketReliability *reliability, RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags)
	{
		int num_packs = m_SerializePackages.size();
		outBitStream->Write(num_packs);
		for(int i = 0 ; i < m_SerializePackages.size(); i++)
		{
			outBitStream->Write(m_SerializePackages[i].Id);
			outBitStream->Write(m_SerializePackages[i].Size);
			outBitStream->Write(m_SerializePackages[i].Data.get(),m_SerializePackages[i].Size);
		}
		m_SerializePackages.clear();
	}

	void RakNetNetworkComponent::Deserialize(RakNet::BitStream *inBitStream, RakNetTime timestamp, RakNetTime lastDeserializeTime, SystemAddress systemAddress )
	{
		int num_packs = 0;
		inBitStream->Read(num_packs);
		for(int i = 0 ; i < num_packs; i++)
		{
			NetworkSerializeMessage::NetworkPackage package;
			
			inBitStream->Read(package.Id);
			inBitStream->Read(package.Size);
			package.Data = boost::shared_ptr<char>(new char[package.Size]);
			inBitStream->Read(package.Data.get(),package.Size);
			//m_SerializePackages.push_back(package);
			//Post messages
			GetSceneObject()->PostMessage(MessagePtr(new NetworkSerializeMessage(package)));
		}
	}
}

