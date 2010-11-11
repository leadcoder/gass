#ifndef MESSAGES_I
#define MESSAGES_I

%{
#include "Core/MessageSystem/IMessage.h"
#include "Sim/Systems/SimSystemMessages.h"
	%}

typedef int SenderID; 

//class IMessage
//{
//public:
//};

class BaseMessage : public IMessage
{
public:
};

class DebugPrintMessage : public BaseMessage
{
public:
	DebugPrintMessage(const std::string &text, SenderID sender_id = -1, double delay= 0);
	std::string GetText() const;
};


class MessagePtr
{
public:
	MessagePtr(IMessage*);
};

class DebugPrintMessagePtr
{
public:
	DebugPrintMessagePtr(DebugPrintMessage *);

};

class ClientConnectedMessage : public BaseMessage
{
public:
	ClientConnectedMessage(const std::string name, int client_port, SenderID sender_id = -1, double delay= 0);
	std::string GetClientName();
};

class ClientConnectedMessagePtr
{
public:
	ClientConnectedMessagePtr(ClientConnectedMessage *);
};

class StartServerMessage  : public BaseMessage
{
public:
	StartServerMessage(const std::string name, int port, SenderID sender_id = -1, double delay= 0);
	std::string GetServerName() const;
	int GetPort() const;
};

class StartServerMessagePtr
{
public:
	StartServerMessagePtr(StartServerMessage *);
};

class StopServerMessage  : public BaseMessage
{
public:
	StopServerMessage();
};

class StopServerMessagePtr
{
public:
	StopServerMessagePtr(StopServerMessage *);
};

class StartClientMessage  : public BaseMessage
{
public:
	StartClientMessage(const std::string name, int client_port, int server_port, SenderID sender_id = -1, double delay= 0);
	std::string GetClientName() const ;
	int GetClientPort() const ;
	int GetServerPort() const ;
};


class StartClientMessagePtr
{
public:
	StartClientMessagePtr(StartClientMessage *);
};


class StopClientMessage  : public BaseMessage
{
public:
	StopClientMessage();
};

class StopClientMessagePtr
{
public:
	StopClientMessagePtr(StopClientMessage *);
};

class ServerResponseMessage : public BaseMessage
{
public:
	ServerResponseMessage(const std::string server_name, int server_port, float ping_time,SenderID sender_id = -1, double delay= 0);
	std::string GetServerName() const;
	int GetServerPort() const;
	float GetServerPingTime() const;
};

class ServerResponseMessagePtr
{
public:
	ServerResponseMessagePtr(ServerResponseMessage *);
};

class ConnectToServerMessage : public BaseMessage
{
public:
	ConnectToServerMessage(const std::string server_name, int server_port,SenderID sender_id = -1, double delay= 0) ;
	std::string GetServerName() const;
	int GetServerPort() const;
};


class ConnectToServerMessagePtr
{
public:
	ConnectToServerMessagePtr(ConnectToServerMessage *);
};


class ClientDisconnectedMessage : public BaseMessage
{
	public:
		ClientDisconnectedMessage(const std::string name, int client_port, SenderID sender_id = -1, double delay= 0);
    	std::string GetClientName() const;
};

class ClientDisconnectedMessagePtr
{
public:
	ClientDisconnectedMessagePtr(ClientDisconnectedMessage *);
};

class ServerDisconnectedMessage : public BaseMessage
{
	public:
		ServerDisconnectedMessage(const std::string name, int client_port, SenderID sender_id = -1, double delay= 0);
    	std::string GetServerName() const;
};

class ServerDisconnectedMessagePtr
{
public:
	ServerDisconnectedMessagePtr(ServerDisconnectedMessage *);
};

class PingRequestMessage : public BaseMessage
{
public:
	PingRequestMessage(int server_port,SenderID sender_id = -1, double delay= 0);
		int GetServerPort() const;
};

class PingRequestMessagePtr
{
public:
	PingRequestMessagePtr(PingRequestMessage *);
};

class StartSceanrioRequestMessage : public BaseMessage
{
public:
	StartSceanrioRequestMessage(const std::string scenario_name, SenderID sender_id = -1, double delay= 0);
		std::string GetScenarioName() const;
};

class StartSceanrioRequestMessagePtr
{
public:
	StartSceanrioRequestMessagePtr(StartSceanrioRequestMessage *);
};

class ScenarioSceneLoadedNotifyMessage : public BaseMessage
{
public:
	ScenarioSceneLoadedNotifyMessage(ScenarioScenePtr scenario_scene, SenderID sender_id = -1, double delay= 0);
	ScenarioScenePtr GetScenarioScene() const;
};

class ScenarioSceneLoadedNotifyMessagePtr
{
public:
	ScenarioSceneLoadedNotifyMessagePtr(StartSceanrioRequestMessage *);
};


%inline %{
	DebugPrintMessage ToDebugPrintMessage(MessagePtr message)
	{
		DebugPrintMessagePtr db_mess = boost::shared_dynamic_cast<DebugPrintMessage>(message);
		DebugPrintMessage ret = *db_mess.get();
		return ret;
	}
	
	ServerResponseMessage ToServerResponseMessage(MessagePtr message)
	{
		ServerResponseMessagePtr db_mess = boost::shared_dynamic_cast<ServerResponseMessage>(message);
		ServerResponseMessage ret = *db_mess.get();
		return ret;
	}
	
	ClientConnectedMessage ToClientConnectedMessage(MessagePtr message)
	{
		ClientConnectedMessagePtr db_mess = boost::shared_dynamic_cast<ClientConnectedMessage>(message);
		ClientConnectedMessage ret = *db_mess.get();
		return ret;
	}
	
	ClientDisconnectedMessage ToClientDisconnectedMessage(MessagePtr message)
	{
		ClientDisconnectedMessagePtr db_mess = boost::shared_dynamic_cast<ClientDisconnectedMessage>(message);
		ClientDisconnectedMessage ret = *db_mess.get();
		return ret;
	}
	
	
	ServerDisconnectedMessage ToServerDisconnectedMessage(MessagePtr message)
	{
		ServerDisconnectedMessagePtr db_mess = boost::shared_dynamic_cast<ServerDisconnectedMessage>(message);
		ServerDisconnectedMessage ret = *db_mess.get();
		return ret;
	}
	
	ScenarioSceneLoadedNotifyMessage ToScenarioSceneLoadedNotifyMessage(MessagePtr message)
	{
		ScenarioSceneLoadedNotifyMessagePtr db_mess = boost::shared_dynamic_cast<ScenarioSceneLoadedNotifyMessage>(message);
		ScenarioSceneLoadedNotifyMessage ret = *db_mess.get();
		return ret;
	}
	
	%}







#endif