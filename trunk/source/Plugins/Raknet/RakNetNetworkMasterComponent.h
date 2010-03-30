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

#ifndef RAK_NET_NETWORK_MASTER_COMPONENT_H
#define RAK_NET_NETWORK_MASTER_COMPONENT_H


#include "PacketPriority.h"
#include "Replica.h"
#include "StringTable.h"
#include "BitStream.h"
#include "GetTime.h"

#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Scenario/Scene/SceneObjectMessages.h"
#include "Sim/Systems/Input/ControlSetting.h"
#include "Sim/Common.h"
#include "Plugins/Game/GameMessages.h"
#include "Plugins/RakNet/RakNetMessages.h"

namespace GASS
{
	class SceneObject;
	class RakNetMasterReplica;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr;
	typedef std::vector<NetworkSerializeMessage::NetworkPackagePtr> NetworkPackageVector;

	class RakNetNetworkMasterComponent : public Reflection<RakNetNetworkMasterComponent,BaseSceneComponent>
	{
	public:
		RakNetNetworkMasterComponent();
		virtual ~RakNetNetworkMasterComponent();
		static void RegisterReflection();
		virtual void OnCreate();
		void OnLoad(LoadGameComponentsMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		RakNetMasterReplica* GetReplica() const {return m_Replica;}
		void SetReplica(RakNetMasterReplica* replica) {m_Replica=replica;}
		void SetAttributes(const std::vector<std::string> &attributes){m_Attributes = attributes;}
		std::vector<std::string> GetAttributes()const {return m_Attributes;}
		//NetworkPackageVector GetNetworkPackages() {return m_SerilizePackages;}
		void Serialize(bool *sendTimestamp, RakNet::BitStream *outBitStream, RakNetTime lastSendTime, PacketPriority *priority, PacketReliability *reliability, RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags);
		void Deserialize(RakNet::BitStream *inBitStream, RakNetTime timestamp, RakNetTime lastDeserializeTime, SystemAddress systemAddress );

	private:
		void OnSerialize(NetworkSerializeMessagePtr message);
		void OnNewReplica(ReplicaCreatedMessagePtr message);
		RakNetMasterReplica* m_Replica;
		std::vector<std::string> m_Attributes;
		NetworkPackageVector m_SerializePackages;
	};

	typedef boost::shared_ptr<RakNetNetworkMasterComponent> RakNetNetworkMasterComponentPtr;
}
#endif
