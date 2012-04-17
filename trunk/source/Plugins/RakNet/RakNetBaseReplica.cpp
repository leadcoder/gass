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

#include "Core/ComponentSystem/GASSBaseComponentContainerTemplateManager.h"
#include "Core/ComponentSystem/GASSComponentContainerFactory.h"

#include "Sim/Scenario/Scene/GASSSceneObject.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "Sim/Scenario/GASSScenario.h"
#include "Sim/Scenario/GASSScenario.h"
#include "Sim/Scenario/Scene/GASSSceneObjectManager.h"

#include "RakNetBaseReplica.h"
#include "RakNetNetworkMasterComponent.h"
#include "RakNetNetworkSystem.h"
#include "RakNetInputTransferComponent.h"
#include "RakNetMessageTransferComponent.h"
#include "Plugins/Game/GameMessages.h"


namespace GASS
{
	RakNetBaseReplica::RakNetBaseReplica() 
	{
		
	}

	RakNetBaseReplica::~RakNetBaseReplica()
	{
		
	}
	
/*	int RakNetBaseReplica::EnterVehicle(const char *client_address, RakNet::AutoRPC* networkCaller) 
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
	}*/

	int RakNetBaseReplica::RemoteMessageWithData(const char *message, const char *data, RakNet::AutoRPC* networkCaller)
	{
		if(m_Owner)
		{
			RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<RakNetNetworkSystem>();
			if (networkCaller==0)
			{
				raknet->GetRPC()->SetRecipientObject(GetNetworkID());
				raknet->GetRPC()->Call("RakNetBaseReplica::RemoteMessageWithData", message,data);
				raknet->GetRPC()->SetRecipientObject(UNASSIGNED_NETWORK_ID);
			}
			else
			{
				RakNetMessageTransferComponentPtr comp = m_Owner->GetFirstComponentByClass<RakNetMessageTransferComponent>();
				comp->Called(std::string(message),std::string(data));
				/*if(std::string(message) == "OutOfArmor")
				{
					MessagePtr message(new OutOfArmorMessage());
					m_Owner->PostMessage(message);
				}*/
				//std::cout << "EnterObject called from client, target address:" << std::string(str) << " client address:" <<  std::string(raknet->GetRakPeer()->GetInternalID().ToString()) <<std::endl;
			}
		}
		return 1;
	}

	int RakNetBaseReplica::RemoteMessage(const char *client_address, const char *message , RakNet::AutoRPC* networkCaller) 
	{
		if(m_Owner)
		{
			RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<RakNetNetworkSystem>();
			if (networkCaller==0)
			{
				raknet->GetRPC()->SetRecipientObject(GetNetworkID());
				raknet->GetRPC()->Call("RakNetBaseReplica::RemoteMessage", client_address,message);
				raknet->GetRPC()->SetRecipientObject(UNASSIGNED_NETWORK_ID);
				//std::cout << "EnterObject called from server" <<std::endl;
			}
			else
			{
				if(std::string(client_address)  == std::string(raknet->GetRakPeer()->GetInternalID().ToString()))
				{
					
					if(std::string(message) == "EnterVehicle")
					{
						MessagePtr enter_msg(new EnterVehicleMessage());
						m_Owner->PostMessage(enter_msg);
					}
					else if(std::string(message) == "ExitVehicle")
					{
						MessagePtr exit_msg(new ExitVehicleMessage());
						m_Owner->PostMessage(exit_msg);
					}
				}
				//std::cout << "EnterObject called from client, target address:" << std::string(str) << " client address:" <<  std::string(raknet->GetRakPeer()->GetInternalID().ToString()) <<std::endl;
			}
		}
		return 1;
	}


	int RakNetBaseReplica::RemoteInput(SystemAddress input_source, int controller, float value, RakNet::AutoRPC* networkCaller) 
	{
		if(m_Owner)
		{
			RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<RakNetNetworkSystem>();
			if (networkCaller==0)
			{
				raknet->GetRPC()->SetRecipientObject(GetNetworkID());
				raknet->GetRPC()->Call("RakNetBaseReplica::RemoteInput", input_source ,controller, value);
				raknet->GetRPC()->SetRecipientObject(UNASSIGNED_NETWORK_ID);
				if(raknet->GetDebug())
					std::cout << "RemoteInput called" <<std::endl;
			}
			else
			{
				//check that we differ from input source, we dont want to duplicate our own input 
				if(input_source != raknet->GetRakPeer()->GetInternalID())
				{
					RakNetInputTransferComponentPtr input_comp =  m_Owner->GetFirstComponentByClass<RakNetInputTransferComponent>();
					if(input_comp)
					{
						if(raknet->GetDebug())
							std::cout << "RemoteInput received:" << input_source.ToString() <<std::endl;
						//int id = 8888;
						input_comp->ReceivedInput(controller,value);
					}
					//MessagePtr message(new ControllerMessage(controller,value,id));
					//m_Owner->PostMessage(message);
					
				}

				//relay to all clients if server
				if(raknet->IsServer())
				{
					raknet->GetRPC()->SetRecipientObject(GetNetworkID());
					raknet->GetRPC()->Call("RakNetBaseReplica::RemoteInput", input_source, controller, value);
					raknet->GetRPC()->SetRecipientObject(UNASSIGNED_NETWORK_ID);
					//std::cout << "RemoteInput called:" << input_source.ToString()  <<std::endl;
				}

				//if(std::string(client_address) == std::string(raknet->GetRakPeer()->GetInternalID().ToString()))
				/*{
					
					if(std::string(message) == "EnterVehicle")
					{
						MessagePtr enter_msg(new EnterVehicleMessage());
						m_Owner->PostMessage(enter_msg);
					}
					else if(std::string(message) == "ExitVehicle")
					{
						MessagePtr exit_msg(new ExitVehicleMessage());
						m_Owner->PostMessage(exit_msg);
					}
				}*/
				//std::cout << "EnterObject called from client, target address:" << std::string(str) << " client address:" <<  std::string(raknet->GetRakPeer()->GetInternalID().ToString()) <<std::endl;
			}
		}
		return 1;
	}

}
