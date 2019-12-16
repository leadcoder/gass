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


#include "RakNetInputTransferComponent.h"
#include "Plugins/RakNet/RakNetNetworkSystem.h"
#include "RakNetNetworkMasterComponent.h"
#include "RakNetNetworkChildComponent.h"
#include "RakNetMasterReplica.h"
#include "RakNetChildReplica.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"



namespace GASS
{
	RakNetInputTransferComponent::RakNetInputTransferComponent() 
	{

	}

	RakNetInputTransferComponent::~RakNetInputTransferComponent()
	{

	}

	void RakNetInputTransferComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register<RakNetInputTransferComponent>("InputTransferComponent");
		GASS::PackageFactory::GetPtr()->Register(INPUT_DATA,new GASS::Creator<InputPackage, NetworkPackage>);

		RegisterGetSet("ControlSetting", &RakNetInputTransferComponent::GetControlSetting, &RakNetInputTransferComponent::SetControlSetting);
	}

	void RakNetInputTransferComponent::OnInitialize()
	{
		RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<RakNetNetworkSystem>();
		if(!raknet->IsActive())
			return;

		//m_ControlSetting = SimEngine::Get().GetControlSettingsManager()->GetControlSetting(m_ControlSettingName);
		//if(m_ControlSetting == NULL)
		//	GASS_LOG(LWARNING) << "Could not find control settings:" << m_ControlSettingName << " in RakNetInputTransferComponent";

		if(!raknet->IsServer())
		{
			GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetInputTransferComponent::OnInput,InputRelayEvent,0));

			//test input chain
			GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetInputTransferComponent::OnDeserialize,NetworkDeserializeRequest,0));
		}
		else
		{
			GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetInputTransferComponent::OnDeserialize,NetworkDeserializeRequest,0));
			GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetInputTransferComponent::OnClientRemoteMessage,ClientRemoteMessage,0));
			GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetInputTransferComponent::OnInput,InputRelayEvent,0));
		}
	}

	void RakNetInputTransferComponent::OnInput(InputRelayEventPtr message)
	{
		if(message->GetSenderID() == 8888)
			return;
		RakNetTime time_stamp = RakNet::GetTime();
		std::string controller = message->GetController();

		float value = message->GetValue();
		ControlSettingsSystemPtr css = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<IControlSettingsSystem>();
		int controller_index = css->GetIndexFromName(m_ControlSettingName,controller);
		if(controller_index >= 0)
		{
			//Check history
			bool new_value = false;
			
			//int controller_index = m_ControlSetting->m_NameToIndex[controller];
			if(m_InputHistory.find(controller_index) == m_InputHistory.end())
			{
				new_value = true;
			}
			else
			{
				float last_value = m_InputHistory[controller_index];
				if(value != last_value) //add dead span epsilon here?
				{
					new_value = true;
				}
				//else
				//	std::cout <<"Same value:" << value << " last:" << last_value << "\n";
			}
			m_InputHistory[controller_index] = value;
			

			RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<RakNetNetworkSystem>();

			if(message->GetControllerType() == CT_AXIS && new_value)
			{
				SystemAddress address = raknet->GetRakPeer()->GetInternalID();
				GASS_SHARED_PTR<InputPackage> package(new InputPackage(INPUT_DATA,time_stamp,address.binaryAddress,controller_index,value));
				
				
				NetworkSerializeRequestPtr serialize_message(new NetworkSerializeRequest(NetworkAddress(address.binaryAddress,address.port),0,package));
				GetSceneObject()->SendImmediateRequest(serialize_message);
			}
			else if(message->GetControllerType() == CT_TRIGGER)
			{
				SystemAddress address = raknet->GetRakPeer()->GetInternalID();
				RakNetNetworkMasterComponentPtr master_comp = GetSceneObject()->GetFirstComponentByClass<RakNetNetworkMasterComponent>();
				if(master_comp)
				{
					master_comp->GetReplica()->RemoteInput(address ,controller_index, message->GetValue(),0);
				}
				else
				{
					RakNetNetworkChildComponentPtr child_comp = GetSceneObject()->GetFirstComponentByClass<RakNetNetworkChildComponent>();
					if(child_comp)
					{
						child_comp->GetReplica()->RemoteInput(address,controller_index,message->GetValue(),0);
					}
				}
			}
		}
		else
		{
			std::cout << "failed to find controller:" << controller << std::endl;
		}
	}

	void RakNetInputTransferComponent::OnDelete()
	{

	}

	

	void RakNetInputTransferComponent::ReceivedInput(int controller, float value)
	{
		int id = 8888;
		ControlSettingsSystemPtr css = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<IControlSettingsSystem>();
		std::string c_name = css->GetNameFromIndex(m_ControlSettingName,controller);
		//std::string c_name = m_ControlSetting->m_IndexToName[controller];
		InputRelayEventPtr message(new InputRelayEvent(m_ControlSettingName,c_name,value,CT_TRIGGER,id));
		GetSceneObject()->PostEvent(message);
	}

	void RakNetInputTransferComponent::OnDeserialize(NetworkDeserializeRequestPtr message)
	{
		if(message->GetPackage()->Id == INPUT_DATA)
		{
			int id = 8888;
			NetworkPackagePtr package = message->GetPackage();
			InputPackagePtr input_package = GASS_DYNAMIC_PTR_CAST<InputPackage>(package);

			RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<RakNetNetworkSystem>();
			SystemAddress address = raknet->GetRakPeer()->GetInternalID();
			if(input_package->Generator == address.binaryAddress) //generate by my self!!!
				return;
			

			ControlSettingsSystemPtr css = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<IControlSettingsSystem>();
			//std::string controller = m_ControlSetting->m_IndexToName[input_package->Index];
			std::string controller = css->GetNameFromIndex(m_ControlSettingName,input_package->Index);
			InputRelayEventPtr ctrl_message(new InputRelayEvent(m_ControlSettingName,controller,input_package->Value,CT_AXIS,id));
			GetSceneObject()->PostEvent(ctrl_message);

			/*if(controller == "Fire")
			{
				std::cout << "Got Fire from:" << message->GetAddress().m_Address <<  "Port:" << message->GetAddress().m_Port << " value:" << input_package->Value << std::endl;
			}*/

			//if server, relay this to all clients except sender
			if(raknet->GetDebug())
				std::cout << "Got " << controller << " Input from:" <<   message->GetAddress().m_Address <<  " On port:" << message->GetAddress().m_Port << " With value value:" << input_package->Value << std::endl;
			
			if(raknet->IsServer() && raknet->GetRelayInputOnServer())
			{
				NetworkAddress message_address  = message->GetAddress();
				NetworkSerializeRequestPtr serialize_message(new NetworkSerializeRequest(message_address,0,package));
				GetSceneObject()->PostRequest(serialize_message);
			}
			//std::cout << "got input from client" << std::endl;
		}
	}

	void RakNetInputTransferComponent::OnClientRemoteMessage(ClientRemoteMessagePtr message)
	{
		RakNetNetworkMasterComponentPtr comp = GetSceneObject()->GetFirstComponentByClass<RakNetNetworkMasterComponent>();
		if(comp)
		{
			comp->GetReplica()->RemoteMessage(message->GetClient().c_str(),message->GetMessage().c_str(),0);
		}
		else
		{
			RakNetNetworkChildComponentPtr child_comp = GetSceneObject()->GetFirstComponentByClass<RakNetNetworkChildComponent>();
			if(child_comp)
			{
				child_comp->GetReplica()->RemoteMessage(message->GetClient().c_str(),message->GetMessage().c_str(),0);
			}
		}
	}
}