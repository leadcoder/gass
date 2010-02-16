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
		m_TemplateName = GetSceneObject()->GetTemplateName();
		//m_PartId = GetSceneObject()->GetPartId();

		//Is master
		if(IsMaster())
		{
			m_Replica = new RakNetReplicaMember;
			m_Replica->SetParent(this);
			m_Replica->SetNetworkIDManager(caller->GetNetworkIDManager());
			SceneObjectPtr object_root =  object->GetSceneObject()->GetRoot();
			if(object_root)
			{
				RakNetNetworkComponentPtr root_net_obj = object_root->GetFirstComponent<RakNetNetworkComponent>();
				if(root_net_obj)
					m_PartOfId = root_net_obj->GetReplica()->GetNetworkID();
		}

		if (caller->IsServer())
			caller->GetReplicaManager()->Construct(m_Replica , false, UNASSIGNED_SYSTEM_ADDRESS, true);

		// For security, as a server disable these interfaces
		if (caller->IsServer())
		{
			// For security, as a server disable all receives except REPLICA_RECEIVE_SERIALIZE
			// I could do this manually by putting if (isServer) return; at the top of all my receive functions too.
			caller->GetReplicaManager()->DisableReplicaInterfaces(m_Replica, REPLICA_RECEIVE_DESTRUCTION | REPLICA_RECEIVE_SCOPE_CHANGE );
		}
		else
		{
			// For convenience and for saving bandwidth, as a client disable all sends except REPLICA_SEND_SERIALIZE
			// I could do this manually by putting if (isServer==false) return; at the top of all my send functions too.
			caller->GetReplicaManager()->DisableReplicaInterfaces(m_Replica, REPLICA_SEND_CONSTRUCTION | REPLICA_SEND_DESTRUCTION | REPLICA_SEND_SCOPE_CHANGE );
		}
	}

	void RakNetNetworkComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
	//	message->GetSimSceneManager()->GetScenarioScene()->UnregisterForMessage(SCENARIO_RM_ENTER_VEHICLE,TYPED_MESSAGE_FUNC(PlayerInputComponent::OnEnter,AnyMessage));
	}

}
