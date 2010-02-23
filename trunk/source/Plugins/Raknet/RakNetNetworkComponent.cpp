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

#include "RakNetNetworkComponent.h"
#include "Plugins/RakNet/RakNetNetworkSystem.h"
#include "Plugins/RakNet/RakNetBase.h"

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
	RakNetNetworkComponent::RakNetNetworkComponent() 
	{

	}

	RakNetNetworkComponent::~RakNetNetworkComponent()
	{

	}

	void RakNetNetworkComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("NetworkComponent",new Creator<RakNetNetworkComponent, IComponent>);
	}

	void RakNetNetworkComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetNetworkComponent::OnUnload,UnloadComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetNetworkComponent::OnLoad,LoadGameComponentsMessage,0));
	}

	void RakNetNetworkComponent::OnLoad(LoadGameComponentsMessagePtr message)
	{
		RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<RakNetNetworkSystem>();
		if(raknet->IsServer())
		{
			RakNetBase* replica = new RakNetBase(raknet->GetReplicaManager());
			replica->LocalInit(GetSceneObject());
		}
		else
		{
			if(replica) //top object
			{
				
			}
			else
			{
				//Get part of id
				SceneObjectPtr root = GetSceneObject();
				while(root->GetParentSceneObject())
					root = root->GetParentSceneObject();
				RakNetNetworkComponentPtr top_comp = root->GetFirstComponent<RakNetNetworkComponent>();
				NetworkID part_of_id = top_comp->GetReplica()->GetPartOfId();
				int part_id = GetPartId();
				RakNetBase* replica = raknet->FindReplica(part_of_id,part_id);
				if(!replica) //not available jet
					GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetNetworkComponent::OnReplicaCreated,ReplicaCreatedMessage,0));
			}
		}
	}

	void RakNetNetworkComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
	//	message->GetSimSceneManager()->GetScenarioScene()->UnregisterForMessage(SCENARIO_RM_ENTER_VEHICLE,TYPED_MESSAGE_FUNC(PlayerInputComponent::OnEnter,AnyMessage));
	}

	void RakNetNetworkComponent::OnReplicaCreated(ReplicaCreatedMessagePtr message)
	{
		Replica message->
	}
}
