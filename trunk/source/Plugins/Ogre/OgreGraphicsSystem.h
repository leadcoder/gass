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

#include "Sim/Interface/GASSIGraphicsSystem.h"
#include "Sim/GASSSimSystem.h"
#include "Sim/Messages/GASSCoreSystemMessages.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"
#include "Core/MessageSystem/GASSMessageType.h"
#include "Plugins/Ogre/OgreGraphicsSceneManager.h"
#include <OgreRenderWindow.h>
#include <string>

namespace Ogre
{
	class Root;
	class SceneManager;
	class ColourValue;
	class Viewport;
}

namespace GASS
{
	class OgreGraphicsSystem;
	typedef boost::shared_ptr<OgreGraphicsSystem> OgreGraphicsSystemPtr;
	typedef boost::weak_ptr<OgreGraphicsSystem> OgreGraphicsSystemWeakPtr;

	class OgreViewport  : public IViewport
	{
	public:
		OgreViewport(Ogre::Viewport* vp) : m_OgreViewport(vp)
		{

		}
		OgreViewport(){}
		Ogre::Viewport* m_OgreViewport;
	};
	typedef boost::shared_ptr<OgreViewport> OgreViewportPtr;

	class OgreRenderWindow  : public IRenderWindow
	{
	public:
		OgreRenderWindow(OgreGraphicsSystem* system, Ogre::RenderWindow* win) : m_System(system), m_Window(win)
		{

		}
		
		virtual unsigned int GetWidth() const {return m_Window->getWidth();}
		virtual unsigned int GetHeight() const {return m_Window->getHeight();}
		virtual void* GetHWND() const
		{
			void* window_hnd = 0;
			m_Window->getCustomAttribute("WINDOW", &window_hnd);
			return window_hnd; 
		}
		OgreGraphicsSystem* GetSystem() const{return m_System;}
		ViewportPtr CreateViewport(const std::string &name, float  left, float top, float width, float height);
		Ogre::RenderWindow* m_Window;
		std::vector<OgreViewportPtr> m_Viewports;
		OgreGraphicsSystem* m_System;
	};
	typedef boost::shared_ptr<OgreRenderWindow> OgreRenderWindowPtr;


	class Viewport
	{
	public:
		Viewport(const std::string &name, const std::string &window, float left,float top,float width, float height, int z_depth, Ogre::Viewport* vp = NULL) : m_Name(name),m_Window(window),m_Left(left),m_Top(top),m_Width(width), m_Height(height),m_OgreViewport(vp),m_ZDepth(z_depth)
		{
		}
		Viewport(){}
		std::string m_Name;
		std::string m_Window;
		float m_Left;
		float m_Top;
		float m_Width;
		float m_Height;
		int m_ZDepth;
		Ogre::Viewport* m_OgreViewport;
	};
	class OgreDebugTextOutput;
	class OgrePostProcess;
	class OgreCameraComponent;
	typedef boost::shared_ptr<OgrePostProcess> OgrePostProcessPtr;
	typedef boost::shared_ptr<OgreCameraComponent> OgreCameraComponentPtr;

	class OgreGraphicsSystem : public Reflection<OgreGraphicsSystem, SimSystem>, public IGraphicsSystem
	{
		friend class OgreGraphicsSceneManager;
	public:
		OgreGraphicsSystem();
		virtual ~OgreGraphicsSystem();
		static void RegisterReflection();
		virtual void Init();
		virtual std::string GetSystemName() const {return "OgreGraphicsSystem";}
		virtual void Update(double time);
		//IGraphicsSystem

		virtual RenderWindowPtr GetMainRenderWindow() const;
		virtual std::vector<RenderWindowPtr> GetRenderWindows() const;

		//void GetMainWindowInfo(unsigned int &width, unsigned int &height, int &left, int &top) const;
		RenderWindowPtr CreateRenderWindow(const std::string &name, int width, int height, void* external_window_handle = 0);
		Ogre::SceneManager* GetBootSceneManager() const {return m_SceneMgr;}
		OgrePostProcessPtr GetPostProcess() const {return m_PostProcess;}
	protected:
		ADD_ATTRIBUTE(bool,UpdateMessagePump);

		void OnDebugPrint(DebugPrintRequestPtr message);
		void OnDrawLine(DrawLineRequestPtr message);
		void OnDrawCircle(DrawCircleRequestPtr message);
		void OnInitializeTextBox(CreateTextBoxRequestPtr message);
		void SetActiveSceneManger(Ogre::SceneManager *sm);
		void AddPlugin(const std::string &plugin){m_Plugins.push_back(plugin);}
		void AddViewport(Ogre::SceneManager *sm, const std::string &name, const std::string &win_name, float left , float top, float width , float height,Ogre::ColourValue colour, int zdepth);
		void RemoveViewport(const std::string &name, const std::string &win_name);
		bool GetCreateMainWindowOnInit() const {return m_CreateMainWindowOnInit;}
		void SetCreateMainWindowOnInit(bool value){m_CreateMainWindowOnInit = value;}
		bool GetShowStats() const {return m_ShowStats;}
		void SetShowStats(bool value){m_ShowStats = value;}
		void ChangeCamera(const std::string &vp_name, OgreCameraComponentPtr cam_comp);
		std::vector<std::string> GetPostFilters() const;
		void SetPostFilters(const std::vector<std::string> &filters);
		void OnViewportMovedOrResized(ViewportMovedOrResizedEventPtr message);
		std::string m_RenderSystem;
		Ogre::Root* m_Root;
		std::map<std::string, OgreRenderWindowPtr>	m_Windows;
		std::map<std::string, Viewport> m_Viewports;
		std::map<std::string, Ogre::Viewport> m_OgreViewports;
		Ogre::SceneManager* m_SceneMgr;
		std::vector<std::string> m_Plugins;
		std::vector<std::string> m_PostFilters;
		OgreDebugTextOutput* m_DebugTextBox;
		bool m_CreateMainWindowOnInit;
		bool m_ShowStats;
		OgrePostProcessPtr m_PostProcess;
	};
	
	
}
