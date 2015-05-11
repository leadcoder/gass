/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                        *
*                                                                           *
* Copyright (c) 2008-2015 GASS team. See Contributors.txt for details.      *
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

#ifndef RAKNET_CHILD_REPLICA_H
#define RAKNET_CHILD_REPLICA_H

#include "Sim/GASSCommon.h"
#include "PacketPriority.h"
#include "Replica.h"
#include "StringTable.h"
#include "BitStream.h"
#include "GetTime.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSQuaternion.h"
#include "RakNetBaseReplica.h"
class ReplicaManager;
	
namespace GASS
{
	class SceneObject;
	class BaseReflectionObject;
	class IProperty;
	typedef SPTR<SceneObject> SceneObjectPtr;

	class RakNetChildReplica  : public RakNetBaseReplica
	{
	public:
		RakNetChildReplica(ReplicaManager* manager);
		virtual ~RakNetChildReplica();
		void RemoteInit(RakNet::BitStream *inBitStream, RakNetTime timestamp, NetworkID networkID, SystemAddress senderId);
		void LocalInit(SceneObjectPtr object);
		
		//Replica member functions
		virtual ReplicaReturnResult SendConstruction( RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags, RakNet::BitStream *outBitStream, bool *includeTimestamp );
		virtual ReplicaReturnResult SendDestruction(RakNet::BitStream *outBitStream, SystemAddress systemAddress, bool *includeTimestamp);
		virtual ReplicaReturnResult ReceiveDestruction(RakNet::BitStream *inBitStream, SystemAddress systemAddress, RakNetTime timestamp);
		virtual ReplicaReturnResult SendScopeChange(bool inScope, RakNet::BitStream *outBitStream, RakNetTime currentTime, SystemAddress systemAddress, bool *includeTimestamp);
		virtual ReplicaReturnResult ReceiveScopeChange(RakNet::BitStream *inBitStream, SystemAddress systemAddress, RakNetTime timestamp);
		virtual ReplicaReturnResult Serialize(bool *sendTimestamp, RakNet::BitStream *outBitStream, RakNetTime lastSendTime, PacketPriority *priority, PacketReliability *reliability, RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags);
		virtual ReplicaReturnResult Deserialize(RakNet::BitStream *inBitStream, RakNetTime timestamp, RakNetTime lastDeserializeTime, SystemAddress systemAddress );
		
		virtual int GetSortPriority(void) const 
		{
			return 0;
		}

		virtual void ReceiveConstruction(RakNet::BitStream *inBitStream);
		void SendConstruction(RakNet::BitStream *outBitStream);
		SystemAddress  GetOwnerSystemAddress() {return m_OwnerSystemAddress;}
		void SetOwnerSystemAddress(SystemAddress sa) {m_OwnerSystemAddress = sa;}
		bool AllowRemoteOwner(){return m_AllowRemoteOwner;}
		
		void SetPartId(int id) {m_PartId = id;}
		int GetPartId()const {m_PartId;}
		NetworkID GetPartOfId(){return m_PartOfId;}
		int GetPartId(){return m_PartId;}
		void SerializeProperties(RakNet::BitStream *bit_stream);
		void DeserializeProperties(RakNet::BitStream *bit_stream);
		bool HasPropertiesChanged();
	protected:
		bool GetProperty(const std::string &prop_name, BaseReflectionObject* &component, IProperty* &abstract_property);
		
		bool m_AllowRemoteOwner;
	private:
		SystemAddress m_OwnerSystemAddress;
		NetworkID m_PartOfId;
		ReplicaManager* m_Manager;
		int m_PartId;
		std::vector<std::string>  m_SavedValues;
	protected:
	};
}

#endif
