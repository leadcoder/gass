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
}

namespace GASS
{
	class OgreDebugTextOutput;
	class OgrePostProcess;
	typedef boost::shared_ptr<OgrePostProcess> OgrePostProcessPtr;
	class OgreGraphicsSystem : public Reflection<OgreGraphicsSystem, SimSystem>, public IGraphicsSystem, public ITaskListener
	{
		friend class OgreGraphicsSceneManager;
	public:
		OgreGraphicsSystem();
		virtual ~OgreGraphicsSystem();
		static void RegisterReflection();
		virtual void OnCreate();
		SystemType GetSystemType() const {return "GraphicsSystem";}
		void GetMainWindowInfo(unsigned int &width, unsigned int &height, int &left, int &top);

		//ITaskListener interface
		void Update(double delta);
		TaskGroup GetTaskGroup() const;

		Ogre::RenderWindow* GetMainWindow() const {return m_Window;}
		OgrePostProcessPtr GetPostProcess() {return m_PostProcess;}
	protected:
		void OnDebugPrint(DebugPrintMessagePtr message);
		void SetActiveSceneManger(Ogre::SceneManager *sm);
		void AddPlugin(const std::string &plugin){m_Plugins.push_back(plugin);}
		void AddViewport(Ogre::SceneManager *sm, Ogre::RenderWindow* win, float left , float top, float width , float height,Ogre::ColourValue colour);
		bool GetCreateMainWindowOnInit() const {return m_CreateMainWindowOnInit;}
		void SetCreateMainWindowOnInit(bool value){m_CreateMainWindowOnInit = value;}
		std::vector<std::string> GetPostFilters() const;
		void SetPostFilters(const std::vector<std::string> &filters);
		void SetTaskGroup(TaskGroup value);


		void OnInit(InitMessagePtr message);
		void OnCreateRenderWindow(CreateRenderWindowMessagePtr message);
		void OnWindowMovedOrResized(MainWindowMovedOrResizedNotifyMessagePtr message);

		std::string m_RenderSystem;
		Ogre::Root* m_Root;
		Ogre::RenderWindow* m_Window;
		Ogre::SceneManager* m_SceneMgr;
		std::vector<std::string> m_Plugins;
		std::vector<std::string> m_PostFilters;
		OgreDebugTextOutput* m_DebugTextBox;
		bool m_CreateMainWindowOnInit;
		OgrePostProcessPtr m_PostProcess;
		TaskGroup m_TaskGroup;
	};
	typedef boost::shared_ptr<OgreGraphicsSystem> OgreGraphicsSystemPtr;
}
