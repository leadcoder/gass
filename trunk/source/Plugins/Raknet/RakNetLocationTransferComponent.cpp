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
		GASS::PackageFactory::GetPtr()->Register(TRANSFORMATION_DATA,new GASS::Creator<TransformationPackage, NetworkPackage>);	
	}

	void RakNetLocationTransferComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetLocationTransferComponent::OnUnload,UnloadComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetLocationTransferComponent::OnLoad,LoadGameComponentsMessage,1));
	}

	void RakNetLocationTransferComponent::OnLoad(LoadGameComponentsMessagePtr message)
	{
		RakNetNetworkSystemPtr raknet = SimEngine::Get().GetSimSystemManager()->GetFirstSystem<RakNetNetworkSystem>();
		//RakNetNetworkComponentPtr nc = GetSceneObject()->GetFirstComponent<RakNetNetworkComponent>();
		//if(!nc)
		//	Log::Error("RakNetLocationTransferComponent require RakNetNetworkComponent to be present");
		if(raknet->IsServer())
		{
			GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetLocationTransferComponent::OnTransformationChanged,TransformationNotifyMessage,0));
		}
		else
		{
			GetSceneObject()->RegisterForMessage(REG_TMESS(RakNetLocationTransferComponent::OnSerialize,NetworkSerializeMessage,0));

			MessagePtr disable_msg(new PhysicsBodyMessage(PhysicsBodyMessage::DISABLE,Vec3(0,0,0)));
			GetSceneObject()->PostMessage(disable_msg);

			IComponentContainerTemplate::ComponentVector components;
			GetSceneObject()->GetComponentsByClass(components,"ODEBodyComponent");
			for(int i = 0;  i< components.size(); i++)
			{
				BaseSceneComponentPtr comp = boost::shared_dynamic_cast<BaseSceneComponent>(components[i]);
				comp->GetSceneObject()->PostMessage(disable_msg);
			}
		}
	}

	void RakNetLocationTransferComponent::OnTransformationChanged(TransformationNotifyMessagePtr message)
	{
		//Request data transfer
		//RakNetNetworkComponentPtr nc = GetSceneObject()->GetFirstComponent<RakNetNetworkComponent>();

		//pack data

		Vec3 pos = message->GetPosition();
		Quaternion rot = message->GetRotation();
		boost::shared_ptr<TransformationPackage> package(new TransformationPackage(TRANSFORMATION_DATA,pos, rot));
		
		
		/*package.Id = TRANSFORMATION_DATA;


		//pack position data
		Vec3 pos = message->GetPosition();
		package.Data = boost::shared_ptr<char>(new char[sizeof(Vec3)]);
		*(Vec3*) package.Data.get() = pos;
		package.Size = sizeof(Vec3);*/

		MessagePtr serialize_message(new NetworkSerializeMessage(package));
		GetSceneObject()->SendImmediate(serialize_message);

	}

	void RakNetLocationTransferComponent::OnUnload(UnloadComponentsMessagePtr message)
	{

	}


	void RakNetLocationTransferComponent::OnSerialize(NetworkSerializeMessagePtr message)
	{
		if(message->GetPackage()->Id == TRANSFORMATION_DATA)
		{
			NetworkPackagePtr package = message->GetPackage();
			//Log::Print("Before cast\n");
			TransformationPackagePtr trans_package = boost::shared_dynamic_cast<TransformationPackage>(package);
			//Log::Print("after cast\n");
			Vec3 pos = trans_package->Position;
			Quaternion rot = trans_package->Rotation;
			GetSceneObject()->PostMessage(MessagePtr(new PositionMessage(pos)));
			GetSceneObject()->PostMessage(MessagePtr(new RotationMessage(rot)));
			//std::cout << pos << std::endl;
		}
	}

}

