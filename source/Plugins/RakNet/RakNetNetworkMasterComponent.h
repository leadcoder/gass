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

#ifndef RAK_NET_NETWORK_MASTER_COMPONENT_H
#define RAK_NET_NETWORK_MASTER_COMPONENT_H


#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Interface/GASSINetworkComponent.h"
#include "Sim/GASSComponent.h"
#include "Sim/Messages/GASSNetworkSceneObjectMessages.h"
#include "Sim/GASSCommon.h"
#include "Plugins/RakNet/RakNetMessages.h"
#include "RakNetCommon.h"

namespace GASS
{
	class SceneObject;
	class RakNetMasterReplica;
	using SceneObjectPtr = std::shared_ptr<SceneObject>;
	using SceneObjectWeakPtr = std::weak_ptr<SceneObject>;
	using NetworkPackageVector = std::vector<NetworkPackagePtr>;

	class RakNetNetworkMasterComponent : public Reflection<RakNetNetworkMasterComponent,INetworkComponent>
	{
	public:
		RakNetNetworkMasterComponent();
		~RakNetNetworkMasterComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;
		RakNetMasterReplica* GetReplica() const {return m_Replica;}
		void SetReplica(RakNetMasterReplica* replica) {m_Replica=replica;}
		void SetAttributes(const std::vector<std::string> &attributes){m_Attributes = attributes;}
		std::vector<std::string> GetAttributes()const {return m_Attributes;}
		bool IsRemote() const override;
		void Serialize(NetworkPackagePtr package, unsigned int timeStamp, NetworkAddress address) override;
		void Serialize(bool *sendTimestamp, RakNet::BitStream *outBitStream, RakNetTime lastSendTime, PacketPriority *priority, PacketReliability *reliability, RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags);
		void Deserialize(RakNet::BitStream *inBitStream, RakNetTime timestamp, RakNetTime lastDeserializeTime, SystemAddress systemAddress );
	private:
		void GeneratePartID(SceneObjectPtr obj, int &id);
		void OnNetworkPostUpdate(NetworkPostUpdateEventPtr message);
		RakNetMasterReplica* m_Replica{nullptr};
		std::vector<std::string> m_Attributes;
		NetworkPackageVector m_SerializePackages;
	};

	using RakNetNetworkMasterComponentPtr = std::shared_ptr<RakNetNetworkMasterComponent>;
}
#endif
