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

#ifndef RAKNET_NETWORK_SYSTEM_H
#define RAKNET_NETWORK_SYSTEM_H

#include <map>
//Raknet includes
#include "MessageIdentifiers.h"
#include "NetworkIDManager.h"
#include "ReplicaEnums.h"
#include "ReplicaManager.h"
#include "BitStream.h"

//GASS includes
#include "Core/MessageSystem/IMessage.h"
#include "Sim/Scenario/Scene/BaseSceneManager.h"
#include "Sim/Systems/SimSystem.h"
#include "Sim/Systems/SimSystemMessages.h"
#include "Sim/Scheduling/ITaskListener.h"



class ReplicaManager;
class RakPeerInterface;


namespace GASS
{
	#define MAX_PEERS 64
	struct ServerData;


	enum
	{
		ID_START_SCENARIO = ID_USER_PACKET_ENUM+1,
		ID_REMOTE_COMMAND = ID_USER_PACKET_ENUM+2,
		ID_REMOTE_CREATE = ID_USER_PACKET_ENUM+3,
	};


	struct ServerPingReponse
		{
			std::string IP;
			float Time;
			float Ping;
			int Port;
		};

		struct ClientData
		{
			std::string IP;

		};
		typedef std::map<std::string,ServerPingReponse> ServerReponseMap;
		typedef std::map<std::string,ClientData> ClientDataMap;




	class RakNetNetworkSystem  : public Reflection<RakNetNetworkSystem, SimSystem>, ReceiveConstructionInterface, public ITaskListener
	{

	public:
		RakNetNetworkSystem();
		virtual ~RakNetNetworkSystem();
		static void RegisterReflection();
		virtual void OnCreate();

		bool IsServer() {return  m_IsServer;}

		ReplicaManager* GetReplicaManager()const {return m_ReplicaManager;}
		//helpers
		static void WriteString(const std::string &str,RakNet::BitStream *outBitStream);
		static std::string ReadString(RakNet::BitStream *inBitStream);

	private:
		void OnInit(MessagePtr message);
		void OnShutdown(MessagePtr message);
		void OnStartServer(StartServerMessagePtr message);
		void OnStartClient(StartClientMessagePtr message);
		void OnConnectToServer(ConnectToServerMessagePtr message);
		void OnPingRequest(PingRequestMessagePtr message);
		void OnScenarioAboutToLoad(ScenarioAboutToLoadNotifyMessagePtr message);
		//ITaskListener
		void Update(double delta);
		TaskGroup GetTaskGroup() const {return "NETWORK_TASK_GROUP";}
	private:
		//Helpers
		void StartServer(const std::string &name,int port);
		void StartClient(int client_port,int server_port);
		bool ConnectToServer(const std::string &server,int server_port,int client_port);
		ReplicaReturnResult ReceiveConstruction(RakNet::BitStream *inBitStream, RakNetTime timestamp, NetworkID networkID, NetworkIDObject *existingObject, SystemAddress senderId, ReplicaManager *caller);
		void UpdateServer(double delta);
		void UpdateClient(double delta);
		void SerializeServerData(RakNet::BitStream &bstream,ServerData* data);
		void DeserializeServerData(RakNet::BitStream *bstream ,ServerData* data);

	
		ReplicaManager* m_ReplicaManager;
		RakPeerInterface *m_RakPeer;
		NetworkIDManager m_NetworkIDManager;

		int m_IsServer;
		//std::string m_ServerIP;
		int m_ServerPort;
		int m_ClientPort;
		//bool m_UpdateTransformations;
		//float m_TransformationUpdateInterval;
		ServerData *m_ServerData;
		ServerData *m_ServerDataOnClient;
		SystemAddress m_RemoteOwnerId;
		//if true, players will be be created on server but the client is master
		bool m_RemoteCreatePlayers;
		bool m_AcceptLateJoin;
		bool m_ScenarioIsRunning;
		ClientDataMap m_ClientMap;
	};
	typedef boost::shared_ptr<RakNetNetworkSystem> RakNetNetworkSystemPtr;
}

#endif