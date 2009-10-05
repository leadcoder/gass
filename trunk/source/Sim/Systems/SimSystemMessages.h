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

	//Divided messages in two catagories, notify and request
	//Messages with prefix SYSTEM_RM, is a request message
	//Messages with prefix SYSTEM_NM, is a notify message
	enum SimSystemMessage
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
	};

	class CreateRenderWindowMessage : public BaseMessage
	{
	public:
		CreateRenderWindowMessage(const std::string &name, int height,int width, int handle,int main_handle = 0, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(CreateRenderWindowMessage::MID , sender_id , delay), 
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
		  enum { MID = SYSTEM_RM_CREATE_RENDER_WINDOW};
	private:
		std::string m_Name;
		int m_Height;
		int m_Width;
		int m_Handle;
		int m_MainHandle;
	};
	typedef boost::shared_ptr<CreateRenderWindowMessage> CreateRenderWindowMessagePtr;

	class DebugPrintMessage : public BaseMessage
	{
	public:
		DebugPrintMessage(const std::string &text, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(DebugPrintMessage::MID , sender_id , delay), 
			  m_Text(text)  { }
		  std::string GetText()const {return m_Text;}
		  enum { MID = SYSTEM_RM_DEBUG_PRINT};
	private:
		std::string m_Text;

	};
	typedef boost::shared_ptr<DebugPrintMessage> DebugPrintMessagePtr;


	class GFXSceneManagerLoadedNotifyMessage : public BaseMessage
	{
	public:
		GFXSceneManagerLoadedNotifyMessage(const std::string &render_system, void* scene_graph_root_node,SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(GFXSceneManagerLoadedNotifyMessage::MID , sender_id , delay), 
			  m_RenderSystem(render_system),m_RootNode(scene_graph_root_node) { }
		  std::string GetRenderSystem()const {return m_RenderSystem;}
		  void *GetSceneGraphRootNode()const {return m_RootNode;}
		  enum { MID = SYSTEM_NM_GFX_SM_LOADED};
	private:
		void* m_RootNode;
		std::string m_RenderSystem;
	};
	typedef boost::shared_ptr<GFXSceneManagerLoadedNotifyMessage> GFXSceneManagerLoadedNotifyMessagePtr;

	class MainWindowCreatedNotifyMessage : public BaseMessage
	{
	public:
		MainWindowCreatedNotifyMessage(int render_window_handle, int main_window_handle,SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(MainWindowCreatedNotifyMessage::MID , sender_id , delay), 
			  m_Handle(render_window_handle),m_MainHandle(main_window_handle) { }
		  int GetRenderWindowHandle() const {return m_Handle;}
		  int GetMainHandle() const {return m_MainHandle;}
		  enum { MID = SYSTEM_NM_MAIN_WINDOW_CREATED};
	private:
		int m_Handle;
		int m_MainHandle;

	};
	typedef boost::shared_ptr<MainWindowCreatedNotifyMessage> MainWindowCreatedNotifyMessagePtr;


	class MainWindowMovedOrResizedNotifyMessage : public BaseMessage
	{
	public:
		MainWindowMovedOrResizedNotifyMessage (SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(MainWindowMovedOrResizedNotifyMessage::MID , sender_id , delay)  {}
		  enum { MID = SYSTEM_NM_WINDOW_MOVED_OR_RESIZED};
	private:
	};
	typedef boost::shared_ptr<MainWindowMovedOrResizedNotifyMessage> MainWindowMovedOrResizedNotifyMessagePtr;

	class ScenarioSceneLoadedNotifyMessage : public BaseMessage
	{
	public:
		ScenarioSceneLoadedNotifyMessage (ScenarioScene* scenario_scene,SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(ScenarioSceneLoadedNotifyMessage::MID , sender_id , delay) , 
			  m_ScenarioScene(scenario_scene){}
		  enum { MID = SYSTEM_NM_SCENARIO_SCENE_LOADED};
		  ScenarioScene* GetScenarioScene() const {return m_ScenarioScene;}
	private:
		ScenarioScene* m_ScenarioScene;		
	};
	typedef boost::shared_ptr<ScenarioSceneLoadedNotifyMessage> ScenarioSceneLoadedNotifyMessagePtr;

	class ScenarioSceneAboutToLoadNotifyMessage : public BaseMessage
	{
	public:
		ScenarioSceneAboutToLoadNotifyMessage(ScenarioScene* scenario_scene, SenderID sender_id = -1, double delay= 0) : 
		  BaseMessage(ScenarioSceneAboutToLoadNotifyMessage::MID , sender_id , delay) , 
			  m_ScenarioScene(scenario_scene){}
		  enum { MID = SYSTEM_NM_SCENARIO_SCENE_ABOUT_TO_LOAD};
		  ScenarioScene* GetScenarioScene() const {return m_ScenarioScene;}
	private:
		ScenarioScene* m_ScenarioScene;		
	};
	typedef boost::shared_ptr<ScenarioSceneAboutToLoadNotifyMessage> ScenarioSceneAboutToLoadNotifyMessagePtr;

}
#endif