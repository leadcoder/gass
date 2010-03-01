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
#include "Plugins/RakNet/RakNetBase.h"
#include "RakNetLocationTransferComponent.h"


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
	RakNetLocationTransferComponent::RakNetLocationTransferComponent() 
	{

	}

	RakNetLocationTransferComponent::~RakNetLocationTransferComponent()
	{

	}

	void RakNetLocationTransferComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("LocationTransferComponent",new Creator<RakNetLocationTransferComponent, IComponent>);
	}

	void RakNetLocationTransferComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetLocationTransferComponent::OnUnload,UnloadComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetLocationTransferComponent::OnLoad,LoadGameComponentsMessage,1));
			}

	void RakNetLocationTransferComponent::OnLoad(LoadGameComponentsMessagePtr message)
	{
		RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<RakNetNetworkSystem>();
		RakNetNetworkComponentPtr nc = GetSceneObject()->GetFirstComponent<RakNetNetworkComponent>();
		if(!nc)
			Log::Error("RakNetLocationTransferComponent require RakNetNetworkComponent to be present");
		if(raknet->IsServer())
		{
			
		}
		else
		{
			GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetLocationTransferComponent::OnTransformationChanged,TransformationNotifyMessage,0));
		}
	}

	void RakNetLocationTransferComponent::OnTransformationChanged(TransformationNotifyMessagePtr message)
	{
		//Request data transfer
		RakNetNetworkComponentPtr nc = GetSceneObject()->GetFirstComponent<RakNetNetworkComponent>();

		//pack data

		NetworkSerializeMessage::NetworkPackage package;
		package.Id = TRANSFORMATION_DATA;
		package.Data = 
		
		MessagePtr message(new NetworkSerializeMessage(package));
		
	}

	void RakNetLocationTransferComponent::OnUnload(UnloadComponentsMessagePtr message)
	{

	}
	
}

