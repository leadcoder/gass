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

#include "GetTime.h"
#include "rakpeerinterface.h"


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
			Log::Error("could not find control settings: %s in RakNetInputTransferComponent",m_ControlSettingName.c_str());
		
		if(!raknet->IsServer())
		{
			GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetInputTransferComponent::OnInput,ControllerMessage,0));
		}
		else
		{
			GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetInputTransferComponent::OnDeserialize,NetworkDeserializeMessage,0));
			GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetInputTransferComponent::OnClientEnterVehicle,ClientEnterVehicleMessage,0));
		}
//		ARPC_REGISTER_CPP_FUNCTION2(raknet->GetRPC(), "RakNetInputTransferComponent::EnterObject", int, RakNetInputTransferComponent, EnterObject, const char *str, RakNet::AutoRPC* networkCaller);
	}

	void RakNetInputTransferComponent::OnInput(ControllerMessagePtr message)
	{
		RakNetTime time_stamp = RakNet::GetTime();
		std::string controller = message->GetController();
		
		int index = m_ControlSetting->m_NameToIndex[controller];
		float value = message->GetValue();
		boost::shared_ptr<InputPackage> package(new InputPackage(INPUT_DATA,time_stamp,index,value));
		MessagePtr serialize_message(new NetworkSerializeMessage(0,package));
		GetSceneObject()->SendImmediate(serialize_message);
		//std::cout << "send input to server" << std::endl;
	}

	void RakNetInputTransferComponent::OnUnload(UnloadComponentsMessagePtr message)
	{

	}

	void RakNetInputTransferComponent::Update(double delta)
	{

	}

	void RakNetInputTransferComponent::OnDeserialize(NetworkDeserializeMessagePtr message)
	{
		if(message->GetPackage()->Id == INPUT_DATA)
		{
			NetworkPackagePtr package = message->GetPackage();
			InputPackagePtr input_package = boost::shared_dynamic_cast<InputPackage>(package);
			
			std::string controller = m_ControlSetting->m_IndexToName[input_package->Index];
			MessagePtr message(new ControllerMessage(controller,input_package->Value));
			GetSceneObject()->PostMessage(message);
			//std::cout << "got input from client" << std::endl;
		}
	}

	TaskGroup RakNetInputTransferComponent::GetTaskGroup() const 
	{
		return NETWORK_TASK_GROUP;
	}

	/*int RakNetInputTransferComponent::EnterObject(const char *str, RakNet::AutoRPC* networkCaller) 
	{
		RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<RakNetNetworkSystem>();
		if (networkCaller==0)
		{
			RakNetNetworkMasterComponentPtr comp = GetSceneObject()->GetFirstComponent<RakNetNetworkMasterComponent>();
			if(comp)
			{
				raknet->GetRPC()->SetRecipientObject(comp->GetReplica()->GetNetworkID());
			}
			else
			{
				RakNetNetworkChildComponentPtr comp = GetSceneObject()->GetFirstComponent<RakNetNetworkChildComponent>();
				if(comp)
				{
					raknet->GetRPC()->SetRecipientObject(comp->GetReplica()->GetNetworkID());
				}
			}
			raknet->GetRPC()->Call("RakNetInputTransferComponent::EnterObject", str);
			raknet->GetRPC()->SetRecipientObject(UNASSIGNED_NETWORK_ID);
			std::cout << "EnterObject called from server" <<std::endl;
		
		}
		else
		{
			MessagePtr enter_msg(new EnterVehicleMessage());
			GetSceneObject()->PostMessage(enter_msg);
			std::cout << "EnterObject called from client, target address:" << std::string(str) << " client address:" <<  std::string(raknet->GetRakPeer()->GetInternalID().ToString()) <<std::endl;
		}
		//printf("object member a=%i memberVariable=%i sender=%s\n", a, memberVariable, networkCaller==0 ? "N/A" : networkCaller->GetLastSenderAddress().ToString());
		return 0;
	}*/

	void RakNetInputTransferComponent::OnClientEnterVehicle(ClientEnterVehicleMessagePtr message)
	{
		RakNetNetworkMasterComponentPtr comp = GetSceneObject()->GetFirstComponent<RakNetNetworkMasterComponent>();
		if(comp)
		{
			comp->GetReplica()->EnterObject(message->GetClient().c_str(),0);
		}
		else
		{
			RakNetNetworkChildComponentPtr comp = GetSceneObject()->GetFirstComponent<RakNetNetworkChildComponent>();
			if(comp)
			{
				comp->GetReplica()->EnterObject(message->GetClient().c_str(),0);
		//		raknet->GetRPC()->SetRecipientObject(comp->GetReplica()->GetNetworkID());
			}
		}
	}
}

