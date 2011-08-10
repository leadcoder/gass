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
#include "Core/Common.h"
#include "Plugins/Ogre/OgreGraphicsSystem.h"
#include "Plugins/Ogre/OgreDebugTextOutput.h"
#include "Plugins/Ogre/OgrePostProcess.h"
#include "Core/System/SystemFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Sim/Scheduling/IRuntimeController.h"
#include "Sim/Systems/Input/IInputSystem.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/SimEngine.h"
#include <boost/bind.hpp>

#include <OgreRoot.h>
#include <OgreRenderWindow.h>
#include <OgreRenderSystem.h>
#include <OgreWindowEventUtilities.h>
#include <OgreStringConverter.h>

using namespace Ogre;

namespace GASS
{
	OgreGraphicsSystem::OgreGraphicsSystem(void): m_CreateMainWindowOnInit(true), m_SceneMgr(NULL),
		m_TaskGroup(GRAPHICS_TASK_GROUP)
	{
		m_DebugTextBox = new OgreDebugTextOutput();

	}

	OgreGraphicsSystem::~OgreGraphicsSystem(void)
	{
		delete m_DebugTextBox;
	}

	void OgreGraphicsSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("OgreGraphicsSystem",new GASS::Creator<OgreGraphicsSystem, ISystem>);
		RegisterProperty<std::string>( "Plugin", NULL, &GASS::OgreGraphicsSystem::AddPlugin);
		RegisterVectorProperty<std::string >("PostFilters", &GASS::OgreGraphicsSystem::GetPostFilters, &GASS::OgreGraphicsSystem::SetPostFilters);
		RegisterProperty<bool>("CreateMainWindowOnInit", &GASS::OgreGraphicsSystem::GetCreateMainWindowOnInit, &GASS::OgreGraphicsSystem::SetCreateMainWindowOnInit);
		RegisterProperty<TaskGroup>("TaskGroup", &GASS::OgreGraphicsSystem::GetTaskGroup, &GASS::OgreGraphicsSystem::SetTaskGroup);
		RegisterProperty<bool>("ShowStats", &GASS::OgreGraphicsSystem::GetShowStats, &GASS::OgreGraphicsSystem::SetShowStats);

	}

	void OgreGraphicsSystem::OnCreate()
	{
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OgreGraphicsSystem::OnInit,InitMessage,0));
		//GetSimSystemManager()->RegisterForMessage(REG_TMESS(OgreGraphicsSystem::OnCreateRenderWindow, CreateRenderWindowMessage,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OgreGraphicsSystem::OnViewportMovedOrResized,ViewportMovedOrResizedNotifyMessage,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OgreGraphicsSystem::OnDebugPrint,DebugPrintMessage,0));

	}

	void OgreGraphicsSystem::OnInit(InitMessagePtr message)
	{

		std::cout << "init:" << m_Name << std::endl;

		//Load plugins
		m_Root = new Root("","ogre.cfg","ogre.log");
		for(int i = 0; i < m_Plugins.size(); i++)
		{
			std::string plugin_name = m_Plugins[i];
#ifdef _DEBUG
			plugin_name += "_d";
#endif
			m_Root->loadPlugin(plugin_name);
		}

		if(!m_Root->restoreConfig())
		{
			if (!m_Root->showConfigDialog())
				return;
			else
				m_Root->saveConfig();
		}

		//Force register in primary thread if ogl
		if(m_Root->getRenderSystem()->getName().find("GL") != Ogre::String::npos)
			m_TaskGroup = MAIN_TASK_GROUP;

		SimEngine::GetPtr()->GetRuntimeController()->Register(this);

		if(m_CreateMainWindowOnInit)
		{
			CreateRenderWindow("MainWindow", 800, 600, 0, 0);
			//CreateViewport("Viewport0","MainWindow", 0, 0, 1, 1);
		}
		else
		{
			m_Root->initialise(false);
		}
	}


	void OgreGraphicsSystem::OnDebugPrint(DebugPrintMessagePtr message)
	{
		std::string debug_text = message->GetText();
		m_DebugTextBox->Print(debug_text.c_str());
		m_DebugTextBox->SetActive(true);
	}

	void OgreGraphicsSystem::OnViewportMovedOrResized(ViewportMovedOrResizedNotifyMessagePtr message)
	{
		std::map<std::string, Ogre::RenderWindow*>::iterator iter = m_Windows.begin();
		while(iter != m_Windows.end())
		{
			//resize all temporary
			iter->second->windowMovedOrResized();
			/*for(int i = 0; i < iter->second->getNumViewports();i++)
			{

			}*/
			++iter;

		}
	}

	void OgreGraphicsSystem::SetActiveSceneManger(Ogre::SceneManager *sm)
	{
		std::map<std::string, Ogre::RenderWindow*>::iterator win_iter = m_Windows.begin();
		while(win_iter != m_Windows.end())
		{
			//set same scene manager in all views
			win_iter->second->removeAllViewports();	
			++win_iter;
		}

		//readd all viewports?
		std::map<std::string, Viewport>::iterator vp_iter = m_Viewports.begin();
		while(vp_iter != m_Viewports.end())
		{
			AddViewport(sm,vp_iter->second.m_Name,vp_iter->second.m_Window,vp_iter->second.m_Left,vp_iter->second.m_Top,vp_iter->second.m_Width,vp_iter->second.m_Height,Ogre::ColourValue(),vp_iter->second.m_ZDepth);
			++vp_iter;
		}
		
		
	}

	void OgreGraphicsSystem::GetMainWindowInfo(unsigned int &width, unsigned int &height, int &left, int &top)
	{
		unsigned int depth;
		if(m_Windows.size() > 0)
			m_Windows.begin()->second->getMetrics(width, height, depth, left, top);
	}



	void OgreGraphicsSystem::Update(double delta_time)
	{
		
		//set thread priority to highest!!
		WindowEventUtilities::messagePump();
		m_Root->renderOneFrame();

		m_DebugTextBox->SetActive(true);
		m_DebugTextBox->UpdateTextBox();

		if(m_Windows.size() > 0 && m_ShowStats)
		{
			float a_fps = m_Windows.begin()->second->getAverageFPS(); 
			size_t tri_count = m_Windows.begin()->second->getTriangleCount(); 
			size_t batch_count = m_Windows.begin()->second->getBatchCount();

			std::stringstream sstream;
			sstream << "AVERAGE FPS:" << a_fps << "\n" <<
				"TRIANGLE COUNT: " << tri_count << "\n"
				"BATCH COUNT: " << batch_count << "\n";
			std::string stats_text = sstream.str();
			GetSimSystemManager()->SendImmediate(MessagePtr( new DebugPrintMessage(stats_text)));
		}
	}

	void OgreGraphicsSystem::AddViewport(Ogre::SceneManager *sm, const std::string &name,const std::string &win_name, float left , float top, float width , float height,Ogre::ColourValue colour, int zdepth)
	{
		Ogre::RenderWindow* win = m_Windows[win_name];
		
		int num_viewports = win->getNumViewports();
		
		//we need a camera before we can create the viewport!
		std::stringstream ss;
		ss << "DefaultViewportCamera" << num_viewports;
		std::string cam_name;
		ss >> cam_name;

		Ogre::Camera* cam;
		if(sm->hasCamera(cam_name))
			cam = sm->getCamera(cam_name);
		else
			cam = sm->createCamera(cam_name);

		cam->setPosition(Ogre::Vector3(0,0,0));
		cam->setNearClipDistance(0.02f);
		cam->setFarClipDistance(5000);
		assert(cam && win);
		Ogre::Viewport* vp = win->addViewport(cam, zdepth, left , top, width , height);

		// Create one viewport, entire window
		vp->setBackgroundColour(colour);
		//Alter the camera aspect ratio to match the viewport
		cam->setAspectRatio( Ogre::Real(vp->getActualWidth())/Ogre::Real(vp->getActualHeight()));

		//add post process to all windows, change this to camera effect instead
		if(m_PostProcess)
		{
			m_PostProcess.reset();
		}

		m_PostProcess = OgrePostProcessPtr(new OgrePostProcess(vp));
		m_PostProcess->SetActiveCompositors(GetPostFilters());

		//save viewport settings for recreation when scenario manager is changed
		Viewport save_vp(name,win_name,left,top,width,height,zdepth,vp);
		m_Viewports[name] = save_vp;
	}

	std::vector<std::string> OgreGraphicsSystem::GetPostFilters() const
	{
		return m_PostFilters;
	}

	void OgreGraphicsSystem::SetPostFilters(const std::vector<std::string> &filters)
	{
		m_PostFilters = filters;
	}

	void OgreGraphicsSystem::SetTaskGroup(TaskGroup value)
	{
		m_TaskGroup = value;
	}

	TaskGroup OgreGraphicsSystem::GetTaskGroup() const
	{
		return m_TaskGroup;
	}

	void OgreGraphicsSystem::CreateRenderWindow(const std::string &name, int width, int height, int handle, int main_handle)
	{
		Ogre::RenderWindow *window = NULL;

		if(main_handle == 0) //autocreate?
		{
			window = m_Root->initialise(true,name);
			window->setDeactivateOnFocusChange(false);
			size_t window_hnd = 0;
			window->getCustomAttribute("WINDOW", &window_hnd);
			handle =window_hnd;
			main_handle = window_hnd;
		}
		else
		{
			if(handle)
			{
				Ogre::NameValuePairList miscParams;
				miscParams["externalWindowHandle"] = Ogre::StringConverter::toString((size_t)handle);
				window = Ogre::Root::getSingleton().createRenderWindow(name,width, height, false, &miscParams);
			}
			else
			{
				window = Ogre::Root::getSingleton().createRenderWindow(name,width, height, false);
			}
		}
		
		m_Windows[name] = window;

		if(m_Windows.size() == 1) // first window
		{
			//We send a message when this window is cretated, usefull for other plugins to get hold of windows handle
			MessagePtr window_msg(new MainWindowCreatedNotifyMessage((int)handle,main_handle));
			GetSimSystemManager()->SendImmediate(window_msg);
		}
	}

	void OgreGraphicsSystem::CreateViewport(const std::string &name, const std::string &render_window, float left, float top, float  width, float height)
	{
		//check that this window exist!
		Ogre::RenderWindow* window = m_Windows[render_window];
		if(window)
		{
			if(m_SceneMgr == NULL) //we need a scene mananger before we can create viewports
				m_SceneMgr = m_Root->createSceneManager("TerrainSceneManager");

			AddViewport(m_SceneMgr, name, render_window, left , top, width , height,Ogre::ColourValue(),m_Viewports.size());
		}
	}

	void OgreGraphicsSystem::ChangeCamera(const std::string &vp_name, OgreCameraComponentPtr cam_comp)
	{
		if(vp_name == "ALL")
		{
			std::map<std::string,Viewport>::iterator iter = m_Viewports.begin();
			while(iter != m_Viewports.end())
			{

				if(iter->second.m_OgreViewport)
					iter->second.m_OgreViewport->setCamera(cam_comp->GetOgreCamera());
				++iter;
			}
		}
		else if(m_Viewports.find(vp_name) != m_Viewports.end() && m_Viewports[vp_name].m_OgreViewport != NULL)
		{
			m_Viewports[vp_name].m_OgreViewport->setCamera(cam_comp->GetOgreCamera());
		}
	}
}





