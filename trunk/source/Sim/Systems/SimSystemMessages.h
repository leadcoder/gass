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
	class ScenarioScene;
	typedef boost::shared_ptr<ScenarioScene> ScenarioScenePtr;

	/**
	Enumeration of message types used by the SimSystemManager
	Sim system messages are used to communicate with systems and 
	used by systems to notify listeners about critcal system events
	To send a SimSystemMessage you have to get hold of the 
	SimSystemManager and then post a message, ex:
	SimEngine::Get().GetSystemManager()->PostMessage(MessagePtr(new DebugPrintMessage("Testing")))

	Sim system messages are divided in two catagories, notify and request.
	Mesages with prefix SYSTEM_RM, is a request message
	Messages with prefix SYSTEM_NM, is a notify message*/
	/*enum SimSystemMessage
	{
		//-----------------Request section-------------
		SYSTEM_RM_INIT, 
		SYSTEM_RM_CREATE_RENDER_WINDOW,
		SYSTEM_RM_DEBUG_PRINT,

		//--------------------Notify section------------------------
		SYSTEM_NM_GFX_SM_LOADED,
		SYSTEM_NM_MAIN_WINDOW_CREATED,
		SYSTEM_NM_WINDOW_MOVED_OR_RESIZED,
		SYSTEM_NM_SCENARIO_SCENE_LOADED,
		SYSTEM_NM_SCENARIO_SCENE_ABOUT_TO_LOAD
	};*/

	/**
	Message used that can be posted by anyone to request that a new render window should be created.
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


	class CreateRenderWindowMessage : public BaseMessage
	{
	public:
		CreateRenderWindowMessage(const std::string &name, int height,int width, int handle,int main_handle = 0, SenderID sender_id = -1, double delay= 0) : 
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
	typedef boost::shared_ptr<CreateRenderWindowMessage> CreateRenderWindowMessagePtr;

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
		GFXSceneManagerLoadedNotifyMessage(const std::string &render_system, void* scene_graph_root_node,SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay), 
			  m_RenderSystem(render_system),m_RootNode(scene_graph_root_node) { }
		  std::string GetRenderSystem()const {return m_RenderSystem;}
		  void *GetSceneGraphRootNode()const {return m_RootNode;}
		 
	private:
		void* m_RootNode;
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

	class MainWindowMovedOrResizedNotifyMessage : public BaseMessage
	{
	public:
		MainWindowMovedOrResizedNotifyMessage (SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(sender_id , delay)  {}
		  
	private:
	};
	typedef boost::shared_ptr<MainWindowMovedOrResizedNotifyMessage> MainWindowMovedOrResizedNotifyMessagePtr;

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
		This message is posted by the ScenarioScene class before the scenario objects are loaded.
		Suscribe to this message if you want to get hold of sceario scenes before all scene objects are loaded. This
		can be usefull if you want to modify, add or save some objects the scenario scene loaded
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

	/**
Convenience macro used for registration of scene object message callbacks for specific scene object message class
*/

#define REGISTER_SYSTEM_MESSAGE_CLASS(FUNCTION,TYPED_MESSAGE,PRIORITY) GetSimSystemManager()->RegisterForMessage((SimSystemMessage)TYPED_MESSAGE::SSMID,TYPED_MESSAGE_FUNC(FUNCTION,TYPED_MESSAGE),PRIORITY);

/**
Convenience macro used for registration of scene object message callbacks for specific scene object message type
*/
#define REGISTER_SYSTEM_MESSAGE_TYPE(FUNCTION,MESSAGE_TYPE,PRIORITY) GetSimSystemManager()->RegisterForMessage(MESSAGE_TYPE,MESSAGE_FUNC(FUNCTION),PRIORITY);

}
#endif