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

#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/Messages/GASSNetworkSceneObjectMessages.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Sim/GASSCommon.h"
#include "Plugins/RakNet/RakNetMessages.h"

namespace GASS
{
	class SceneObject;
	class RakNetMasterReplica;
	typedef SPTR<SceneObject> SceneObjectPtr;
	typedef WPTR<SceneObject> SceneObjectWeakPtr;
	typedef std::vector<NetworkPackagePtr> NetworkPackageVector;

	class RakNetNetworkMasterComponent : public Reflection<RakNetNetworkMasterComponent,BaseSceneComponent>
	{
	public:
		RakNetNetworkMasterComponent();
		virtual ~RakNetNetworkMasterComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();
		RakNetMasterReplica* GetReplica() const {return m_Replica;}
		void SetReplica(RakNetMasterReplica* replica) {m_Replica=replica;}
		void SetAttributes(const std::vector<std::string> &attributes){m_Attributes = attributes;}
		std::vector<std::string> GetAttributes()const {return m_Attributes;}
		//NetworkPackageVector GetNetworkPackages() {return m_SerilizePackages;}
		void Serialize(bool *sendTimestamp, RakNet::BitStream *outBitStream, RakNetTime lastSendTime, PacketPriority *priority, PacketReliability *reliability, RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags);
		void Deserialize(RakNet::BitStream *inBitStream, RakNetTime timestamp, RakNetTime lastDeserializeTime, SystemAddress systemAddress );

	private:
		void GeneratePartID(SceneObjectPtr obj, int &id);
		void OnSerialize(NetworkSerializeRequestPtr message);
		void OnNetworkPostUpdate(NetworkPostUpdateEventPtr message);
		//void OnNewReplica(ReplicaCreatedMessagePtr message);
		RakNetMasterReplica* m_Replica;
		std::vector<std::string> m_Attributes;
		NetworkPackageVector m_SerializePackages;
	};

	typedef SPTR<RakNetNetworkMasterComponent> RakNetNetworkMasterComponentPtr;
}
#endif
