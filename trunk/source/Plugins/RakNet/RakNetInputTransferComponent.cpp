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
#include "RakNetInputTransferComponent.h"

#include "RakNetNetworkMasterComponent.h"
#include "RakNetNetworkChildComponent.h"
#include "RakNetMasterReplica.h"
#include "RakNetChildReplica.h"



#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/Scenario/GASSScenario.h"
#include "Sim/Scenario/Scene/GASSSceneObject.h"
#include "Sim/Scenario/Scene/GASSSceneObjectManager.h"

#include "Sim/Systems/Resource/GASSIResourceSystem.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "Sim/Scheduling/GASSIRuntimeController.h"
#include "Sim/Systems/Input/GASSControlSettingsManager.h"
#include "Sim/Systems/Input/GASSControlSetting.h"
#include "Sim/Components/Graphics/GASSICameraComponent.h"

#include "GetTime.h"
#include "RakPeerInterface.h"


namespace GASS
{
	RakNetInputTransferComponent::RakNetInputTransferComponent() : m_ControlSetting(NULL)
	{

	}

	RakNetInputTransferComponent::~RakNetInputTransferComponent()
	{

	}

	void RakNetInputTransferComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("InputTransferComponent",new Creator<RakNetInputTransferComponent, IComponent>);
		GASS::PackageFactory::GetPtr()->Register(INPUT_DATA,new GASS::Creator<InputPackage, NetworkPackage>);

		RegisterProperty<std::string>("ControlSetting", &RakNetInputTransferComponent::GetControlSetting, &RakNetInputTransferComponent::SetControlSetting);
	}

	void RakNetInputTransferComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetInputTransferComponent::OnUnload,UnloadComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetInputTransferComponent::OnLoad,LoadNetworkComponentsMessage,1));

	}

	void RakNetInputTransferComponent::OnLoad(LoadNetworkComponentsMessagePtr message)
	{
		RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<RakNetNetworkSystem>();

		m_ControlSetting = SimEngine::Get().GetControlSettingsManager()->GetControlSetting(m_ControlSettingName);
		if(m_ControlSetting == NULL)
			LogManager::getSingleton().stream() << "WARNING: Could not find control settings:" << m_ControlSettingName << " in RakNetInputTransferComponent";

		if(!raknet->IsServer())
		{
			GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetInputTransferComponent::OnInput,ControllerMessage,0));

			//test input chain
			GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetInputTransferComponent::OnDeserialize,NetworkDeserializeMessage,0));
		}
		else
		{
			GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetInputTransferComponent::OnDeserialize,NetworkDeserializeMessage,0));
			GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetInputTransferComponent::OnClientRemoteMessage,ClientRemoteMessage,0));
			GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetInputTransferComponent::OnInput,ControllerMessage,0));
		}
	}

	void RakNetInputTransferComponent::OnInput(ControllerMessagePtr message)
	{
		if(message->GetSenderID() == 8888)
			return;
		RakNetTime time_stamp = RakNet::GetTime();
		std::string controller = message->GetController();

		float value = message->GetValue();

		if(m_ControlSetting->GetController(controller))
		{
			//Check history
			bool new_value = false;
			int controller_index = m_ControlSetting->m_NameToIndex[controller];
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
			

			RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<RakNetNetworkSystem>();

			if(message->GetControllerType() == CT_AXIS && new_value)
			{
				SystemAddress address = raknet->GetRakPeer()->GetInternalID();
				boost::shared_ptr<InputPackage> package(new InputPackage(INPUT_DATA,time_stamp,address.binaryAddress,controller_index,value));
				
				
				MessagePtr serialize_message(new NetworkSerializeMessage(NetworkAddress(address.binaryAddress,address.port),0,package));
				GetSceneObject()->SendImmediate(serialize_message);
			}
			else if(message->GetControllerType() == CT_TRIGGER)
			{
				SystemAddress address = raknet->GetRakPeer()->GetInternalID();
				RakNetNetworkMasterComponentPtr comp = GetSceneObject()->GetFirstComponentByClass<RakNetNetworkMasterComponent>();
				if(comp)
				{
					comp->GetReplica()->RemoteInput(address ,controller_index, message->GetValue(),0);
				}
				else
				{
					RakNetNetworkChildComponentPtr comp = GetSceneObject()->GetFirstComponentByClass<RakNetNetworkChildComponent>();
					if(comp)
					{
						comp->GetReplica()->RemoteInput(address,controller_index,message->GetValue(),0);
					}
				}
			}
		}
		else
		{
			std::cout << "failed to find controller:" << controller << std::endl;
		}
	}

	void RakNetInputTransferComponent::OnUnload(UnloadComponentsMessagePtr message)
	{

	}

	

	void RakNetInputTransferComponent::ReceivedInput(int controller, float value)
	{
		int id = 8888;
		std::string c_name = m_ControlSetting->m_IndexToName[controller];
		MessagePtr message(new ControllerMessage(c_name,value,CT_TRIGGER,id));
		GetSceneObject()->PostMessage(message);
	}

	void RakNetInputTransferComponent::OnDeserialize(NetworkDeserializeMessagePtr message)
	{
		if(message->GetPackage()->Id == INPUT_DATA)
		{
			int id = 8888;
			NetworkPackagePtr package = message->GetPackage();
			InputPackagePtr input_package = boost::shared_dynamic_cast<InputPackage>(package);

			RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<RakNetNetworkSystem>();
			SystemAddress address = raknet->GetRakPeer()->GetInternalID();
			if(input_package->Generator == address.binaryAddress) //generate by my self!!!
				return;
			

			std::string controller = m_ControlSetting->m_IndexToName[input_package->Index];
			MessagePtr ctrl_message(new ControllerMessage(controller,input_package->Value,CT_AXIS,id));
			GetSceneObject()->PostMessage(ctrl_message);

			/*if(controller == "Fire")
			{
				std::cout << "Got Fire from:" << message->GetAddress().m_Address <<  "Port:" << message->GetAddress().m_Port << " value:" << input_package->Value << std::endl;
			}*/

			//if server, relay this to all clients except sender
			

			
			if(raknet->GetDebug())
				std::cout << "Got " << controller << " Input from:" <<   message->GetAddress().m_Address <<  " On port:" << message->GetAddress().m_Port << " With value value:" << input_package->Value << std::endl;
			
			if(raknet->IsServer() && raknet->GetRelayInputOnServer())
			{
				NetworkAddress address  = message->GetAddress();
				MessagePtr serialize_message(new NetworkSerializeMessage(address ,0,package));
				GetSceneObject()->PostMessage(serialize_message);
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
			RakNetNetworkChildComponentPtr comp = GetSceneObject()->GetFirstComponentByClass<RakNetNetworkChildComponent>();
			if(comp)
			{
				comp->GetReplica()->RemoteMessage(message->GetClient().c_str(),message->GetMessage().c_str(),0);
			}
		}
	}
}

