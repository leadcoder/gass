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


#include "RakNetBaseReplica.h"


#include "Sim/GASSSceneObjectTemplateManager.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "RakNetNetworkSystem.h"
#include "RakNetInputTransferComponent.h"
#include "RakNetMessageTransferComponent.h"
#include "Sim/Messages/GASSPlatformMessages.h"


namespace GASS
{
	RakNetBaseReplica::RakNetBaseReplica()
	{

	}

	RakNetBaseReplica::~RakNetBaseReplica()
	{

	}

	int RakNetBaseReplica::RemoteMessageWithData(const char *message, const char *data, RakNet::AutoRPC* networkCaller)
	{
		if (m_Owner)
		{
			RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<RakNetNetworkSystem>();
			if (networkCaller == nullptr)
			{
				raknet->GetRPC()->SetRecipientObject(GetNetworkID());
				raknet->GetRPC()->Call("RakNetBaseReplica::RemoteMessageWithData", message, data);
				raknet->GetRPC()->SetRecipientObject(UNASSIGNED_NETWORK_ID);
			}
			else
			{
				RakNetMessageTransferComponentPtr comp = m_Owner->GetFirstComponentByClass<RakNetMessageTransferComponent>();
				comp->Called(std::string(message), std::string(data));
			}
		}
		return 1;
	}

	int RakNetBaseReplica::RemoteMessage(const char *client_address, const char *message, RakNet::AutoRPC* networkCaller)
	{
		RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<RakNetNetworkSystem>();
		if (networkCaller == nullptr)
		{
			raknet->GetRPC()->SetRecipientObject(GetNetworkID());
			raknet->GetRPC()->Call("RakNetBaseReplica::RemoteMessage", client_address, message);
			raknet->GetRPC()->SetRecipientObject(UNASSIGNED_NETWORK_ID);
		}
		else
		{
			std::string my_addres(raknet->GetRakPeer()->GetExternalID(UNASSIGNED_SYSTEM_ADDRESS).ToString());
			if (std::string(client_address) == my_addres)
			{
				m_MessageBuffer.emplace_back(message);
				ProcessMessages();
			}
		}

		return 1;
	}

	void RakNetBaseReplica::ProcessMessages()
	{
		if (m_Owner)
		{
			for (size_t i = 0; i < m_MessageBuffer.size(); i++)
			{
				std::string message = m_MessageBuffer[i];
				if (message == "EnterVehicle")
				{
					EnterVehicleRequestPtr enter_msg(new EnterVehicleRequest());
					m_Owner->PostRequest(enter_msg);
				}
				else if (message == "ExitVehicle")
				{
					ExitVehicleRequestPtr exit_msg(new ExitVehicleRequest());
					m_Owner->PostRequest(exit_msg);
				}
			}
			m_MessageBuffer.clear();
		}
	}

	int RakNetBaseReplica::RemoteInput(SystemAddress input_source, int controller, float value, RakNet::AutoRPC* networkCaller)
	{
		if (m_Owner)
		{
			RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<RakNetNetworkSystem>();
			if (networkCaller == nullptr)
			{
				raknet->GetRPC()->SetRecipientObject(GetNetworkID());
				raknet->GetRPC()->Call("RakNetBaseReplica::RemoteInput", input_source, controller, value);
				raknet->GetRPC()->SetRecipientObject(UNASSIGNED_NETWORK_ID);
				if (raknet->GetDebug())
					std::cout << "RemoteInput called" << std::endl;
			}
			else
			{
				//check that we differ from input source, we dont want to duplicate our own input 
				if (input_source != raknet->GetRakPeer()->GetInternalID())
				{
					RakNetInputTransferComponentPtr input_comp = m_Owner->GetFirstComponentByClass<RakNetInputTransferComponent>();
					if (input_comp)
					{
						if (raknet->GetDebug())
							std::cout << "RemoteInput received:" << input_source.ToString() << std::endl;
						input_comp->ReceivedInput(controller, value);
					}
				}

				//relay to all clients if server
				if (raknet->IsServer())
				{
					raknet->GetRPC()->SetRecipientObject(GetNetworkID());
					raknet->GetRPC()->Call("RakNetBaseReplica::RemoteInput", input_source, controller, value);
					raknet->GetRPC()->SetRecipientObject(UNASSIGNED_NETWORK_ID);
				}
			}
		}
		return 1;
	}
}
