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

#include "Plugins/RakNet/RakNetNetworkSystem.h"
#include "Plugins/RakNet/RakNetMasterReplica.h"
#include "Plugins/RakNet/RakNetChildReplica.h"
#include "Plugins/RakNet/RakNetInputTransferComponent.h"
#include "Plugins/RakNet/RakNetNetworkSceneManager.h"
#include "Plugins/RakNet/RakNetMessages.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSException.h"
#include "Sim/GASSSystemFactory.h"
#include "Sim/GASSSceneManagerFactory.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"
#include "NetworkData.h"

namespace GASS
{
	RakNetNetworkSystem::RakNetNetworkSystem(SimSystemManagerWeakPtr manager) : Reflection(manager), 
		m_IsServer(0),
		m_ReplicaManager (new ReplicaManager()),
		m_NetworkIDManager(new NetworkIDManager()),
		m_ServerPort (60005),
		m_ClientPort (60006),
		m_RakPeer(nullptr),
		m_ServerData(new ServerData()),
		m_ServerDataOnClient(new ServerData()),
		m_SceneIsRunning (false),
		m_AcceptLateJoin (true),
		m_RemoteCreatePlayers (true),
		m_Active(false),
		m_InterpolationLag(100.0), //100ms , this should be based on ping time
		m_LocationSendFrequency(20),  //20 Hz
		m_SleepTime(0),
		m_RelayInputOnServer(false),
		m_Debug(false)
	{
		m_UpdateGroup = UGID_SIM;
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
		SystemFactory::GetPtr()->Register<RakNetNetworkSystem>("RakNetNetworkSystem");
		RegisterGetSet("InterpolationLag", &RakNetNetworkSystem::GetInterpolationLag, &RakNetNetworkSystem::SetInterpolationLag);
		RegisterGetSet("LocationSendFrequency", &RakNetNetworkSystem::GetLocationSendFrequency, &RakNetNetworkSystem::SetLocationSendFrequency);
		RegisterMember("SleepTime", &RakNetNetworkSystem::m_SleepTime);
		RegisterMember("Debug", &RakNetNetworkSystem::m_Debug);
		RegisterMember("RelayInputOnServer", &RakNetNetworkSystem::m_RelayInputOnServer);
	}

	void RakNetNetworkSystem::OnSystemInit()
	{
		//Only register scene manager if system is created
		SceneManagerFactory::GetPtr()->Register<RaknetNetworkSceneManager>("NetworkSceneManager");

		GetSimSystemManager()->RegisterForMessage(REG_TMESS(RakNetNetworkSystem::OnSceneAboutToLoad, PostSceneLoadEvent,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(RakNetNetworkSystem::OnStartServer,StartServerRequest,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(RakNetNetworkSystem::OnStopServer,StopServerRequest,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(RakNetNetworkSystem::OnStopClient,StopClientRequest,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(RakNetNetworkSystem::OnStartClient,StartClientRequest,0));

		m_RakPeer = RakNetworkFactory::GetRakPeerInterface();
		//You have to attach ReplicaManager for it to work, as it is one of the RakNet plugins
		m_RakPeer->AttachPlugin(m_ReplicaManager);
		m_RakPeer->AttachPlugin(&m_AutoRPC);
		//m_RakPeer->AttachPlugin(&m_Logger);

		//RakNet::StringTable::Instance()->AddString("RakNetBase", false); // 2nd parameter of false means a static string so it's not necessary to copy it
		RakNet::StringTable::Instance()->AddString("RakNetMasterReplica", false); // 2nd parameter of false means a static string so it's not necessary to copy it
		RakNet::StringTable::Instance()->AddString("RakNetChildReplica", false);

		//register RPC
		ARPC_REGISTER_CPP_FUNCTION3(GetRPC(), "RakNetBaseReplica::RemoteMessage", int, RakNetBaseReplica, RemoteMessage, const char *client_address, const char *message, RakNet::AutoRPC* networkCaller);
		ARPC_REGISTER_CPP_FUNCTION4(GetRPC(), "RakNetBaseReplica::RemoteInput", int, RakNetBaseReplica, RemoteInput, SystemAddress input_source, int controller, float value,RakNet::AutoRPC* networkCaller);
		ARPC_REGISTER_CPP_FUNCTION3(GetRPC(), "RakNetBaseReplica::RemoteMessageWithData", int, RakNetBaseReplica, RemoteMessageWithData, const char *message, const char *data, RakNet::AutoRPC* networkCaller);
	}

	void RakNetNetworkSystem::OnTimeOfDay(TimeOfDayRequestPtr message)
	{
		if(m_IsServer && m_RakPeer)
		{
			RakNet::BitStream out;
			out.Reset();

			out.Write((MessageID)ID_TIME_OF_DAY);
			out.Write(message->GetTime());
			out.Write(message->GetSpeed());
			out.Write(message->GetSunRise());
			out.Write(message->GetSunSet());
			m_RakPeer->Send(&out, HIGH_PRIORITY, RELIABLE_ORDERED,0,UNASSIGNED_SYSTEM_ADDRESS,true);
		}
	}

	void RakNetNetworkSystem::OnWeatherRequest(WeatherRequestPtr message)
	{
		if(m_IsServer && m_RakPeer)
		{
			RakNet::BitStream out;
			out.Reset();
			out.Write((MessageID)ID_WEATHER);
			out.Write(message->GetFogDistance());
			out.Write(message->GetFogDensity());
			out.Write(message->GetClouds());
			m_RakPeer->Send(&out, HIGH_PRIORITY, RELIABLE_ORDERED,0,UNASSIGNED_SYSTEM_ADDRESS,true);
		}
	}

	void RakNetNetworkSystem::OnStartServer(StartServerRequestPtr message)
	{
		StartServer(message->GetServerName(),message->GetPort());
	}

	void RakNetNetworkSystem::OnStopServer(StopServerRequestPtr /*message*/)
	{
		Stop();
	}

	void RakNetNetworkSystem::OnStopClient(StopClientRequestPtr /*message*/)
	{
		if(m_Active)
		{
			GetSimSystemManager()->UnregisterForMessage(UNREG_TMESS(RakNetNetworkSystem::OnConnectToServer,ConnectToServerRequest));
			GetSimSystemManager()->UnregisterForMessage(UNREG_TMESS(RakNetNetworkSystem::OnPingRequest,PingRequest));
		}
		if(m_RakPeer)
			m_RakPeer->Shutdown(100, 0);
		m_Active = false;
	}

	void RakNetNetworkSystem::OnStartClient(StartClientRequestPtr message)
	{
		StartClient(message->GetClientPort(),message->GetServerPort());
	}

	void RakNetNetworkSystem::OnConnectToServer(ConnectToServerRequestPtr message)
	{
		ConnectToServer(message->GetServerName(),message->GetServerPort(),0);
	}

	void RakNetNetworkSystem::OnSystemShutdown()
	{
		Stop();
	}

	void RakNetNetworkSystem::Stop()
	{
		if(m_RakPeer)
			m_RakPeer->Shutdown(100, 0);
		if(m_Active)
		{

		}
		m_Active=false;
	}

	void RakNetNetworkSystem::StartServer(const std::string &name,int port)
	{
		if(m_Active)
		{
			GASS_LOG(LWARNING) << "Server already started";
			return;
		}
		GASS_LOG(LINFO) << "Starting Raknet server:" << name << "port:" << port;
		m_IsServer = 1;


		//Anytime we get a new connection, call AddParticipant() on that connection
		m_ReplicaManager->SetAutoParticipateNewConnections(true);

		// Anytime we get a new participant, automatically call Construct() for them with all known objects
		m_ReplicaManager->SetAutoConstructToNewParticipants(true);

		m_ReplicaManager->SetReceiveConstructionCB(this);
		m_NetworkIDManager->SetIsNetworkIDAuthority(true);
		NetworkID::SetPeerToPeerMode(false);

		m_RakPeer->SetNetworkIDManager(m_NetworkIDManager);

		m_AutoRPC.SetNetworkIDManager(m_NetworkIDManager);



		// By default all objects are not in scope, meaning we won't serialize the data automatically when they are constructed
		// Calling this eliminates the need to call replicaManager.SetScope(this, true, playerId); in Replica::SendConstruction.
		m_ReplicaManager->SetDefaultScope(true);
		SocketDescriptor socket_descriptor(static_cast<unsigned short>(port),nullptr);
		GASS_LOG(LINFO) << "Raknet starup....";
		bool ret = m_RakPeer->Startup(MAX_PEERS,static_cast<int>(m_SleepTime),&socket_descriptor, 1);
		if(ret == false)
		{
			GASS_EXCEPT(Exception::ERR_INTERNAL_ERROR,"Failed to start raknet server","RakNetNetworkSystem::StartServer");
		}
		GASS_LOG(LINFO) << "Raknet startup done";
		m_RakPeer->SetMaximumIncomingConnections(MAX_PEERS);
		GASS_LOG(LINFO) << "Raknet SetMaximumIncomingConnections done";
	
		m_RakPeer->SetOccasionalPing(true);
		m_Active = true;
	}


	void RakNetNetworkSystem::StartClient(int /*client_port*/,int server_port)
	{
		m_IsServer = 0;
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(RakNetNetworkSystem::OnConnectToServer,ConnectToServerRequest,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(RakNetNetworkSystem::OnPingRequest,PingRequest,0));

		//Anytime we get a new connection, call AddParticipant() on that connection
		m_ReplicaManager->SetAutoParticipateNewConnections(true);

		// Anytime we get a new participant, automatically call Construct() for them with all known objects
		m_ReplicaManager->SetAutoConstructToNewParticipants(true);

		m_ReplicaManager->SetReceiveConstructionCB(this);

		m_NetworkIDManager->SetIsNetworkIDAuthority(false);
		NetworkID::SetPeerToPeerMode(false);

		m_RakPeer->SetNetworkIDManager(m_NetworkIDManager);

		m_AutoRPC.SetNetworkIDManager(m_NetworkIDManager);


		// By default all objects are not in scope, meaning we won't serialize the data automatically when they are constructed
		// Calling this eliminates the need to call replicaManager.SetScope(this, true, playerId); in Replica::SendConstruction.
		m_ReplicaManager->SetDefaultScope(true);

		SocketDescriptor socket_descriptor(0,nullptr);
		m_RakPeer->Startup(1,static_cast<int>(m_SleepTime),&socket_descriptor, 1);

		m_RakPeer->Ping("255.255.255.255", static_cast<unsigned short>(server_port), true);
		m_RakPeer->SetOccasionalPing(true);

		m_Active = true;
	}

	void RakNetNetworkSystem::OnSystemUpdate(double delta)
	{
		if(IsServer())
		{
			UpdateServer(delta);
		}
		else
			UpdateClient(delta);

		SimEngine::Get().GetSimSystemManager()->SendImmediate(SystemMessagePtr(new NetworkPostUpdateEvent()));
	}

	bool RakNetNetworkSystem::ConnectToServer(const std::string &server,int server_port,int /*client_port*/)
	{
		bool connected =  m_RakPeer->Connect(server.c_str(), static_cast<unsigned short>(server_port), nullptr, 0);
		return connected;
	}

	ReplicaReturnResult RakNetNetworkSystem::ReceiveConstruction(RakNet::BitStream *inBitStream, RakNetTime timestamp, NetworkID networkID, NetworkIDObject * /*existingObject*/, SystemAddress senderId, ReplicaManager * /*caller*/)
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
			auto* object = new RakNetMasterReplica(m_ReplicaManager);
			object->RemoteInit(inBitStream, timestamp, networkID,senderId);
			SystemMessagePtr message( new MasterReplicaCreatedEvent(object));
			SimEngine::Get().GetSimSystemManager()->PostMessage(message);
			//printf("replica created!\n");
		}
		else if (strcmp(output, "RakNetChildReplica")==0)
		{
			//printf("replica about to be created!\n");
			auto* object = new RakNetChildReplica(m_ReplicaManager);
			object->RemoteInit(inBitStream, timestamp, networkID,senderId);
			SystemMessagePtr message( new ChildReplicaCreatedEvent(object));
			SimEngine::Get().GetSimSystemManager()->PostMessage(message);
			//printf("replica created!\n");
		}
		return REPLICA_PROCESSING_DONE;
	}

	void RakNetNetworkSystem::UpdateServer(double /*delta*/)
	{
		Packet *p;

		p = m_RakPeer->Receive();
		while(p)
		{
			if (p->data[0]==ID_DISCONNECTION_NOTIFICATION || p->data[0]==ID_CONNECTION_LOST)
			{
				//printf("Connection lost to %s:%i\n", rakPeer->PlayerIDToDottedIP(p->systemAddress), p->systemAddress.port);
				// Delete the object owned by this player
				//for (int index=0; index < m_ReplicaManager->GetReplicaCount(); index++)
				//{

					/*RakNetReplicaMember *rm = (RakNetReplicaMember*) m_ReplicaManager->GetReplicaAtIndex(index);
					RakNetBase *base = (RakNetBase*) rm->GetParent();
					if (base->GetOwnerSystemAddress() == p->systemAddress) //bugg fix this, this only indicates master/slave
					{
					m_ReplicaManager->Destruct(base->GetReplica(), UNASSIGNED_SYSTEM_ADDRESS, true); //Send the destruct message to all
					delete base;
					break;
					}*/
				//}

				std::string name = p->systemAddress.ToString();//GetRakPeer()->GetExternalID(p->systemAddress).ToString();
				int port = p->systemAddress.port;
				ClientDataMap::iterator pos;
				pos = m_ClientMap.find(name);
				if (pos != m_ClientMap.end())
				{
					m_ClientMap.erase(pos);
				}
				SystemMessagePtr message (new ClientDisconnectedEvent(name,port));
				GetSimSystemManager()->PostMessage(message);
			}
			else if (p->data[0] == ID_NEW_INCOMING_CONNECTION || p->data[0]==ID_CONNECTION_REQUEST_ACCEPTED)
			{
				//printf("New connection from %s:%i\n", rakPeer->PlayerIDToDottedIP(p->systemAddress), p->systemAddress.port);
				// We need our own unique IP address to go along with the NetworkID.  Our externalPlayerID should be unique.
				// The internalPlayerID, returned by rakPeer->GetLocalIP, won't be unique if we are behind a NAT machine
				//if (NetworkIDGenerator::GetExternalPlayerID()==UNASSIGNED_SYSTEM_ADDRESS)
				//	NetworkIDGenerator::SetExternalPlayerID(((RakPeerInterface*)m_RakServer)->GetExternalID(p->systemAddress));


				ClientData data;
				std::string name = p->systemAddress.ToString();//GetRakPeer()->GetExternalID(p->systemAddress).ToString();
				int port = p->systemAddress.port;
				data.IP = name;
				m_ClientMap[name] = data;

				SystemMessagePtr message (new ClientConnectedEvent(name,port));
				GetSimSystemManager()->PostMessage(message);

				//Send client config
				if(m_SceneIsRunning && m_AcceptLateJoin)
				{
					RakNet::BitStream out;
					out.Reset();
					out.Write((MessageID)ID_START_SCENE);
					SerializeServerData(out,m_ServerData);
					m_RakPeer->Send(&out, HIGH_PRIORITY, RELIABLE_ORDERED,0,p->systemAddress,false);
					std::cout << "send server data" << std::endl;
					//printf("send connection\n");
				}
			}
			else if (p->data[0]==ID_CONNECTION_ATTEMPT_FAILED)
			{
				//printf("Connection attempt to %s:%i failedIBaseSound.hn", rakPeer->PlayerIDToDottedIP(p->systemAddress), p->systemAddress.port);
			}
			else if(p->data[0]==ID_RPC_REMOTE_ERROR)
			{
				// Receipient system returned an error
				switch (p->data[1])
				{
				case RakNet::RPC_ERROR_NETWORK_ID_MANAGER_UNAVAILABLE:
					std::cout << ("RPC_ERROR_NETWORK_ID_MANAGER_UNAVAILABLE\n")<< std::endl;
					break;
				case RakNet::RPC_ERROR_OBJECT_DOES_NOT_EXIST:
					std::cout << ("RPC_ERROR_OBJECT_DOES_NOT_EXIST\n")<< std::endl;
					break;
				case RakNet::RPC_ERROR_FUNCTION_INDEX_OUT_OF_RANGE:
					std::cout << ("RPC_ERROR_FUNCTION_INDEX_OUT_OF_RANGE\n")<< std::endl;
					break;
				case RakNet::RPC_ERROR_FUNCTION_NOT_REGISTERED:
					std::cout << ("RPC_ERROR_FUNCTION_NOT_REGISTERED\n")<< std::endl;
					break;
				case RakNet::RPC_ERROR_FUNCTION_NO_LONGER_REGISTERED:
					std::cout << ("RPC_ERROR_FUNCTION_NO_LONGER_REGISTERED\n")<< std::endl;
					break;
				case RakNet::RPC_ERROR_CALLING_CPP_AS_C:
					std::cout << ("RPC_ERROR_CALLING_CPP_AS_C\n")<< std::endl;
					break;
				case RakNet::RPC_ERROR_CALLING_C_AS_CPP:
					std::cout << ("RPC_ERROR_CALLING_C_AS_CPP\n")<< std::endl;
					break;
				case RakNet::RPC_ERROR_STACK_TOO_SMALL:
					std::cout << ("RPC_ERROR_STACK_TOO_SMALL\n")<< std::endl;
					break;
				case RakNet::RPC_ERROR_STACK_DESERIALIZATION_FAILED:
					std::cout << ("RPC_ERROR_STACK_DESERIALIZATION_FAILED\n") << std::endl;
					break;

				}
			}

			m_RakPeer->DeallocatePacket(p);
			p = m_RakPeer->Receive();
		}
	}

	void RakNetNetworkSystem::OnPingRequest(PingRequestPtr message)
	{
		if(m_RakPeer)
		{
			m_RakPeer->Ping("255.255.255.255", static_cast<unsigned short>(message->GetServerPort()), true);
		}
	}

	void RakNetNetworkSystem::OnSceneAboutToLoad(PostSceneLoadEventPtr message)
	{
		m_Scene = message->GetScene();
		message->GetScene()->RegisterForMessage(REG_TMESS(RakNetNetworkSystem::OnTimeOfDay,TimeOfDayRequest,0));
		message->GetScene()->RegisterForMessage(REG_TMESS(RakNetNetworkSystem::OnWeatherRequest,WeatherRequest,0));

		m_ServerData->MapName =	message->GetScene()->GetSceneFile().GetRawPath();
		//std::cout << "Map to send:" << m_ServerData->MapName << std::endl;

		m_SceneIsRunning = true;
		if(m_Active && m_IsServer)
		{
			//std::cout << "scene about to load" << std::endl;
			RakNet::BitStream out;
			out.Reset();
			out.Write((unsigned char)ID_START_SCENE);
			SerializeServerData(out,m_ServerData);
			//Send server data to all clients
			for (int index=0; index < MAX_PEERS; index++)
			{
				SystemAddress sa=m_RakPeer->GetSystemAddressFromIndex(index);
				if (sa==UNASSIGNED_SYSTEM_ADDRESS)
					break;
				m_RakPeer->Send(&out, HIGH_PRIORITY, RELIABLE_ORDERED,0,sa,false);

				std::cout << "Send scene data" << std::endl;
			}
		}
	}

	void RakNetNetworkSystem::UpdateClient(double /*delta*/)
	{
		Packet *p;
		p = m_RakPeer->Receive();
		while(p)
		{
			auto packet_identifier = ( unsigned char ) p->data[ 0 ];
			if (p->data[0]==ID_PONG)
			{
				RakNetTime time;//, dataLength;
				RakNet::BitStream pong( p->data+1, sizeof(RakNetTime), false);
				pong.Read(time);
				//dataLength = p->length - sizeof(unsigned char) - sizeof(RakNetTime);
				ServerPingReponse response;
				//response.IP = m_RakPeer->PlayerIDToDottedIP(p->systemAddress);
				response.IP = p->systemAddress.ToString();//binaryAddress;
				response.Port = p->systemAddress.port;
				response.Ping = static_cast<float>(RakNet::GetTime() - time);
				response.Time = static_cast<float>(time);
				//printf("Time is %i\n",time);
				//printf("Ping is %i\n", (unsigned int)(RakNet::GetTime()-time));
				//printf("Data is %i bytes longIBaseSound.hn", dataLength);
				//m_ServerMap[response.IP] = response;
				SystemMessagePtr message(new ServerResponseEvent(response.IP, response.Port, response.Ping));
				GetSimSystemManager()->PostMessage(message);
				GASS_PRINT("Ping time:" << response.Ping << " Time:" << response.Time << "\n")
				//printf("Got pong from %s with time %i\n", client->PlayerIDToDottedIP(p->systemAddress), RakNet::GetTime() - time);
			}
			else if(packet_identifier == ID_START_SCENE)
			{
				ServerData data;
				RakNet::BitStream server_data(p->data+1,p->length-1,false);
				DeserializeServerData(&server_data,&data);
				SystemMessagePtr message(new LoadSceneRequest(data.MapName));
				GetSimSystemManager()->PostMessage(message);

				std::cout << "ID_START_SCENE" << std::endl;

				//load scene

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
			else if (p->data[0]==ID_DISCONNECTION_NOTIFICATION || p->data[0]==ID_CONNECTION_LOST)
			{
				std::string name = p->systemAddress.ToString();
				int port = p->systemAddress.port;
				SystemMessagePtr message (new ServerDisconnectedEvent(name,port));
				GetSimSystemManager()->PostMessage(message);
			}
			else if (p->data[0]==ID_TIME_OF_DAY)
			{

				RakNet::BitStream bs(p->data+1,p->length-1,false);
				double time,speed,rise,set;
				bs.Read(time);
				bs.Read(speed);
				bs.Read(rise);
				bs.Read(set);
				SceneMessagePtr message(new TimeOfDayRequest(time,set,rise,speed));

				ScenePtr scene = GetScene();
				if(scene)
				{
					scene->PostMessage(message);
				}
			}
			else if (p->data[0]==ID_WEATHER)
			{

				RakNet::BitStream bs(p->data+1,p->length-1,false);
				float fog_dist,fog_density,cloud_factor;
				bs.Read(fog_dist);
				bs.Read(fog_density);
				bs.Read(cloud_factor);
				SceneMessagePtr message(new WeatherRequest(fog_dist,fog_density,cloud_factor));

				ScenePtr scene = GetScene();
				if(scene)
				{
					scene->PostMessage(message);
				}
			}
			else if(p->data[0]==ID_RPC_REMOTE_ERROR)
			{
				// Receipient system returned an error
				switch (p->data[1])
				{
				case RakNet::RPC_ERROR_NETWORK_ID_MANAGER_UNAVAILABLE:
					std::cout << ("RPC_ERROR_NETWORK_ID_MANAGER_UNAVAILABLE\n")<< std::endl;
					break;
				case RakNet::RPC_ERROR_OBJECT_DOES_NOT_EXIST:
					std::cout << ("RPC_ERROR_OBJECT_DOES_NOT_EXIST\n")<< std::endl;
					break;
				case RakNet::RPC_ERROR_FUNCTION_INDEX_OUT_OF_RANGE:
					std::cout << ("RPC_ERROR_FUNCTION_INDEX_OUT_OF_RANGE\n")<< std::endl;
					break;
				case RakNet::RPC_ERROR_FUNCTION_NOT_REGISTERED:
					std::cout << ("RPC_ERROR_FUNCTION_NOT_REGISTERED\n")<< std::endl;
					break;
				case RakNet::RPC_ERROR_FUNCTION_NO_LONGER_REGISTERED:
					std::cout << ("RPC_ERROR_FUNCTION_NO_LONGER_REGISTERED\n")<< std::endl;
					break;
				case RakNet::RPC_ERROR_CALLING_CPP_AS_C:
					std::cout << ("RPC_ERROR_CALLING_CPP_AS_C\n")<< std::endl;
					break;
				case RakNet::RPC_ERROR_CALLING_C_AS_CPP:
					std::cout << ("RPC_ERROR_CALLING_C_AS_CPP\n")<< std::endl;
					break;
				case RakNet::RPC_ERROR_STACK_TOO_SMALL:
					std::cout << ("RPC_ERROR_STACK_TOO_SMALL\n")<< std::endl;
					break;
				case RakNet::RPC_ERROR_STACK_DESERIALIZATION_FAILED:
					std::cout << ("RPC_ERROR_STACK_DESERIALIZATION_FAILED\n") << std::endl;
					break;

				}
			}


			m_RakPeer->DeallocatePacket(p);
			p = m_RakPeer->Receive();
		}
	}

	RakNetChildReplica* RakNetNetworkSystem::FindReplica(const NetworkID &part_of_network_id,int part_id) const
	{
		//Find replica object
		for (unsigned int index=0; index < m_ReplicaManager->GetReplicaCount(); index++)
		{
			auto *rep = (RakNetChildReplica*) dynamic_cast<RakNetChildReplica*>(m_ReplicaManager->GetReplicaAtIndex(index));
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
		return nullptr;
	}

	void RakNetNetworkSystem::WriteString(const std::string &str,RakNet::BitStream *outBitStream)
	{
		int str_size = static_cast<int>(str.length());
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
			char* in_string = new char[ str_size + 1 ];
			inBitStream->Read(in_string, str_size);
			in_string[ str_size] = '\0';
			final = in_string;
			delete[] in_string;
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
