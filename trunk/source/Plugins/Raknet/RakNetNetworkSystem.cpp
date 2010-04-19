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
#include <boost/bind.hpp>
#include "Plugins/RakNet/RakNetNetworkSystem.h"
//#include "Plugins/RakNet/RakNetReplicaMember.h"
//#include "Plugins/RakNet/RakNetBase.h"
#include "Plugins/RakNet/RakNetMasterReplica.h"
#include "Plugins/RakNet/RakNetChildReplica.h"


#include "Plugins/RakNet/RakNetMessages.h"

#include "RakNetworkFactory.h"
#include "RakPeerInterface.h"
#include "ReplicaManager.h"
#include "StringTable.h"
#include "NetworkData.h"
#include "GetTime.h"



#include "Core/Utils/Log.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/System/SystemFactory.h"
#include "Sim/Scenario/Scene/SceneManagerFactory.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Systems/SimSystemManager.h"

#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/SimEngine.h"
#include "Sim/Scheduling/IRuntimeController.h"

#include "Sim/Components/Graphics/Geometry/IMeshComponent.h"


namespace GASS
{

	RakNetNetworkSystem::RakNetNetworkSystem() : m_IsServer(0),
		m_ReplicaManager (new ReplicaManager()),
		m_NetworkIDManager(new NetworkIDManager()),
		m_ServerPort (60005),
		m_ClientPort (60006),
		//m_UseP2P(false),
		m_RakPeer(NULL),
		m_ServerData(new ServerData()),
		m_ServerDataOnClient(new ServerData()),
		m_ScenarioIsRunning (false),
		m_AcceptLateJoin (true),
		m_RemoteCreatePlayers (true),
		m_Active(false)
	{
	
	}

	RakNetNetworkSystem::~RakNetNetworkSystem()
	{
		RakNetworkFactory::DestroyRakPeerInterface(m_RakPeer);
		delete m_ReplicaManager;
		delete m_ServerData;
		delete m_ServerDataOnClient;
		delete m_NetworkIDManager;
	}

	void RakNetNetworkSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("RakNetNetworkSystem",new GASS::Creator<RakNetNetworkSystem, ISystem>);
	}

	void RakNetNetworkSystem::OnCreate()
	{
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(RakNetNetworkSystem::OnInit,InitMessage,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(RakNetNetworkSystem::OnStartServer,StartServerMessage,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(RakNetNetworkSystem::OnStartClient,StartClientMessage,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(RakNetNetworkSystem::OnSceneLoaded,ScenarioSceneAboutToLoadNotifyMessage,0));
	
	}

	void RakNetNetworkSystem::OnSceneLoaded(ScenarioSceneAboutToLoadNotifyMessagePtr message)
	{
		m_Scene = message->GetScenarioScene();
	}

	void RakNetNetworkSystem::OnInit(MessagePtr message)
	{
		m_RakPeer = RakNetworkFactory::GetRakPeerInterface();
		//You have to attach ReplicaManager for it to work, as it is one of the RakNet plugins
		m_RakPeer->AttachPlugin(m_ReplicaManager);

		//RakNet::StringTable::Instance()->AddString("RakNetBase", false); // 2nd parameter of false means a static string so it's not necessary to copy it
		RakNet::StringTable::Instance()->AddString("RakNetMasterReplica", false); // 2nd parameter of false means a static string so it's not necessary to copy it
		RakNet::StringTable::Instance()->AddString("RakNetChildReplica", false); 
	}


	void RakNetNetworkSystem::OnStartServer(StartServerMessagePtr message)
	{
		StartServer(message->GetServerName(),message->GetPort());
	}

	void RakNetNetworkSystem::OnStartClient(StartClientMessagePtr message)
	{
		StartClient(message->GetClientPort(),message->GetServerPort());
	}


	void RakNetNetworkSystem::OnConnectToServer(ConnectToServerMessagePtr message)
	{
		ConnectToServer(message->GetServerName(),message->GetServerPort(),0);
	}

	void RakNetNetworkSystem::OnShutdown(MessagePtr message)
	{
		if(m_RakPeer) 
			m_RakPeer->Shutdown(100, 0);
	}

	void RakNetNetworkSystem::StartServer(const std::string &name,int port)
	{
	    Log::Print("Starting raknet server:%s port:%d",name.c_str(),port);
		m_IsServer = 1;

		
		//Anytime we get a new connection, call AddParticipant() on that connection
		m_ReplicaManager->SetAutoParticipateNewConnections(true);

		// Anytime we get a new participant, automatically call Construct() for them with all known objects
		m_ReplicaManager->SetAutoConstructToNewParticipants(true);

		m_ReplicaManager->SetReceiveConstructionCB(this);
		m_NetworkIDManager->SetIsNetworkIDAuthority(true);
		NetworkID::SetPeerToPeerMode(false);

		m_RakPeer->SetNetworkIDManager(m_NetworkIDManager);
	

		
		// By default all objects are not in scope, meaning we won't serialize the data automatically when they are constructed
		// Calling this eliminates the need to call replicaManager.SetScope(this, true, playerId); in Replica::SendConstruction.
		m_ReplicaManager->SetDefaultScope(true);

	
		SocketDescriptor socketDescriptor(port,0);
		Log::Print("Raknet starup....");
		bool ret = m_RakPeer->Startup(MAX_PEERS,0,&socketDescriptor, 1);
		if(ret == false)
		{
		    Log::Error("Failed to start raknet server");
		}
		Log::Print("Raknet startup done");
		m_RakPeer->SetMaximumIncomingConnections(MAX_PEERS);
		Log::Print("Raknet SetMaximumIncomingConnections done");

		//Register update fucntion
		SimEngine::GetPtr()->GetRuntimeController()->Register(this);

		//Catch scenario load messages
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(RakNetNetworkSystem::OnScenarioAboutToLoad,ScenarioAboutToLoadNotifyMessage,0));
		m_RakPeer->SetOccasionalPing(true);
		m_Active = true;
	}


	void RakNetNetworkSystem::StartClient(int client_port,int server_port)
	{
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(RakNetNetworkSystem::OnConnectToServer,ConnectToServerMessage,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(RakNetNetworkSystem::OnPingRequest,PingRequestMessage,0));
		
		//Anytime we get a new connection, call AddParticipant() on that connection
		m_ReplicaManager->SetAutoParticipateNewConnections(true);

		// Anytime we get a new participant, automatically call Construct() for them with all known objects
		m_ReplicaManager->SetAutoConstructToNewParticipants(true);

		m_ReplicaManager->SetReceiveConstructionCB(this);

		m_NetworkIDManager->SetIsNetworkIDAuthority(false);
		NetworkID::SetPeerToPeerMode(false);

		m_RakPeer->SetNetworkIDManager(m_NetworkIDManager);
		

		// By default all objects are not in scope, meaning we won't serialize the data automatically when they are constructed
		// Calling this eliminates the need to call replicaManager.SetScope(this, true, playerId); in Replica::SendConstruction.
		m_ReplicaManager->SetDefaultScope(true);
		
		SocketDescriptor socketDescriptor(0,0);
		m_RakPeer->Startup(1,0,&socketDescriptor, 1);

		m_RakPeer->Ping("255.255.255.255", server_port, true);
		m_RakPeer->SetOccasionalPing(true);

		//Register update fucntion
		SimEngine::GetPtr()->GetRuntimeController()->Register(this);
		
		m_Active = true;
	}

	void RakNetNetworkSystem::Update(double delta)
	{
		if(IsServer())
		{
			UpdateServer(delta);
		}
		else 
			UpdateClient(delta);
	}

	bool RakNetNetworkSystem::ConnectToServer(const std::string &server,int server_port,int client_port)
	{
		bool connected =  m_RakPeer->Connect(server.c_str(), server_port, 0, 0);
		if(connected)
		{
			// Server data on the client
			/*if (m_RakClient->GetStaticServerData())
			{
				ServerData* data = (ServerData*)m_RakClient->GetStaticServerData()->GetData();
				int da;
				da = 0;
				//DeserializeServerData((char *)m_RakClient->GetStaticServerData()->GetData(),m_ServerDataOnClient);
			}*/
		}
		return connected;
	}

	ReplicaReturnResult RakNetNetworkSystem::ReceiveConstruction(RakNet::BitStream *inBitStream, RakNetTime timestamp, NetworkID networkID, NetworkIDObject *existingObject, SystemAddress senderId, ReplicaManager *caller)
	{
		char output[255];
		
		//RakNetBase *object =NULL;

		// I encoded all the data in inBitStream SendConstruction
		// I am using the string table to send strings, which is a way to send pre-known strings in a single byte.

		// The stringTable system has the limitation that all systems must register all the same strings in the same order.
		// I could have also used stringCompressor, which would always work but is less efficient to use when we have known strings
		RakNet::StringTable::Instance()->DecodeString(output, 255, inBitStream);
		
		/*if (strcmp(output, "RakNetBase")==0)
		{
			printf("replica about to be created!\n");
			RakNetBase* object = new RakNetBase(m_ReplicaManager);
			object->RemoteInit(inBitStream, timestamp, networkID,senderId);
			MessagePtr message( new ReplicaCreatedMessage(object));
			SimEngine::Get().GetSimSystemManager()->PostMessage(message);
			printf("replica created!\n");
		}

		else*/ if (strcmp(output, "RakNetMasterReplica")==0)
		{
			//printf("replica about to be created!\n");
			RakNetMasterReplica* object = new RakNetMasterReplica(m_ReplicaManager);
			object->RemoteInit(inBitStream, timestamp, networkID,senderId);
			MessagePtr message( new MasterReplicaCreatedMessage(object));
			SimEngine::Get().GetSimSystemManager()->PostMessage(message);
			//printf("replica created!\n");
		}
		else if (strcmp(output, "RakNetChildReplica")==0)
		{
			//printf("replica about to be created!\n");
			RakNetChildReplica* object = new RakNetChildReplica(m_ReplicaManager);
			object->RemoteInit(inBitStream, timestamp, networkID,senderId);
			MessagePtr message( new ChildReplicaCreatedMessage(object));
			SimEngine::Get().GetSimSystemManager()->PostMessage(message);
			//printf("replica created!\n");
		}
		return REPLICA_PROCESSING_DONE;
	}

	void RakNetNetworkSystem::UpdateServer(double delta)
	{
		Packet *p;

		p = m_RakPeer->Receive();
		while(p)
		{
			if (p->data[0]==ID_DISCONNECTION_NOTIFICATION || p->data[0]==ID_CONNECTION_LOST)
			{
				//printf("Connection lost to %s:%i\n", rakPeer->PlayerIDToDottedIP(p->systemAddress), p->systemAddress.port);
				// Delete the object owned by this player
				for (int index=0; index < m_ReplicaManager->GetReplicaCount(); index++)
				{

					/*RakNetReplicaMember *rm = (RakNetReplicaMember*) m_ReplicaManager->GetReplicaAtIndex(index);
					RakNetBase *base = (RakNetBase*) rm->GetParent();
					if (base->GetOwnerSystemAddress() == p->systemAddress) //bugg fix this, this only indicates master/slave
					{
						m_ReplicaManager->Destruct(base->GetReplica(), UNASSIGNED_SYSTEM_ADDRESS, true); //Send the destruct message to all
						delete base;
						break;
					}*/
				}

				std::string name = p->systemAddress.ToString();
				ClientDataMap::iterator pos;
				pos = m_ClientMap.find(name);
				if (pos != m_ClientMap.end())
				{
					m_ClientMap.erase(pos);
				}
			}
			else if (p->data[0] == ID_NEW_INCOMING_CONNECTION || p->data[0]==ID_CONNECTION_REQUEST_ACCEPTED)
			{
				//printf("New connection from %s:%i\n", rakPeer->PlayerIDToDottedIP(p->systemAddress), p->systemAddress.port);
				// We need our own unique IP address to go along with the NetworkID.  Our externalPlayerID should be unique.
				// The internalPlayerID, returned by rakPeer->GetLocalIP, won't be unique if we are behind a NAT machine
				//if (NetworkIDGenerator::GetExternalPlayerID()==UNASSIGNED_SYSTEM_ADDRESS)
				//	NetworkIDGenerator::SetExternalPlayerID(((RakPeerInterface*)m_RakServer)->GetExternalID(p->systemAddress));


				ClientData data;
				std::string name = p->systemAddress.ToString();
				int port = p->systemAddress.port;
				data.IP = name;
				m_ClientMap[name] = data;

				MessagePtr message (new ClientConnectedMessage(name,port));
				GetSimSystemManager()->PostMessage(message);


				//printf("new connection\n");

				/*if(m_RemoteCreatePlayers)
				{
					//Remote player!
					m_RemoteOwnerId = p->systemAddress;
					//RemotePlayer* player = 	new RemotePlayer();
					BasePlayer* player = (BasePlayer*) Root::Get().GetBaseObjectTemplateManager()->CreateFromTemplate("DefaultPlayer");
					//DYNAMIC_CAST(LocalPlayer,tempp);

					if(player == NULL )
						Log::Error("DefaultPlayer not found");

					player->SetName(name);
					player->Init();
					//reset
					m_RemoteOwnerId = UNASSIGNED_SYSTEM_ADDRESS;
					Root::Get().GetPlayerContainer()->Add(player);

					//Check if we also should create vehicle
					//player->OnServerCreate();
					//RakNetPlayer* rp = (RakNetPlayer*) player->GetNetworkObject();
				}*/
				//Send client config
				if(m_ScenarioIsRunning && m_AcceptLateJoin)
				{
					RakNet::BitStream out;
					out.Reset();
					out.Write((MessageID)ID_START_SCENARIO);
					SerializeServerData(out,m_ServerData);
					m_RakPeer->Send(&out, HIGH_PRIORITY, RELIABLE_ORDERED,0,p->systemAddress,false);

					//printf("send connection\n");
				}
			}
			else if (p->data[0]==ID_CONNECTION_ATTEMPT_FAILED)
			{
				//printf("Connection attempt to %s:%i failedIBaseSound.hn", rakPeer->PlayerIDToDottedIP(p->systemAddress), p->systemAddress.port);
			}
			/*else if (p->data[0]==ID_REMOTE_COMMAND)
			{
				RakNet::BitStream remote_data(p->data+1,p->length,false);
				ReceiveRemoteCommand(&remote_data,p->systemAddress);
			}*/
			m_RakPeer->DeallocatePacket(p);
			p = m_RakPeer->Receive();
		}
	}

	void RakNetNetworkSystem::OnPingRequest(PingRequestMessagePtr message)
	{
		if(m_RakPeer)
		{
			m_RakPeer->Ping("255.255.255.255", message->GetServerPort(), true);
		}
	}

	void RakNetNetworkSystem::OnScenarioAboutToLoad(ScenarioAboutToLoadNotifyMessagePtr message)
	{
		m_ServerData->MapName =	message->GetScenario()->GetPath();
		
		
		RakNet::BitStream out;
		out.Reset();
		out.Write((unsigned char)ID_START_SCENARIO);
		SerializeServerData(out,m_ServerData);
		//Send server data to all clients
		for (int index=0; index < MAX_PEERS; index++)
		{
			SystemAddress sa=m_RakPeer->GetSystemAddressFromIndex(index);
			if (sa==UNASSIGNED_SYSTEM_ADDRESS)
				break;
			m_RakPeer->Send(&out, HIGH_PRIORITY, RELIABLE_ORDERED,0,sa,false);

			printf("send scenario data");
		}
		m_ScenarioIsRunning = true;
	}


	void RakNetNetworkSystem::UpdateClient(double delta)
	{
		
		Packet *p;
		p = m_RakPeer->Receive();
		while(p)
		{
			unsigned char packetIdentifier = ( unsigned char ) p->data[ 0 ];
			if (p->data[0]==ID_PONG)
			{
				RakNetTime time, dataLength;
				RakNet::BitStream pong( p->data+1, sizeof(RakNetTime), false);
				pong.Read(time);
				dataLength = p->length - sizeof(unsigned char) - sizeof(RakNetTime);
				ServerPingReponse response;
				//response.IP = m_RakPeer->PlayerIDToDottedIP(p->systemAddress);
				response.IP = p->systemAddress.ToString();//binaryAddress;
				response.Port = p->systemAddress.port;
				response.Ping = RakNet::GetTime()-time;
				response.Time = time;
				//printf("Time is %i\n",time);
				//printf("Ping is %i\n", (unsigned int)(RakNet::GetTime()-time));
				//printf("Data is %i bytes longIBaseSound.hn", dataLength);

				//m_ServerMap[response.IP] = response;


				MessagePtr message(new ServerResponseMessage(response.IP, response.Port, response.Ping));
				GetSimSystemManager()->PostMessage(message);
				//printf("Got pong from %s with time %i\n", client->PlayerIDToDottedIP(p->systemAddress), RakNet::GetTime() - time);
			}
			else if(packetIdentifier == ID_START_SCENARIO)
			{
				ServerData data;
				RakNet::BitStream server_data(p->data+1,p->length-1,false);
				DeserializeServerData(&server_data,&data);
				MessagePtr message(new StartSceanrioRequestMessage(data.MapName));
				GetSimSystemManager()->PostMessage(message);

				printf("got connection\n");
				
				//load scenario

				//wait for local player?

		/*		if(m_RemoteCreatePlayers)
				{
					//Wait for player
					while(Root::GetPtr()->GetDefaultLocalPlayer() == NULL)
					{
						CheckIncomingAndOutgoingObjects();
						if(p) m_RakPeer->DeallocatePacket(p);
						p = m_RakPeer->Receive();
					}
				}
				else
				{
					//Root::Get().CreateLocalPlayer("LocalKing");
				}*/
				//Load level
				//Root::GetPtr()->GetLevel()->LoadXML(data.MapName);
			}
		
			m_RakPeer->DeallocatePacket(p);
			p = m_RakPeer->Receive();
		}
	}


	/*INetworkObject* RakNetNetworkSystem::Init(RaknetNetworkComponenPtr object)
	{
		// check if root/to object
		if(object->IsRoot())
		{
			//this should never happen, or?
			if(object->GetNetworkObject()) 
				return object->GetNetworkObject();
			else
			{
				//Create network object for this scene node
				return NewNetworkObject(object);
			}
		}
		else
		{
			//This is a child object
			BaseObject* root = (BaseObject*) object->GetRoot();
			if(root->IsMaster())
			{
				//root object master, create new network object  
				return NewNetworkObject(object);
			}
			else
			{
				//Try to find existing network object for this node
				INetworkObject* network_obj = FindNetworkObject(object);
				network_obj->SetOwner(object);
				return network_obj;
			}
		}
	}*/


	/*RakNetBase* RakNetNetworkSystem::NewNetworkObject(RaknetNetworkComponenPtr object)
	{
		RakNetBase* net_obj = NULL;
		if(object->IsKindOf(&BasePlayer::m_RTTI))
		{
			net_obj = new RakNetPlayer();
		}
		else if(object->IsKindOf(&BaseObject::m_RTTI))
		{
			net_obj = new RakNetBase();
		}
		if(net_obj)
		{
			//Check if we want other owner then the server, only valid for players?
			if(net_obj->AllowRemoteOwner() && m_RemoteOwnerId != UNASSIGNED_SYSTEM_ADDRESS)
			{
				net_obj->SetOwnerSystemAddress(m_RemoteOwnerId);
			}
			else
			{
				net_obj->SetOwnerSystemAddress(m_RakPeer->GetInternalID());
			}
			net_obj->LocalInit(object, this);
		}
		return net_obj;
	}*/


	RakNetChildReplica* RakNetNetworkSystem::FindReplica(const NetworkID &part_of_network_id,int part_id)
	{
		//Find replica object
		for (int index=0; index < m_ReplicaManager->GetReplicaCount(); index++)
		{
			RakNetChildReplica *rep = (RakNetChildReplica*) dynamic_cast<RakNetChildReplica*>(m_ReplicaManager->GetReplicaAtIndex(index));
			if(rep)
			{
				if(rep->GetPartOfId() == part_of_network_id) //we are part of this object
				{
					if(rep->GetPartId() == part_id) //check that if right child
					{
						return rep; //Match!
					}
				}
			}
		}
		return NULL;
	}

	void RakNetNetworkSystem::WriteString(const std::string &str,RakNet::BitStream *outBitStream)
	{
		int str_size = str.length();
		outBitStream->Write(str_size);
		if	(str_size > 0)
		{
			outBitStream->Write(str.c_str(),str_size);
		}
	}

	std::string RakNetNetworkSystem::ReadString(RakNet::BitStream *inBitStream)
	{
		std::string final = "";
		int str_size;
		inBitStream->Read(str_size);
		if(str_size >0)
		{
			char* inString = new char[ str_size + 1 ];
			inBitStream->Read(inString, str_size);
			inString[ str_size] = '\0';
			final = inString;
			delete[] inString;
		}
		return final;
	}


	void RakNetNetworkSystem::SerializeServerData(RakNet::BitStream &bstream,ServerData* data)
	{
		WriteString(data->ServerName,&bstream);
		WriteString(data->MapName,&bstream);

	}
	void RakNetNetworkSystem::DeserializeServerData(RakNet::BitStream *bstream ,ServerData* data)
	{
		data->ServerName = RakNetNetworkSystem::ReadString(bstream);
		data->MapName = RakNetNetworkSystem::ReadString(bstream);

	}



}
