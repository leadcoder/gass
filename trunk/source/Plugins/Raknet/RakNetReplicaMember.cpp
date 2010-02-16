#include "RakNetReplicaMember.h"
#include "RakNetBase.h"


// This file demonstrates using an object of class Replica as a member of another object, rather than inheriting it.
// All I do here is pass all calls up to the parent.
// In this case I know the parent is of class RakNetBase, but in a real game you'd use Entity or Object or whatever your base network class is.
namespace GASS
{
	ReplicaReturnResult RakNetReplicaMember::SendConstruction( RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags, RakNet::BitStream *outBitStream, bool *includeTimestamp )
	{
		return ((RakNetBase*)GetParent())->SendConstruction(currentTime, systemAddress, flags, outBitStream, includeTimestamp);
	}
	ReplicaReturnResult RakNetReplicaMember::SendDestruction(RakNet::BitStream *outBitStream, SystemAddress systemAddress, bool *includeTimestamp)
	{
		return ((RakNetBase*)GetParent())->SendDestruction( outBitStream, systemAddress, includeTimestamp);
	}
	ReplicaReturnResult RakNetReplicaMember::ReceiveDestruction(RakNet::BitStream *inBitStream, SystemAddress systemAddress, RakNetTime timestamp)
	{
		return ((RakNetBase*)GetParent())->ReceiveDestruction(inBitStream, systemAddress, timestamp);
	}
	ReplicaReturnResult RakNetReplicaMember::SendScopeChange(bool inScope, RakNet::BitStream *outBitStream, RakNetTime currentTime, SystemAddress systemAddress, bool *includeTimestamp)
	{
		return ((RakNetBase*)GetParent())->SendScopeChange(inScope, outBitStream, currentTime, systemAddress, includeTimestamp);
	}
	ReplicaReturnResult RakNetReplicaMember::ReceiveScopeChange(RakNet::BitStream *inBitStream, SystemAddress systemAddress, RakNetTime timestamp)
	{
		return ((RakNetBase*)GetParent())->ReceiveScopeChange(inBitStream, systemAddress, timestamp);
	}
	ReplicaReturnResult RakNetReplicaMember::Serialize(bool *sendTimestamp, RakNet::BitStream *outBitStream, RakNetTime lastSendTime, PacketPriority *priority, PacketReliability *reliability, RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags)
	{
		return ((RakNetBase*)GetParent())->Serialize(sendTimestamp, outBitStream, lastSendTime, priority, reliability, currentTime, systemAddress, flags);
	}
	ReplicaReturnResult RakNetReplicaMember::Deserialize(RakNet::BitStream *inBitStream, RakNetTime timestamp, RakNetTime lastDeserializeTime, SystemAddress systemAddress )
	{
		return ((RakNetBase*)GetParent())->Deserialize(inBitStream, timestamp, lastDeserializeTime, systemAddress);
	}
	bool RakNetReplicaMember::RequiresSetParent(void) const
	{
		return true;
	}

	bool RakNetReplicaMember::IsNetworkIDAuthority(void) const
	{
		// In peer to peer, everyone is an authority.  You must define _P2P_OBJECT_ID for this to work.
		return true;
	}

}
