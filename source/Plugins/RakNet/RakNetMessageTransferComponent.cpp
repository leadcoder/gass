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

#include "RakNetMessageTransferComponent.h"
#include "Plugins/RakNet/RakNetNetworkSystem.h"
#include "RakNetNetworkMasterComponent.h"
#include "RakNetNetworkChildComponent.h"
#include "RakNetMasterReplica.h"
#include "RakNetChildReplica.h"
#include "Sim/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"


namespace GASS
{
	RakNetMessageTransferComponent::RakNetMessageTransferComponent() 
	{

	}

	RakNetMessageTransferComponent::~RakNetMessageTransferComponent()
	{

	}

	void RakNetMessageTransferComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register<RakNetMessageTransferComponent>("MessageTransferComponent");
	}

	void RakNetMessageTransferComponent::OnInitialize()
	{
		RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<RakNetNetworkSystem>();
		if(!raknet->IsActive())
			return;

		if(raknet->IsServer())
		{
			GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetMessageTransferComponent::OnOutOfArmor,OutOfArmorMessage,0));
		}
		else
		{
			
		}
	}

	bool RakNetMessageTransferComponent::IsRemote() const
	{
		RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<RakNetNetworkSystem>();
		if(raknet && raknet->IsActive())
			return !raknet->IsServer();
		return false;
	}

	void RakNetMessageTransferComponent::OnOutOfArmor(OutOfArmorMessagePtr message)
	{
		if(message->GetSenderID() == 8888)
			return;
		Call("OutOfArmorMessage","");
		
	}

	void RakNetMessageTransferComponent::Call(const std::string &message, const std::string &data)
	{
		std::cout << "call " << message << " data:" << data << "\n";
		RakNetNetworkMasterComponentPtr comp = GetSceneObject()->GetFirstComponentByClass<RakNetNetworkMasterComponent>();
		if(comp)
		{
			comp->GetReplica()->RemoteMessageWithData(message.c_str(),data.c_str(),0);
		}
		else
		{
			RakNetNetworkChildComponentPtr child_comp = GetSceneObject()->GetFirstComponentByClass<RakNetNetworkChildComponent>();
			if(child_comp)
			{
				child_comp->GetReplica()->RemoteMessageWithData(message.c_str(),data.c_str(),0);
			}
		}

	}

	void RakNetMessageTransferComponent::Called(const std::string &message, const std::string &data)
	{
		std::cout << "called " << message << " data:" << data << "\n";
		if(std::string(message) == "OutOfArmorMessage")
		{
			OutOfArmorMessagePtr armor_message(new OutOfArmorMessage());
			GetSceneObject()->PostEvent(armor_message);
		}
	}

	void RakNetMessageTransferComponent::OnDelete()
	{

	}

	
}

