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


#include "ReplicaManager.h"
#include "StringTable.h"
#include "BitStream.h"
#include "GetTime.h"

#include "RakNetBase.h"
#include "RakNetNetworkSystem.h"

namespace GASS
{
	RakNetBase::RakNetBase() 
	{
		m_SendStopData = true;
//		m_ActionHandlerPlayer = NULL;
		m_PartId = 0;
		m_OwnerSystemAddress = UNASSIGNED_SYSTEM_ADDRESS;
		m_Replica = NULL;
		m_AllowRemoteOwner = false;
		m_DeadCount = 0;
		m_DataToReceive = -1;

		for(int i = 0; i < 4 ;i++)
		{
			m_UpdateTimeStamp[i] = 0;
			m_Pos[i] = Vec3(0,0,0);
			m_Rotation[i] = Quaternion(0,0,0,0);
		}
	}

	RakNetBase::~RakNetBase()
	{
/*		if(m_Owner)
		{
			m_Owner->SetNetworkObject(NULL);
			if(!IsMaster())	m_Owner->SetDelete(true);
		}*/

		if(m_Replica)
		{
			RakNetNetworkManager* raknet = (RakNetNetworkManager*) Root::Get().GetNetworkManager();
			raknet->GetReplicaManager()->Destruct(m_Replica, UNASSIGNED_SYSTEM_ADDRESS, true); // Forward the destruct message to all other systems but the sender
			raknet->GetReplicaManager()->DereferencePointer(m_Replica);
			delete m_Replica;
		}
	}

	void RakNetBase::LocalInit(RakNetNetworkComponentPtr object, RakNetNetworkManager *caller)
	{
		m_Owner = object;
		m_TemplateName = object->GetTemplateName();
		m_PartId = object->GetPartId();

		m_Replica = new RakNetReplicaMember;
		m_Replica->SetParent(this);

		m_Replica->SetNetworkIDManager(caller->GetNetworkIDManager());
		SceneObjectPtr object_root =  object->GetSceneObject()->GetRoot();
		if(object_root)
		{
			RakNetBase* root_net_obj = (RakNetBase*) object_root->GetNetworkObject();
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

	void RakNetBase::RemoteInit(RakNet::BitStream *inBitStream, RakNetTime timestamp, NetworkID networkID, SystemAddress senderId, RakNetNetworkManager *caller)
	{
		m_Replica = new RakNetReplicaMember;
		m_Replica->SetParent(this);

		m_Replica->SetNetworkIDManager(caller->GetNetworkIDManager());
		// We must set the network ID of all remote objects
		m_Replica->SetNetworkID(networkID);

		// Tell the replica manager to create this as an object that originated on a remote node
		caller->GetReplicaManager()->Construct(m_Replica, true, senderId, false);

		// Since SendConstruction is not called for copies and we were calling SetScope there, we need to call it here instead.
		caller->GetReplicaManager()->SetScope(m_Replica, true, senderId, false);

		ReceiveConstruction(inBitStream);


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

		// if this is a server and we receive a remotely created object, we must forward that creation
		// to all of our clients
		if (caller->IsServer())
		{
			caller->GetReplicaManager()->Construct(m_Replica, false, senderId, true);
		}
	}


	void RakNetBase::Update(float delta)
	{
		m_DataToSend = 0; //reset

		//Font::DebugPrint("Object %s Neitid %d %s",GetOwner()->GetName().c_str(),GetReplica()->GetNetworkID().localSystemAddress,GetReplica()->GetNetworkID().systemAddress.ToString());

		RakNetNetworkManager* raknet = (RakNetNetworkManager*) Root::Get().GetNetworkManager();
		if(IsMaster() )
		{
			Mat4 mat = m_Owner->GetAbsoluteTransformation();
			m_Pos[1] = m_Pos[0];
			m_Pos[0] = mat.GetTranslation(); //ABS pos

			m_Rotation[1] = m_Rotation[0];
			Quaternion quat;
			quat.FromRotationMatrix(mat);
			m_Rotation[0] = quat; //ABS rot

			m_DeadCount += delta;
			if(raknet->UpdateTransformations()) //Update tranformation?
			{
				if(m_Pos[0] != m_Pos[1] || m_Rotation[0] != m_Rotation[1])
				{
					m_Rotation[3] = (m_Rotation[0] - m_Rotation[1]);
					m_Rotation[3] = m_Rotation[3]*(1.0/delta);

					m_Pos[3] = (m_Pos[0] - m_Pos[1]);
					m_Pos[3] = m_Pos[3]*(1.0/delta);
					//m_Pos[3] = m_Pos[3]*((l*(1.0/delta)));
					m_DataToSend |= TRANSFORMATION_DATA;
					m_SendStopData = true;
				}
				else if(m_SendStopData)
				{
					m_Rotation[3] = Quaternion(0,0,0,0);
					m_Pos[3] = Vec3(0,0,0);
					//m_Pos[3] = m_Pos[3]*((l*(1.0/delta)));
					m_DataToSend |= TRANSFORMATION_DATA;
					m_SendStopData = false;
				}
				m_DeadCount = 0;
			}

			ActionHandler* ah = m_Owner->GetActionHandler();
			if(ah)
			{
				IPlayer *player = ah->GetPlayer();
				/*if(player) 
				Font::DebugPrint("Action handler has player %s Neitid %d",player->GetName().c_str(),m_ActionHandlerPlayerId.localSystemAddress);
				else 
				Font::DebugPrint("Action handler no player");
				*/
				//Send every frame?
				//if(player != m_ActionHandlerPlayer)
				{
					m_ActionHandlerPlayer = player;
					if(m_ActionHandlerPlayer && player->GetNetworkObject())
					{
						RakNetPlayer* rakplayer = (RakNetPlayer*) player->GetNetworkObject();
						m_ActionHandlerPlayerId = rakplayer->GetReplica()->GetNetworkID();
					}
					m_DataToSend |= OBJECT_DATA;
				}
			}
			if(m_DataToSend != 0)
				raknet->GetReplicaManager()->SignalSerializeNeeded(m_Replica, UNASSIGNED_SYSTEM_ADDRESS, true);
		}
		else
		{

			if(m_DataToReceive != -1)
			{
				Quaternion quat;
				Vec3 pos;
				//m_Object->SetPosition(m_Pos[0]);
				RakNetTime step_back = 150; //
				RakNetTime time = RakNet::GetTime();

				//Font::DebugPrint("Time since last data recieved : %d",(time -m_UpdateTimeStamp[0]));
				//Font::DebugPrint("inte time: %d",(m_UpdateTimeStamp[0] -m_UpdateTimeStamp[1]));
				time = time - step_back;

				if(time > m_UpdateTimeStamp[0])  
				{
					//Font::DebugPrint("extrapolation Time before: %d",(time -m_UpdateTimeStamp[0]));
					/*float prev_delta_time = float(m_UpdateTimeStamp[0] - m_UpdateTimeStamp[1]);
					float delta_time = time - m_UpdateTimeStamp[0];
					Vec3 delta_dir = m_Pos[0]-m_Pos[1];
					float speed = delta_dir.Length()*(1.0/prev_delta_time);*/

					Vec3 delta_dir = m_Pos[3];

					delta_dir.Normalize();
					delta_dir = delta_dir*(m_Pos[3].Length()*m_DeadCount);
					//delta_dir = delta_dir* (1.0/prev_delta_time);
					//delta_dir = delta_dir*delta_time;

					pos = m_Pos[0] + delta_dir;

					quat.x = m_Rotation[0].x  + m_Rotation[3].x*m_DeadCount;
					quat.y = m_Rotation[0].y  + m_Rotation[3].y*m_DeadCount;
					quat.z = m_Rotation[0].z  + m_Rotation[3].z*m_DeadCount;
					quat.w = m_Rotation[0].w  + m_Rotation[3].w*m_DeadCount;
					//we have no new data ,extrapolate?
				}
				else if(time >= m_UpdateTimeStamp[1])
				{
					//Font::DebugPrint("interpolation");
					RakNetTime elapsed = m_UpdateTimeStamp[0] - time;
					RakNetTime tot = m_UpdateTimeStamp[0] - m_UpdateTimeStamp[1];
					float inter = float(elapsed)/float(tot);
					pos = (m_Pos[1]*inter) + (m_Pos[0]*(1.0-inter));
					quat = Quaternion::Slerp2(inter,m_Rotation[0], m_Rotation[1]);
				}
				else if(time >= m_UpdateTimeStamp[2])
				{
					//Font::DebugPrint("interpolation");
					RakNetTime elapsed = m_UpdateTimeStamp[1] - time;
					RakNetTime tot = m_UpdateTimeStamp[1] - m_UpdateTimeStamp[2];
					float inter = float(elapsed)/float(tot);
					pos = (m_Pos[2]*inter) + (m_Pos[1]*(1.0-inter));
					quat = Quaternion::Slerp2(inter,m_Rotation[1], m_Rotation[2]);
				}
				else
				{
					//Font::DebugPrint("behinde last update: %d",(m_UpdateTimeStamp[1]-time));
					quat = m_Rotation[2];
					pos = m_Pos[2];
				}
				m_DeadCount += delta;

				Mat4 transformation;
				transformation.Identity();
				quat.ToRotationMatrix(transformation);
				transformation.m_Data[3][0] = pos.x;
				transformation.m_Data[3][1] = pos.y;
				transformation.m_Data[3][2] = pos.z;
				m_Owner->SetAbsoluteTransformation(transformation);
				m_Owner->SetUpdateTransform(false);

				//Font::DebugPrint("Pos: %f %f %f",pos.x,pos.y,pos.z);

				/*Mat4 transformation;
				transformation.Identity();
				m_Rotation[index].ToRotationMatrix(transformation);
				transformation.m_Data[3][0] = m_Pos[index].x;
				transformation.m_Data[3][1] = m_Pos[index].y;
				transformation.m_Data[3][2] = m_Pos[index].z;
				m_Object->SetAbsoluteTransformation(transformation);
				m_Object->SetUpdateTransform(false);*/
				/*
				else if(m_HasData[prev_index] && m_HasData[next_index])
				{
				Mat4 transformation;
				transformation.Identity();
				Quateriono q = m_Rotation[prev_index].Slerp(0.5,m_Rotation[next_index]);
				q.ToRotationMatrix(transformation);
				transformation.m_Data[3][0] = m_Pos[index].x + ;
				transformation.m_Data[3][1] = m_Pos[index].y;
				transformation.m_Data[3][2] = m_Pos[index].z;
				m_Object->SetAbsoluteTransformation(transformation);
				m_Object->SetUpdateTransform(false);
				}*/
			}
		}
	}

	void RakNetBase::CreateaBaseObject()
	{
		//Check that we have a scenario loaded
		if(!Root::Get().GetLevel()->IsReadyToRun()) return;
		if(IsRoot()) //top object?
		{
			BaseObject* bo = (BaseObject*) Root::Get().GetBaseObjectTemplateManager()->CreateFromTemplate(m_TemplateName);
			if(bo)
			{
				bo->SetNetworkObject(this);
				//bo->SetName(m_Name);
				bo->Init();
				m_Owner = bo;
				//m_Object->SetUpdateTransform(true);
				Root::GetPtr()->GetLevel()->GetDynamicObjectContainer()->Add(bo);
			}
		}
	}

	ReplicaReturnResult RakNetBase::SendConstruction( RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags, RakNet::BitStream *outBitStream, bool *includeTimestamp )
	{
		// Don't send back to the owner of an object.
		// If we didn't prevent then the object would be created on the system that just sent it to us, then back again, forever in a feedback loop.
		//if (playerId==m_Owner)
		//	return REPLICA_PROCESSING_DONE;

		// This string was pre-registered in main with stringTable->AddString so we can send it with the string table and save bandwidth

		RakNet::StringTable::Instance()->EncodeString("RakNetBase", 255, outBitStream);
		// Write the owner when we construct the object, so we have it right away in order to prevent feedback loops
		RakNetBase::SendConstruction(outBitStream);

		//send part id
		return REPLICA_PROCESSING_DONE;
	}



	void RakNetBase::SendConstruction(RakNet::BitStream *outBitStream)
	{
		outBitStream->Write(m_OwnerSystemAddress);
		assert(m_Owner);
		outBitStream->Write(m_PartId);
		outBitStream->Write(m_PartOfId);
		std::string name = m_Owner->GetName();
		std::string template_name = m_Owner->GetTemplateName();

		outBitStream->Write(m_Pos[0].x);
		outBitStream->Write(m_Pos[0].y);
		outBitStream->Write(m_Pos[0].z);

		//Speed
		outBitStream->Write(m_Pos[3].x);
		outBitStream->Write(m_Pos[3].y);
		outBitStream->Write(m_Pos[3].z);

		outBitStream->Write(m_Rotation[0].x);
		outBitStream->Write(m_Rotation[0].y);
		outBitStream->Write(m_Rotation[0].z);
		outBitStream->Write(m_Rotation[0].w);

		//Speed
		outBitStream->Write(m_Rotation[3].x);
		outBitStream->Write(m_Rotation[3].y);
		outBitStream->Write(m_Rotation[3].z);
		outBitStream->Write(m_Rotation[3].w);
		//RakNetNetworkManager::WriteString(name,outBitStream);
		RakNetNetworkManager::WriteString(template_name,outBitStream);

	}



	void RakNetBase::ReceiveConstruction(RakNet::BitStream *inBitStream)
	{
		inBitStream->Read(m_OwnerSystemAddress);
		inBitStream->Read(m_PartId);
		inBitStream->Read(m_PartOfId);

		inBitStream->Read(m_Pos[0].x);
		inBitStream->Read(m_Pos[0].y);
		inBitStream->Read(m_Pos[0].z);

		inBitStream->Read(m_Pos[3].x);
		inBitStream->Read(m_Pos[3].y);
		inBitStream->Read(m_Pos[3].z);

		inBitStream->Read(m_Rotation[0].x);
		inBitStream->Read(m_Rotation[0].y);
		inBitStream->Read(m_Rotation[0].z);
		inBitStream->Read(m_Rotation[0].w);

		inBitStream->Read(m_Rotation[3].x);
		inBitStream->Read(m_Rotation[3].y);
		inBitStream->Read(m_Rotation[3].z);
		inBitStream->Read(m_Rotation[3].w);
		//m_Name = RakNetNetworkManager::ReadString(inBitStream);
		m_TemplateName = RakNetNetworkManager::ReadString(inBitStream);
	}



	ReplicaReturnResult  RakNetBase::SendDestruction(RakNet::BitStream *outBitStream, SystemAddress systemAddress, bool *includeTimestamp)
	{
		// Optional, nothing to send here.
		return REPLICA_PROCESSING_DONE;
	}

	ReplicaReturnResult RakNetBase::ReceiveDestruction(RakNet::BitStream *inBitStream, SystemAddress systemAddress, RakNetTime timestamp)
	{
		//printf("Remote object owned by %s:%i destroyed\n", rakPeer->PlayerIDToDottedIP(owner), owner.port);
		delete this;
		return REPLICA_PROCESSING_DONE;
	}

	ReplicaReturnResult RakNetBase::SendScopeChange(bool inScope, RakNet::BitStream *outBitStream, RakNetTime currentTime, SystemAddress systemAddress, bool *includeTimestamp)
	{
		outBitStream->Write(inScope);
		return REPLICA_PROCESSING_DONE;
	}

	ReplicaReturnResult RakNetBase::ReceiveScopeChange(RakNet::BitStream *inBitStream, SystemAddress systemAddress, RakNetTime timestamp)
	{
		return REPLICA_PROCESSING_DONE;
	}

	ReplicaReturnResult RakNetBase::Serialize(bool *sendTimestamp, RakNet::BitStream *outBitStream, RakNetTime lastSendTime, PacketPriority *priority, PacketReliability *reliability, RakNetTime currentTime, SystemAddress systemAddress, unsigned int &flags)
	{
		// Don't send back to the owner of an object.
		//if (playerId==m_Owner)
		//	return REPLICA_PROCESSING_DONE;
		//outBitStream->Write(testInteger);
		outBitStream->Write(m_DataToSend);
		if(m_DataToSend & TRANSFORMATION_DATA)
		{
			//Font::DebugPrint("Sned Pos: %f %f %f",m_Pos[0].x,m_Pos[0].y,m_Pos[0].z);
			*sendTimestamp = true;
			outBitStream->Write(m_Pos[0].x);
			outBitStream->Write(m_Pos[0].y);
			outBitStream->Write(m_Pos[0].z);

			//Speed
			outBitStream->Write(m_Pos[3].x);
			outBitStream->Write(m_Pos[3].y);
			outBitStream->Write(m_Pos[3].z);

			outBitStream->Write(m_Rotation[0].x);
			outBitStream->Write(m_Rotation[0].y);
			outBitStream->Write(m_Rotation[0].z);
			outBitStream->Write(m_Rotation[0].w);

			//Speed
			outBitStream->Write(m_Rotation[3].x);
			outBitStream->Write(m_Rotation[3].y);
			outBitStream->Write(m_Rotation[3].z);
			outBitStream->Write(m_Rotation[3].w);
		}
		if(m_DataToSend & OBJECT_DATA)
		{
			if(m_ActionHandlerPlayer)
			{
				outBitStream->Write(true); // has player
				outBitStream->Write(m_ActionHandlerPlayerId);
			}
			else
			{
				outBitStream->Write(false); // has player
			}
		}
		return REPLICA_PROCESSING_DONE;
	}
	ReplicaReturnResult RakNetBase::Deserialize(RakNet::BitStream *inBitStream, RakNetTime timestamp, RakNetTime lastDeserializeTime, SystemAddress systemAddress )
	{

		inBitStream->Read(m_DataToReceive);

		if(m_DataToReceive & TRANSFORMATION_DATA)
		{
			m_UpdateTimeStamp[2] = m_UpdateTimeStamp[1];
			m_UpdateTimeStamp[1] = m_UpdateTimeStamp[0];
			m_Pos[2] = m_Pos[1];
			m_Pos[1] = m_Pos[0];
			m_Rotation[2] = m_Rotation[1];
			m_Rotation[1] = m_Rotation[0];
			m_UpdateTimeStamp[0] = timestamp;

			inBitStream->Read(m_Pos[0].x);
			inBitStream->Read(m_Pos[0].y);
			inBitStream->Read(m_Pos[0].z);

			inBitStream->Read(m_Pos[3].x);
			inBitStream->Read(m_Pos[3].y);
			inBitStream->Read(m_Pos[3].z);

			inBitStream->Read(m_Rotation[0].x);
			inBitStream->Read(m_Rotation[0].y);
			inBitStream->Read(m_Rotation[0].z);
			inBitStream->Read(m_Rotation[0].w);

			inBitStream->Read(m_Rotation[3].x);
			inBitStream->Read(m_Rotation[3].y);
			inBitStream->Read(m_Rotation[3].z);
			inBitStream->Read(m_Rotation[3].w);

			m_DeadCount = 0;
		}
		if(m_DataToReceive & OBJECT_DATA)
		{
			if(m_Owner)
			{
				//move this to update!!!!!!
				ActionHandler* ah = m_Owner->GetActionHandler();
				if(ah)
				{
					//IPlayer *player = ah->GetPlayer();
					bool has_player;
					inBitStream->Read(has_player); 
					if(has_player)
					{
						NetworkID pid;
						inBitStream->Read(pid);
						if(pid != m_ActionHandlerPlayerId)
						{
							m_ActionHandlerPlayerId = pid;
							//get player with corrrect id
							ObjectContainer::BaseObjectVector* vec =  Root::Get().GetPlayerContainer()->GetObjects();
							ObjectContainer::BaseObjectIter iter_player = vec->begin();
							while(iter_player != vec->end())
							{
								IPlayer *player = (IPlayer*) *iter_player;
								RakNetPlayer* rak_player = (RakNetPlayer*) player->GetNetworkObject();
								if(rak_player)
								{
									if(rak_player->GetReplica()->GetNetworkID() == m_ActionHandlerPlayerId)
									{
										m_ActionHandlerPlayer = player;
										player->SetActionHandler(ah);
										ah->SetPlayer(player);	
										break;
									}
								}
								iter_player++;
							}
						}
					}
					else
					{
						m_ActionHandlerPlayer = NULL;
						IPlayer *player = ah->GetPlayer();
						ah->SetPlayer(NULL);
						if(player) player->SetActionHandler(NULL);
					}
				}
			}
		}

		RakNetNetworkManager* raknet = (RakNetNetworkManager*) Root::Get().GetNetworkManager();
		// If this is a server
		if (raknet->IsServer())
		{

			// Synchronisation events should be forwarded to other clients
			raknet->GetReplicaManager()->SignalSerializeNeeded(m_Replica, systemAddress, true);
		}
		//raknet->GetReplicaManager()->SignalSerializeNeeded(this, playerId, true);
		return REPLICA_PROCESSING_DONE;
	}

	// Interface from NetworkIDGenerator.


	bool RakNetBase::IsMaster()
	{
		RakNetNetworkManager* raknet = (RakNetNetworkManager*) Root::Get().GetNetworkManager();
		return raknet->IsMaster(this);
	}					
}
