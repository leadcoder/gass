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

#pragma once

#include "Sim/Systems/Graphics/IGraphicsSystem.h"
#include "Sim/Systems/SimSystem.h"
#include "Sim/Systems/SimSystemMessages.h"
#include "Core/MessageSystem/MessageType.h"
#include "Plugins/Ogre/OgreGraphicsSceneManager.h"
#include "Sim/Scheduling/TaskGroups.h"
#include "Sim/Scheduling/ITaskListener.h"
#include <string>

namespace Ogre
{
	class Root;
	class RenderWindow;
	class SceneManager;
	class ColourValue;
	class Viewport;
}

namespace GASS
{
	class Viewport
	{
	public:
		Viewport(const std::string &name, const std::string &window, float left,float top,float width, float height, Ogre::Viewport* vp = NULL) : m_Name(name),m_Window(window),m_Left(left),m_Top(top),m_Width(width), m_Height(height),m_OgreViewport(vp)
		{
		}
		Viewport(){}
		std::string m_Name;
		std::string m_Window;
		float m_Left;
		float m_Top;
		float m_Width;
		float m_Height;
		Ogre::Viewport* m_OgreViewport;
	};
	class OgreDebugTextOutput;
	class OgrePostProcess;
	class OgreCameraComponent;
	typedef boost::shared_ptr<OgrePostProcess> OgrePostProcessPtr;
	typedef boost::shared_ptr<OgreCameraComponent> OgreCameraComponentPtr;

	class OgreGraphicsSystem : public Reflection<OgreGraphicsSystem, SimSystem>, public IGraphicsSystem, public ITaskListener
	{
		friend class OgreGraphicsSceneManager;
	public:
		OgreGraphicsSystem();
		virtual ~OgreGraphicsSystem();
		static void RegisterReflection();
		virtual void OnCreate();
		SystemType GetSystemType() const {return "GraphicsSystem";}

		//IGraphicsSystem
		void GetMainWindowInfo(unsigned int &width, unsigned int &height, int &left, int &top);
		void CreateRenderWindow(const std::string &name, int width, int height, int handle, int main_handle = 0);
		void CreateViewport(const std::string &name, const std::string &render_window, float  left, float top, float width, float height);

		//ITaskListener interface
		void Update(double delta);
		TaskGroup GetTaskGroup() const;

		//Ogre::RenderWindow* GetMainWindow() const {return m_Window;}
		OgrePostProcessPtr GetPostProcess() {return m_PostProcess;}
	protected:
		void OnDebugPrint(DebugPrintMessagePtr message);
		void SetActiveSceneManger(Ogre::SceneManager *sm);
		void AddPlugin(const std::string &plugin){m_Plugins.push_back(plugin);}
		//void AddViewport(Ogre::SceneManager *sm, Ogre::RenderWindow* win, float left , float top, float width , float height,Ogre::ColourValue colour);
		void AddViewport(Ogre::SceneManager *sm, const std::string &name, const std::string &win_name, float left , float top, float width , float height,Ogre::ColourValue colour);
		bool GetCreateMainWindowOnInit() const {return m_CreateMainWindowOnInit;}
		void SetCreateMainWindowOnInit(bool value){m_CreateMainWindowOnInit = value;}
		bool GetShowStats() const {return m_ShowStats;}
		void SetShowStats(bool value){m_ShowStats = value;}
		void ChangeCamera(const std::string &vp_name, OgreCameraComponentPtr cam_comp);
		std::vector<std::string> GetPostFilters() const;
		void SetPostFilters(const std::vector<std::string> &filters);
		void SetTaskGroup(TaskGroup value);


		void OnInit(InitMessagePtr message);
		//void OnCreateRenderWindow(CreateRenderWindowMessagePtr message);
		void OnViewportMovedOrResized(ViewportMovedOrResizedNotifyMessagePtr message);

		std::string m_RenderSystem;
		Ogre::Root* m_Root;
		std::map<std::string, Ogre::RenderWindow*>	m_Windows;
		std::map<std::string, Viewport> m_Viewports;
		std::map<std::string, Ogre::Viewport> m_OgreViewports;
		Ogre::SceneManager* m_SceneMgr;
		std::vector<std::string> m_Plugins;
		std::vector<std::string> m_PostFilters;
		OgreDebugTextOutput* m_DebugTextBox;
		bool m_CreateMainWindowOnInit;
		bool m_ShowStats;
		OgrePostProcessPtr m_PostProcess;
		TaskGroup m_TaskGroup;
	};
	typedef boost::shared_ptr<OgreGraphicsSystem> OgreGraphicsSystemPtr;
}
