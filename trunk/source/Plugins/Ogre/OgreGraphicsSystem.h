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
#include "Core/MessageSystem/MessageType.h"
#include "Plugins/Ogre/OgreGraphicsSceneManager.h"
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
	class OgreGraphicsSystem : public Reflection<OgreGraphicsSystem, SimSystem>, public IGraphicsSystem
	{
		friend class OgreGraphicsSceneManager;
	public:
		OgreGraphicsSystem();
		virtual ~OgreGraphicsSystem();
		static void RegisterReflection();
		virtual void OnCreate();
		SystemType GetSystemType() {return "GraphicsSystem";}
		void GetMainWindowInfo(unsigned int &width, unsigned int &height, int &left, int &top);
		void Update(double delta);
		Ogre::RenderWindow* GetMainWindow() const {return m_Window;}
		OgrePostProcessPtr GetPostProcess() {return m_PostProcess;}
	protected:
		void OnDebugPrint(MessagePtr message);
		void SetActiveSceneManger(Ogre::SceneManager *sm);
		void AddPlugin(const std::string &plugin){m_Plugins.push_back(plugin);}
		void AddViewport(Ogre::SceneManager *sm, Ogre::RenderWindow* win, float left , float top, float width , float height,Ogre::ColourValue colour);
		bool GetCreateMainWindowOnInit() const {return m_CreateMainWindowOnInit;}
		void SetCreateMainWindowOnInit(bool value){m_CreateMainWindowOnInit = value;}
		std::vector<std::string> GetPostFilters() const;
		void SetPostFilters(const std::vector<std::string> &filters);
		void SetPrimaryThread(bool value);
		bool GetPrimaryThread() const;
	
		void OnInit(MessagePtr message);
		void OnCreateRenderWindow(MessagePtr message);
		void OnWindowMovedOrResized(MessagePtr message);
	
		std::string m_RenderSystem;
		Ogre::Root* m_Root;
		Ogre::RenderWindow* m_Window;
		Ogre::SceneManager* m_SceneMgr;
		std::vector<std::string> m_Plugins;
		std::vector<std::string> m_PostFilters;
		OgreDebugTextOutput* m_DebugTextBox;
		//std::vector<ISceneManager*> m_SceneManagers;
		bool m_CreateMainWindowOnInit;
		OgrePostProcessPtr m_PostProcess;
		bool m_PrimaryThread;
	};
	typedef boost::shared_ptr<OgreGraphicsSystem> OgreGraphicsSystemPtr;
}
