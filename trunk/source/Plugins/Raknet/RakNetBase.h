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
//#include "Network/INetworkObject.h"
#include "Core/Utils/Log.h"
#include "Core/Math/Vector.h"
#include "Core/Math/Quaternion.h"

namespace GASS
{
	class RakNetNetworkSystem;
	class RakNetReplicaMember;
	
	enum DATA_TAG
	{
		TRANSFORMATION_DATA = 1,
        INPUT_DATA = 2,
		OBJECT_DATA = 4,
	};

	class RakNetBase 
	{
	public:
		RakNetBase();
		virtual ~RakNetBase();
		virtual void Update(double delta);
		virtual bool IsMaster();
		virtual void CreateaBaseObject();
		void RemoteInit(RakNet::BitStream *inBitStream, RakNetTime timestamp, NetworkID networkID, SystemAddress senderId, RakNetNetworkSystem *caller);
		void LocalInit(BaseObject *object, RakNetNetworkSystem *caller);
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

	protected:
		unsigned char m_DataToReceive;
		unsigned char m_DataToSend;
		std::string m_TemplateName;
		bool m_AllowRemoteOwner;
	private:
		SystemAddress m_OwnerSystemAddress;
		NetworkID m_ActionHandlerPlayerId;
		float m_DeadCount;
		RakNetTime m_UpdateTimeStamp[4];
		Vec3 m_Pos[4];
		Quaternion m_Rotation[4];
		int m_PartId;
		NetworkID m_PartOfId;

		RakNetReplicaMember* m_Replica;
		

		//#define DECLARE_RakNetBase_FUNCTIONS
		//virtual int GetID(){return 0;}//DistributedNetworkObject::GetID();}
		virtual int GetSessionID(){/* TODO */ return 0; } //ReplicaObject::GetSessionID();}
		virtual char* GetStringID(){return NULL;}
		//virtual std::string GetName(){return m_Name;}
		virtual void SetStringID(std::string id){}
		//virtual void SetName(std::string name){strcpy(m_Name,name.c_str());}
		virtual bool IsRoot(){return (m_PartId == 0);}
		//virtual void SetRoot(char value){m_Root = value;}
		//virtual void Publish(){ReplicaObject::Publish();}
		virtual std::string GetUniqueNetworkName()
		{
			return "tst";
			/*int id = GetID();
			int sid = GetSessionID();
			char id_string[256];
			sprintf(id_string,"%d/%d",id,sid);
			return std::string(id_string);*/
		}
		std::string GetUniqueEngineName()
		{
			return std::string("test");
		}

	protected:
		bool m_SendStopData;
	};
}

#endif
