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

#include "GetTime.h"

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
		ComponentFactory::GetPtr()->Register("InputTransferComponent",new Creator<RakNetInputTransferComponent, IComponent>);
		GASS::PackageFactory::GetPtr()->Register(INPUT_DATA,new GASS::Creator<InputPackage, NetworkPackage>);	
		//RegisterProperty<float>("SendFrequency", &RakNetInputTransferComponent::GetSendFrequency, &RakNetInputTransferComponent::SetSendFrequency);
	}

	void RakNetInputTransferComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetInputTransferComponent::OnUnload,UnloadComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetInputTransferComponent::OnLoad,LoadGameComponentsMessage,1));
	}

	void RakNetInputTransferComponent::OnLoad(LoadGameComponentsMessagePtr message)
	{
		RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<RakNetNetworkSystem>();
		//RakNetNetworkComponentPtr nc = GetSceneObject()->GetFirstComponent<RakNetNetworkComponent>();
		//if(!nc)
		//	Log::Error("RakNetInputTransferComponent require RakNetNetworkComponent to be present");
		if(raknet->IsServer())
		{
			GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetInputTransferComponent::OnInput,ControllerMessage,0));
		}
		else
		{
			GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetInputTransferComponent::OnDeserialize,NetworkDeserializeMessage,0));
		}
	}

	void RakNetInputTransferComponent::OnInput(ControllerMessagePtr message)
	{
		RakNetTime time_stamp = RakNet::GetTime();
		std::string controller = message->GetController();
		ControlSetting* cs = SimEngine::Get().GetControlSettingsManager()->GetControlSetting(m_ControlSetting);
		int index = cs->m_NameToIndex[controller];
		float value = message->GetValue();
		boost::shared_ptr<InputPackage> package(new InputPackage(INPUT_DATA,time_stamp,index,value));
		MessagePtr serialize_message(new NetworkSerializeMessage(0,package));
		GetSceneObject()->SendImmediate(serialize_message);
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
			ControlSetting* cs = SimEngine::Get().GetControlSettingsManager()->GetControlSetting(m_ControlSetting);
			std::string controller = cs->m_IndexToName[input_package->Index];
			MessagePtr message(new ControllerMessage(controller,input_package->Value));
			GetSceneObject()->PostMessage(message);
		}
	}

	TaskGroup RakNetInputTransferComponent::GetTaskGroup() const 
	{
		return NETWORK_TASK_GROUP;
	}
}

