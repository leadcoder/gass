/****************************************************************************
* This file is part of GASS.                                                *
* See http://sourceforge.net/projects/gass/                                 *
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

#ifndef RAKNET_BASE_H
#define RAKNET_BASE_H
#include "PacketPriority.h"
#include "Replica.h"
#include "StringTable.h"
#include "BitStream.h"
#include "GetTime.h"
//#include "Network/INetworkObject.h"
#include "Core/Utils/Log.h"
#include "Core/Math/Vector.h"
#include "Core/Math/Quaternion.h"
#include "RakNetNetworkComponent.h"
#include "RakNetReplicaMember.h"


class ReplicaManager;
	
namespace GASS
{

	class RakNetBase 
	{
	public:
		RakNetBase(ReplicaManager* manager);
		virtual ~RakNetBase();
		//virtual void Update(double delta);
		virtual bool IsMaster();
		//virtual void CreateaBaseObject();
		void RemoteInit(RakNet::BitStream *inBitStream, RakNetTime timestamp, NetworkID networkID, SystemAddress senderId);
		void LocalInit(SceneObjectPtr object);
		RakNetReplicaMember* GetReplica(){return m_Replica;}

		//Replica member functions
		virtual ReplicaReturnResult SendConstruction( RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags, RakNet::BitStream *outBitStream, bool *includeTimestamp );
		virtual ReplicaReturnResult SendDestruction(RakNet::BitStream *outBitStream, SystemAddress systemAddress, bool *includeTimestamp);
		virtual ReplicaReturnResult ReceiveDestruction(RakNet::BitStream *inBitStream, SystemAddress systemAddress, RakNetTime timestamp);
		virtual ReplicaReturnResult SendScopeChange(bool inScope, RakNet::BitStream *outBitStream, RakNetTime currentTime, SystemAddress systemAddress, bool *includeTimestamp);
		virtual ReplicaReturnResult ReceiveScopeChange(RakNet::BitStream *inBitStream, SystemAddress systemAddress, RakNetTime timestamp);
		virtual ReplicaReturnResult Serialize(bool *sendTimestamp, RakNet::BitStream *outBitStream, RakNetTime lastSendTime, PacketPriority *priority, PacketReliability *reliability, RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags);
		virtual ReplicaReturnResult Deserialize(RakNet::BitStream *inBitStream, RakNetTime timestamp, RakNetTime lastDeserializeTime, SystemAddress systemAddress );

		virtual void ReceiveConstruction(RakNet::BitStream *inBitStream);
		void SendConstruction(RakNet::BitStream *outBitStream);
		SystemAddress  GetOwnerSystemAddress() {return m_OwnerSystemAddress;}
		void SetOwnerSystemAddress(SystemAddress sa) {m_OwnerSystemAddress = sa;}
		bool AllowRemoteOwner(){return m_AllowRemoteOwner;}
		NetworkID GetPartOfId(){return m_PartOfId;}
		int GetPartId(){return m_PartId;}
		std::string GetTemplateName() {return m_TemplateName;}
		void SetOwner(SceneObjectPtr object) {m_Owner = object;}
	protected:
		AbstractProperty* GetProperty(const std::string &prop_name);
		void SerializeProperties(RakNet::BitStream *bit_stream);
	



		unsigned char m_DataToReceive;
		unsigned char m_DataToSend;
		std::string m_TemplateName;
		bool m_AllowRemoteOwner;
	private:
		SystemAddress m_OwnerSystemAddress;
		NetworkID m_ActionHandlerPlayerId;
		int m_PartId;
		NetworkID m_PartOfId;
		RakNetReplicaMember* m_Replica;
		ReplicaManager* m_Manager;
		SceneObjectPtr m_Owner;
	protected:
	};
}

#endif
