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

#include "Plugins/RakNet/RakNetNetworkSystem.h"
//#include "Plugins/RakNet/RakNetBase.h"
#include "RakNetMessageTransferComponent.h"

#include "RakNetNetworkMasterComponent.h"
#include "RakNetNetworkChildComponent.h"
#include "RakNetMasterReplica.h"
#include "RakNetChildReplica.h"



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

#include "GetTime.h"
#include "RakPeerInterface.h"


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
		ComponentFactory::GetPtr()->Register("MessageTransferComponent",new Creator<RakNetMessageTransferComponent, IComponent>);
		//RegisterProperty<std::string>("ControlSetting", &RakNetMessageTransferComponent::GetControlSetting, &RakNetMessageTransferComponent::SetControlSetting);
	}

	void RakNetMessageTransferComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetMessageTransferComponent::OnUnload,UnloadComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetMessageTransferComponent::OnLoad,LoadNetworkComponentsMessage,1));
	}

	void RakNetMessageTransferComponent::OnLoad(LoadNetworkComponentsMessagePtr message)
	{
		RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<RakNetNetworkSystem>();
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
		RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<RakNetNetworkSystem>();
		if(raknet && raknet->IsActive())
			return !raknet->IsServer();
		return false;
	}

	void RakNetMessageTransferComponent::OnOutOfArmor(OutOfArmorMessagePtr message)
	{
		if(message->GetSenderID() == 8888)
			return;
		//SystemAddress address = raknet->GetRakPeer()->GetInternalID();
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
			RakNetNetworkChildComponentPtr comp = GetSceneObject()->GetFirstComponentByClass<RakNetNetworkChildComponent>();
			if(comp)
			{
				comp->GetReplica()->RemoteMessageWithData(message.c_str(),data.c_str(),0);
			}
		}

	}

	void RakNetMessageTransferComponent::Called(const std::string &message, const std::string &data)
	{
		std::cout << "called " << message << " data:" << data << "\n";
		if(std::string(message) == "OutOfArmorMessage")
		{
			MessagePtr message(new OutOfArmorMessage());
			GetSceneObject()->PostMessage(message);
		}
	}

	void RakNetMessageTransferComponent::OnUnload(UnloadComponentsMessagePtr message)
	{

	}

	
}

