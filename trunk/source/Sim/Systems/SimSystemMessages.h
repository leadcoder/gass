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

#ifndef SIM_SYSTEM_MESSAGES_H
#define SIM_SYSTEM_MESSAGES_H

#include "Sim/Common.h"
#include "Core/MessageSystem/BaseMessage.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Math/Vector.h"
#include "Core/Math/Quaternion.h"
#include <string>

namespace GASS
{
	class Scenario;
	class ScenarioScene;
	class SceneObject;

	typedef boost::shared_ptr<ScenarioScene> ScenarioScenePtr;
	typedef boost::shared_ptr<Scenario> ScenarioPtr;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	

	/**
	Deafult message used by the SimSystemManager
	Sim system messages are used to communicate with systems and
	used by systems to notify listeners about critcal system events
	To send a SimSystemMessage you have to get hold of the
	SimSystemManager and then post a message, ex:
	SimEngine::Get().GetSimSystemManager()->PostMessage(MessagePtr(new DebugPrintMessage("Testing")))
	*/

	class InitMessage : public BaseMessage
	{
	public:
		InitMessage (SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay)
			{ }
	private:

	};
	typedef boost::shared_ptr<InitMessage> InitMessagePtr;


	/**
	Message that can be posted by anyone to request that a new render window should be created.
	@remark DEPRECATED, not possible to create renderwindows in runtime
	*/

	/*class CreateRenderWindowMessage : public BaseMessage
	{
	public:
		CreateRenderWindowMessage(const std::string &name, int width, int height, int handle,int main_handle = 0, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay),
			  m_Name(name),
			  m_Height(height),
			  m_Width(width),
			  m_Handle(handle),
			  m_MainHandle(main_handle)  { }
		  std::string GetName()const {return m_Name;}
		  int GetWidth() const {return m_Width;}
		  int GetHeight() const {return m_Height;}
		  int GetHandle() const {return m_Handle;}
		  int GetMainHandle() const {return m_MainHandle;}
	private:
		std::string m_Name;
		int m_Height;
		int m_Width;
		int m_Handle;
		int m_MainHandle;
	};
	typedef boost::shared_ptr<CreateRenderWindowMessage> CreateRenderWindowMessagePtr;*/

	/**
	Message that can be posted by anyone to request that a new debug messages should be visualized during one frame.
	*/
	class DebugPrintMessage : public BaseMessage
	{
	public:
		DebugPrintMessage(const std::string &text, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay),
			  m_Text(text)  { }
		  std::string GetText()const {return m_Text;}

	private:
		std::string m_Text;

	};
	typedef boost::shared_ptr<DebugPrintMessage> DebugPrintMessagePtr;


	/**
	Message posted by the graphic scene manager to notify that the scene manager
	has loaded successfully. The message provided the name of the render system in use and
	a root node to the scene graph created by the scene manager. The root node is  a void-pointer
	and should be casted to the correct scene node type depending on what the render system in use.
	ie. If Ogre3d is the render system the GetSceneGraphRootNode will return a Ogre::SceneNode*, if OSG
	is used the root node will be osg::Node* etc.
	This message is usefull for render system extensions that don't want to link to a specific render system
	implementation.
	*/

	class GFXSceneManagerLoadedNotifyMessage : public BaseMessage
	{
	public:
		GFXSceneManagerLoadedNotifyMessage(const std::string &render_system, void* scene_graph_root_node,void* scene_graph_shadow_node,SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay),
			  m_RenderSystem(render_system),m_RootNode(scene_graph_root_node) ,m_ShadowNode(scene_graph_shadow_node) { }
		  std::string GetRenderSystem()const {return m_RenderSystem;}
		  void *GetSceneGraphRootNode()const {return m_RootNode;}
		  void *GetSceneGraphShadowNode()const {return m_ShadowNode;}


	private:
		void* m_RootNode;
		void* m_ShadowNode;
		std::string m_RenderSystem;
	};
	typedef boost::shared_ptr<GFXSceneManagerLoadedNotifyMessage> GFXSceneManagerLoadedNotifyMessagePtr;

	/**
		Message posted by the graphic system to notify that a new render window has been created.
		Suscribe to this message if you need to get hold of the render window handle,
	*/

	class MainWindowCreatedNotifyMessage : public BaseMessage
	{
	public:
		MainWindowCreatedNotifyMessage(int render_window_handle, int main_window_handle,SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay),
			  m_Handle(render_window_handle),m_MainHandle(main_window_handle) { }
		  int GetRenderWindowHandle() const {return m_Handle;}
		  int GetMainHandle() const {return m_MainHandle;}

	private:
		int m_Handle;
		int m_MainHandle;

	};
	typedef boost::shared_ptr<MainWindowCreatedNotifyMessage> MainWindowCreatedNotifyMessagePtr;


	/**
		If you have a created a external render window this messages should be posted by the external
		owner if the render window has moved or resized.
		A graphic system implementation should suscribe to this message to notify
		it's internal window system about the change
	*/

	class ViewportMovedOrResizedNotifyMessage : public BaseMessage
	{
	public:
		ViewportMovedOrResizedNotifyMessage (const std::string &viewport_name, int pos_x, int pos_y, int width,int height,SenderID sender_id = -1, double delay= 0) :
		  m_PosX(pos_x),
		  m_PosY(pos_y),
		  m_Width(width),
		  m_Height(height),
		  m_VPName(viewport_name),
		  BaseMessage(sender_id , delay)  {}
		  int GetPositionX()const {return m_PosX;}
		  int GetPositionY()const {return m_PosY;}
		  int GetWidth()const {return m_Width;}
		  int GetHeight()const {return m_Height;}
		  std::string GetViewportName() const {return m_VPName;}
	private:
		int m_Width,m_Height,m_PosX,m_PosY;
		std::string m_VPName;
	};
	typedef boost::shared_ptr<ViewportMovedOrResizedNotifyMessage> ViewportMovedOrResizedNotifyMessagePtr;

	/**
		This message is posted by the ScenarioScene class when a scene in a scenario has loaded successfully.
		This means that all objects specified in the scenario scene is loaded. If you want to catch the
		loading messages of thoes objects you should instead suscribe to the ScenarioSceneAboutToLoadNotifyMessage
		bellow.
		Suscribe to this message if you want to get hold of scenario scenes after all scene objects are loaded.
	*/

	class ScenarioSceneLoadedNotifyMessage : public BaseMessage
	{
	public:
		ScenarioSceneLoadedNotifyMessage(ScenarioScenePtr scenario_scene,SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay) ,
			  m_ScenarioScene(scenario_scene){}

		  ScenarioScenePtr GetScenarioScene() const {return m_ScenarioScene;}
	private:
		ScenarioScenePtr m_ScenarioScene;
	};
	typedef boost::shared_ptr<ScenarioSceneLoadedNotifyMessage> ScenarioSceneLoadedNotifyMessagePtr;


	/**
		This message is posted by the Scenario class before the scenario scenes are loaded.
	*/

	class ScenarioAboutToLoadNotifyMessage : public BaseMessage
	{
	public:
		ScenarioAboutToLoadNotifyMessage(ScenarioPtr scenario, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay) ,
			  m_Scenario(scenario){}

		  ScenarioPtr GetScenario() const {return m_Scenario;}
	private:
		ScenarioPtr m_Scenario;
	};

	typedef boost::shared_ptr<ScenarioAboutToLoadNotifyMessage> ScenarioAboutToLoadNotifyMessagePtr;

	/**
		This message is posted by the ScenarioScene class before the scene objects are loaded.
		Suscribe to this message if you want to get hold of sceario scenes before all scene objects are loaded. This
		can be usefull if you want to modify, add or save some objects loaded by the scenario scene.
	*/

	class ScenarioSceneAboutToLoadNotifyMessage : public BaseMessage
	{
	public:
		ScenarioSceneAboutToLoadNotifyMessage(ScenarioScenePtr scenario_scene, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay) ,
			  m_ScenarioScene(scenario_scene){}

		  ScenarioScenePtr GetScenarioScene() const {return m_ScenarioScene;}
	private:
		ScenarioScenePtr m_ScenarioScene;
	};
	typedef boost::shared_ptr<ScenarioSceneAboutToLoadNotifyMessage> ScenarioSceneAboutToLoadNotifyMessagePtr;


	class ScenarioSceneUnloadNotifyMessage : public BaseMessage
	{
	public:
		ScenarioSceneUnloadNotifyMessage(ScenarioScenePtr scenario_scene, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay) ,
			  m_ScenarioScene(scenario_scene){}

		  ScenarioScenePtr GetScenarioScene() const {return m_ScenarioScene;}
	private:
		ScenarioScenePtr m_ScenarioScene;
	};
	typedef boost::shared_ptr<ScenarioSceneUnloadNotifyMessage> ScenarioSceneUnloadNotifyMessagePtr;



	class RequestTimeStepMessage : public BaseMessage
	{
	public:
		RequestTimeStepMessage  (const double &step, SenderID sender_id = -1, double delay= 0) : BaseMessage(sender_id , delay), m_TimeStep(step)
		{
		}
		double GetTimeStep() const {return m_TimeStep;}
	private:
		double m_TimeStep;
	};
	typedef boost::shared_ptr<RequestTimeStepMessage> RequestTimeStepMessagePtr;

	/////////NETWORK SERVER MESSAGES////////////////

	/**
	Start network server
	*/

	class StartServerMessage : public BaseMessage
	{
	public:
		StartServerMessage(const std::string &name, int port, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay) ,
			  m_Name(name), m_Port(port){}

		  std::string GetServerName() const {return m_Name;}
		  int GetPort() const {return m_Port;}
	private:
		std::string m_Name;
		int m_Port;
	};
	typedef boost::shared_ptr<StartServerMessage> StartServerMessagePtr;


	/**
	Stop network server
	*/
	class StopServerMessage : public BaseMessage
	{
	public:
		StopServerMessage(SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay){}
	private:
	};
	typedef boost::shared_ptr<StopServerMessage> StopServerMessagePtr;



	/**
	Client connected to network server
	*/
	class ClientConnectedMessage : public BaseMessage
	{
	public:
		ClientConnectedMessage(const std::string &name, int client_port, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay) ,
			  m_Name(name), m_ClientPort(client_port){}

		 std::string GetClientName() const {return m_Name;}
	private:
		std::string m_Name;
		int m_ClientPort;
	};
	typedef boost::shared_ptr<ClientConnectedMessage> ClientConnectedMessagePtr;


	/**
	Client disconnected from network server
	*/
	class ClientDisconnectedMessage : public BaseMessage
	{
	public:
		ClientDisconnectedMessage(const std::string &name, int client_port, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay) ,
			  m_Name(name), m_ClientPort(client_port){}

		 std::string GetClientName() const {return m_Name;}
	private:
		std::string m_Name;
		int m_ClientPort;
	};
	typedef boost::shared_ptr<ClientDisconnectedMessage> ClientDisconnectedMessagePtr;


	/////////NETWORK CLIENT MESSAGES////////////////
	/**
	Start network client
	*/
	class StartClientMessage : public BaseMessage
	{
	public:
		StartClientMessage(const std::string &name, int client_port, int server_port, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay) ,
			  m_Name(name), m_ClientPort(client_port), m_ServerPort(server_port){}

		 std::string GetClientName() const {return m_Name;}
		 int GetClientPort() const {return m_ClientPort;}
		 int GetServerPort() const {return m_ServerPort;}
	private:
		std::string m_Name;
		int m_ClientPort;
		int m_ServerPort;
	};
	typedef boost::shared_ptr<StartClientMessage> StartClientMessagePtr;


	/**
	Server disconnected
	*/
	class ServerDisconnectedMessage : public BaseMessage
	{
	public:
		ServerDisconnectedMessage(const std::string &name, int port, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay) ,
			  m_Name(name), m_Port(port){}
		 std::string GetServerName() const {return m_Name;}
	private:
		std::string m_Name;
		int m_Port;
	};
	typedef boost::shared_ptr<ServerDisconnectedMessage> ServerDisconnectedMessagePtr;

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
	class ServerResponseMessage : public BaseMessage
	{
	public:
		ServerResponseMessage(const std::string &server_name, int server_port, float ping_time,SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay) ,
			  m_Name(server_name), m_ServerPort(server_port), m_PingTime(ping_time){}

		 std::string GetServerName() const {return m_Name;}
		 int GetServerPort() const {return m_ServerPort;}
		 float GetServerPingTime() const {return m_PingTime;}
	private:
		std::string m_Name;
		int m_ServerPort;
		float m_PingTime;
	};
	typedef boost::shared_ptr<ServerResponseMessage> ServerResponseMessagePtr;


	/**
	Connect to server
	*/
	class ConnectToServerMessage : public BaseMessage
	{
	public:
		ConnectToServerMessage(const std::string &server_name, int server_port,SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay) ,
			  m_Name(server_name), m_ServerPort(server_port){}

		 std::string GetServerName() const {return m_Name;}
		 int GetServerPort() const {return m_ServerPort;}
	private:
		std::string m_Name;
		int m_ServerPort;
	};
	typedef boost::shared_ptr<ConnectToServerMessage> ConnectToServerMessagePtr;

	/**
	Ping for servers
	*/
	class PingRequestMessage : public BaseMessage
	{
	public:
		PingRequestMessage(int server_port,SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay) ,
			  m_ServerPort(server_port){}
		 int GetServerPort() const {return m_ServerPort;}
	private:
		int m_ServerPort;
	};
	typedef boost::shared_ptr<PingRequestMessage> PingRequestMessagePtr;


	/**
	Server inform client about scenario
	*/
	class StartSceanrioRequestMessage : public BaseMessage
	{
	public:
		StartSceanrioRequestMessage(const std::string &scenario_name, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay) ,
			  m_Name(scenario_name){}

		 std::string GetScenarioName() const {return m_Name;}
	private:
		std::string m_Name;
	};
	typedef boost::shared_ptr<StartSceanrioRequestMessage> StartSceanrioRequestMessagePtr;

	class ShutdownSceanrioRequestMessage : public BaseMessage
	{
	public:
		ShutdownSceanrioRequestMessage(SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay) 
		  {
		  }
	};

	typedef boost::shared_ptr<ShutdownSceanrioRequestMessage> ShutdownSceanrioRequestMessagePtr;


	//Script messages
	/**
	Lua script state is distributed through this message
	*/
	class LuaScriptStateMessage : public BaseMessage
	{
	public:
		LuaScriptStateMessage(void *state, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay) ,
			  m_State(state){}
		 void* GetState() const {return m_State;}
	private:
		void *m_State;
	};
	typedef boost::shared_ptr<LuaScriptStateMessage> LuaScriptStateMessagePtr;

	/**
	Load a gui script file	
	*/
	class GUIScriptMessage : public BaseMessage
	{
	public:
		GUIScriptMessage(const std::string &filename, SenderID sender_id = -1, double delay= 0) :
		  BaseMessage(sender_id , delay) ,
			  m_Name(filename){}

		 std::string GetFilename() const {return m_Name;}
	private:
		std::string m_Name;
	};
	typedef boost::shared_ptr<GUIScriptMessage> GUIScriptMessagePtr;
}
#endif
