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

#ifndef NETWORK_SYSTEM_MESSAGES_H
#define NETWORK_SYSTEM_MESSAGES_H

#include "Sim/GASSCommon.h"
#include "Core/MessageSystem/GASSBaseMessage.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Math/GASSVector.h"
#include "Core/Math/GASSQuaternion.h"
#include "Sim/Messages/GASSCoreSystemMessages.h"
#include <string>

namespace GASS
{
	class Scene;
	class Scene;
	class SceneObject;

	typedef boost::shared_ptr<Scene> ScenePtr;
	typedef boost::shared_ptr<Scene> ScenePtr;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;

	/**
	Start network server
	*/

	class StartServerRequest : public SystemRequestMessage
	{
	public:
		StartServerRequest(const std::string &name, int port, SenderID sender_id = -1, double delay= 0) :
		  SystemRequestMessage(sender_id , delay) ,
			  m_Name(name), m_Port(port){}

		  std::string GetServerName() const {return m_Name;}
		  int GetPort() const {return m_Port;}
	private:
		std::string m_Name;
		int m_Port;
	};
	typedef boost::shared_ptr<StartServerRequest> StartServerRequestPtr;


	/**
	Stop network server
	*/
	class StopServerRequest : public BaseMessage
	{
	public:
		StopServerRequest(SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay){}
	private:
	};
	typedef boost::shared_ptr<StopServerRequest> StopServerRequestPtr;



	/**
	Client connected to network server
	*/
	class ClientConnectedEvent : public SystemEventMessage
	{
	public:
		ClientConnectedEvent(const std::string &name, int client_port, SenderID sender_id = -1, double delay= 0) :
		  SystemEventMessage(sender_id , delay) ,
			  m_Name(name), m_ClientPort(client_port){}

		 std::string GetClientName() const {return m_Name;}
	private:
		std::string m_Name;
		int m_ClientPort;
	};
	typedef boost::shared_ptr<ClientConnectedEvent> ClientConnectedEventPtr;


	/**
	Client disconnected from network server
	*/
	class ClientDisconnectedEvent : public SystemEventMessage
	{
	public:
		ClientDisconnectedEvent(const std::string &name, int client_port, SenderID sender_id = -1, double delay= 0) :
		  SystemEventMessage(sender_id , delay) ,
			  m_Name(name), m_ClientPort(client_port){}

		 std::string GetClientName() const {return m_Name;}
	private:
		std::string m_Name;
		int m_ClientPort;
	};
	typedef boost::shared_ptr<ClientDisconnectedEvent> ClientDisconnectedEventPtr;


	/////////NETWORK CLIENT MESSAGES////////////////
	/**
	Start network client
	*/
	class StartClientRequest : public SystemRequestMessage
	{
	public:
		StartClientRequest(const std::string &name, int client_port, int server_port, SenderID sender_id = -1, double delay= 0) :
		  SystemRequestMessage(sender_id , delay) ,
			  m_Name(name), m_ClientPort(client_port), m_ServerPort(server_port){}

		 std::string GetClientName() const {return m_Name;}
		 int GetClientPort() const {return m_ClientPort;}
		 int GetServerPort() const {return m_ServerPort;}
	private:
		std::string m_Name;
		int m_ClientPort;
		int m_ServerPort;
	};
	typedef boost::shared_ptr<StartClientRequest> StartClientRequestPtr;


	/**
	Server disconnected
	*/
	class ServerDisconnectedEvent : public SystemEventMessage
	{
	public:
		ServerDisconnectedEvent(const std::string &name, int port, SenderID sender_id = -1, double delay= 0) :
		  SystemEventMessage(sender_id , delay) ,
			  m_Name(name), m_Port(port){}
		 std::string GetServerName() const {return m_Name;}
	private:
		std::string m_Name;
		int m_Port;
	};
	typedef boost::shared_ptr<ServerDisconnectedEvent> ServerDisconnectedEventPtr;

	/**
	Stop network client
	*/
	class StopClientMessage : public BaseMessage
	{
	public:
		StopClientMessage(SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay){}
	private:
	};
	typedef boost::shared_ptr<StopClientMessage> StopClientMessagePtr;


	/**
	Server response to network client
	*/
	class ServerResponseEvent : public SystemEventMessage
	{
	public:
		ServerResponseEvent(const std::string &server_name, int server_port, float ping_time,SenderID sender_id = -1, double delay= 0) :
		  SystemEventMessage(sender_id , delay) ,
			  m_Name(server_name), m_ServerPort(server_port), m_PingTime(ping_time){}

		 std::string GetServerName() const {return m_Name;}
		 int GetServerPort() const {return m_ServerPort;}
		 float GetServerPingTime() const {return m_PingTime;}
	private:
		std::string m_Name;
		int m_ServerPort;
		float m_PingTime;
	};
	typedef boost::shared_ptr<ServerResponseEvent> ServerResponseEventPtr;


	/**
	Connect to server
	*/
	class ConnectToServerRequest : public SystemRequestMessage
	{
	public:
		ConnectToServerRequest(const std::string &server_name, int server_port,SenderID sender_id = -1, double delay= 0) :
		  SystemRequestMessage(sender_id , delay) ,
			  m_Name(server_name), m_ServerPort(server_port){}

		 std::string GetServerName() const {return m_Name;}
		 int GetServerPort() const {return m_ServerPort;}
	private:
		std::string m_Name;
		int m_ServerPort;
	};
	typedef boost::shared_ptr<ConnectToServerRequest> ConnectToServerRequestPtr;

	/**
	Ping for servers
	*/
	class PingRequest : public SystemRequestMessage
	{
	public:
		PingRequest(int server_port,SenderID sender_id = -1, double delay= 0) :
		  SystemRequestMessage(sender_id , delay) ,
			  m_ServerPort(server_port){}
		 int GetServerPort() const {return m_ServerPort;}
	private:
		int m_ServerPort;
	};
	typedef boost::shared_ptr<PingRequest> PingRequestPtr;


	/**
	Server inform client about scene
	*/
	class StartSceanrioRequest : public SystemRequestMessage
	{
	public:
		StartSceanrioRequest(const std::string &scene_name, SenderID sender_id = -1, double delay= 0) :
		  SystemRequestMessage(sender_id , delay) ,
			  m_Name(scene_name){}

		 std::string GetSceneName() const {return m_Name;}
	private:
		std::string m_Name;
	};
	typedef boost::shared_ptr<StartSceanrioRequest> StartSceanrioRequestPtr;

	class ShutdownSceanrioRequestMessage : public BaseMessage
	{
	public:
		ShutdownSceanrioRequestMessage(SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay) 
		  {
		  }
	};

	typedef boost::shared_ptr<ShutdownSceanrioRequestMessage> ShutdownSceanrioRequestMessagePtr;

}
#endif
