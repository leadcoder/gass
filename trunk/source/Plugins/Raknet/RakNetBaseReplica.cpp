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

#include "Core/ComponentSystem/BaseComponentContainerTemplateManager.h"
#include "Core/ComponentSystem/ComponentContainerFactory.h"

#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/SimEngine.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Scenario/Scenario.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"

#include "RakNetBaseReplica.h"
#include "RakNetNetworkMasterComponent.h"
#include "RakNetNetworkSystem.h"
#include "Plugins/Game/GameMessages.h"


namespace GASS
{
	RakNetBaseReplica::RakNetBaseReplica() 
	{
		
	}

	RakNetBaseReplica::~RakNetBaseReplica()
	{
		
	}
	
	int RakNetBaseReplica::EnterVehicle(const char *client_address, RakNet::AutoRPC* networkCaller) 
	{
		if(m_Owner)
		{
			RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<RakNetNetworkSystem>();
			if (networkCaller==0)
			{
				raknet->GetRPC()->SetRecipientObject(GetNetworkID());
				raknet->GetRPC()->Call("RakNetBaseReplica::EnterVehicle", client_address);
				raknet->GetRPC()->SetRecipientObject(UNASSIGNED_NETWORK_ID);
				//std::cout << "EnterObject called from server" <<std::endl;
			}
			else
			{
				if(std::string(client_address)  == std::string(raknet->GetRakPeer()->GetInternalID().ToString()))
				{
					MessagePtr enter_msg(new EnterVehicleMessage());
					m_Owner->PostMessage(enter_msg);
				}
				//std::cout << "EnterObject called from client, target address:" << std::string(str) << " client address:" <<  std::string(raknet->GetRakPeer()->GetInternalID().ToString()) <<std::endl;
			}
		}
		return 1;
	}

	int RakNetBaseReplica::ExitVehicle(const char *client_address, RakNet::AutoRPC* networkCaller) 
	{
		if(m_Owner)
		{
			RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<RakNetNetworkSystem>();
			if (networkCaller==0)
			{
				raknet->GetRPC()->SetRecipientObject(GetNetworkID());
				raknet->GetRPC()->Call("RakNetBaseReplica::ExitVehicle", client_address);
				raknet->GetRPC()->SetRecipientObject(UNASSIGNED_NETWORK_ID);
				//std::cout << "EnterObject called from server" <<std::endl;
			}
			else
			{
				if(std::string(client_address)  == std::string(raknet->GetRakPeer()->GetInternalID().ToString()))
				{
					MessagePtr exit_msg(new ExitVehicleMessage());
					m_Owner->PostMessage(exit_msg);
				}
				//std::cout << "EnterObject called from client, target address:" << std::string(str) << " client address:" <<  std::string(raknet->GetRakPeer()->GetInternalID().ToString()) <<std::endl;
			}
		}
		return 1;
	}
}
