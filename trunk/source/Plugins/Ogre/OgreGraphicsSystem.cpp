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
#include "Plugins/Ogre/Helpers/DebugDrawer.h"
#include "Plugins/Ogre/OgreConvert.h"
#include "Plugins/Ogre/Helpers/OgreText.h"

#include "Core/System/GASSSystemFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"

#include "Sim/Interface/GASSIInputSystem.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"
#include <boost/bind.hpp>

#include <OgreRoot.h>
#include <OgreRenderWindow.h>
#include <OgreRenderSystem.h>
#include <OgreWindowEventUtilities.h>
#include <OgreStringConverter.h>
#include <OgreLogManager.h>



namespace GASS
{
	OgreGraphicsSystem::OgreGraphicsSystem(void): m_CreateMainWindowOnInit(true), 
		m_SceneMgr(NULL),
		m_UpdateMessagePump(true),
		m_DebugTextBox (new OgreDebugTextOutput())
	{
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
		RegisterProperty<bool>("UpdateMessagePump", &GASS::OgreGraphicsSystem::GetUpdateMessagePump, &GASS::OgreGraphicsSystem::SetUpdateMessagePump);
		RegisterProperty<bool>("ShowStats", &GASS::OgreGraphicsSystem::GetShowStats, &GASS::OgreGraphicsSystem::SetShowStats);
	}

	void OgreGraphicsSystem::Init()
	{
		SimEngine::Get().GetRuntimeController()->Register(shared_from_this(),m_TaskNodeName);
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OgreGraphicsSystem::OnViewportMovedOrResized,ViewportMovedOrResizedNotifyMessage,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OgreGraphicsSystem::OnDebugPrint,DebugPrintMessage,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OgreGraphicsSystem::OnDrawLine,DrawLineMessage ,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OgreGraphicsSystem::OnDrawCircle,DrawCircleMessage ,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OgreGraphicsSystem::OnInitializeTextBox,CreateTextBoxMessage ,0));

		//Load plugins
		m_Root = new Ogre::Root("","ogre.cfg","ogre.log");

		//Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_LOW);

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
		//if(m_Root->getRenderSystem()->getName().find("GL") != Ogre::String::npos)
		//	m_TaskGroup = MAIN_TASK_GROUP;

		if(m_CreateMainWindowOnInit)
		{
			std::string name = "MainWindow";
			Ogre::RenderWindow *window = m_Root->initialise(true,name);
			window->setDeactivateOnFocusChange(false);
			void* window_hnd = 0;
			window->getCustomAttribute("WINDOW", &window_hnd);
			void* handle = static_cast<void*>(window_hnd);
			void* main_handle = static_cast<void*>(window_hnd);
			m_Windows[name] = window;
			//We send a message when this window is cretated, usefull for other plugins to get hold of windows handle
			MessagePtr window_msg(new MainWindowCreatedNotifyMessage(handle,main_handle));
			GetSimSystemManager()->SendImmediate(window_msg);
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
		//resize all 
		while(iter != m_Windows.end())
		{
			iter->second->windowMovedOrResized();
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

		//re-add all viewports?
		std::map<std::string, Viewport>::iterator vp_iter = m_Viewports.begin();
		while(vp_iter != m_Viewports.end())
		{
			AddViewport(sm,vp_iter->second.m_Name,vp_iter->second.m_Window,vp_iter->second.m_Left,vp_iter->second.m_Top,vp_iter->second.m_Width,vp_iter->second.m_Height,Ogre::ColourValue(),vp_iter->second.m_ZDepth);
			++vp_iter;
		}
	}

	void OgreGraphicsSystem::GetMainWindowInfo(unsigned int &width, unsigned int &height, int &left, int &top) const
	{
		unsigned int depth;
		if(m_Windows.size() > 0)
			m_Windows.begin()->second->getMetrics(width, height, depth, left, top);
	}

	void OgreGraphicsSystem::Update(double delta_time)
	{
		if(m_UpdateMessagePump) //take care of window events?
			Ogre::WindowEventUtilities::messagePump();

		if(DebugDrawer::getSingletonPtr())
			DebugDrawer::getSingleton().build();

		m_Root->renderOneFrame();

		if(DebugDrawer::getSingletonPtr())
			DebugDrawer::getSingleton().clear();

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
		//update listeners
		SimSystem::Update(delta_time);
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

		//add post process to all windows, change this to camera effect instead?
		if(m_PostProcess)
		{
			m_PostProcess.reset();
		}

		m_PostProcess = OgrePostProcessPtr(new OgrePostProcess(vp));
		m_PostProcess->SetActiveCompositors(GetPostFilters());

		//save viewport settings for recreation when scene manager is changed
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

	void OgreGraphicsSystem::CreateRenderWindow(const std::string &name, int width, int height, void* handle, void* main_handle)
	{
		Ogre::RenderWindow *window = NULL;

		if(m_Windows.find(name) != m_Windows.end())
			return;

		if(handle)
		{
			Ogre::NameValuePairList miscParams;
			miscParams["externalWindowHandle"] = Ogre::StringConverter::toString((size_t)handle);
			window = Ogre::Root::getSingleton().createRenderWindow(name,width, height, false, &miscParams);
		}
		else
		{
			window = Ogre::Root::getSingleton().createRenderWindow(name,width, height, false);
			if(main_handle == 0)
			{
				void* window_hnd = 0;
				window->getCustomAttribute("WINDOW", &window_hnd);
				handle = window_hnd;
				main_handle = window_hnd;
			}
		}
		m_Windows[name] = window;
		if(m_Windows.size() == 1) // first window?
		{
			//We send a message when this window is cretated, usefull for other plugins to get hold of windows handle
			MessagePtr window_msg(new MainWindowCreatedNotifyMessage(handle,main_handle));
			GetSimSystemManager()->SendImmediate(window_msg);
		}
	}

	void OgreGraphicsSystem::CreateViewport(const std::string &name, const std::string &render_window, float left, float top, float  width, float height)
	{
		//check that this window exist!
		Ogre::RenderWindow* window = m_Windows[render_window];
		if(window)
		{
			//we need a scene mananger before we can create viewports, 
			//"TerrainSceneManager" will do for now, this will change when scene is loaded by OgreSceneManager class
			if(m_SceneMgr == NULL) 
				m_SceneMgr = m_Root->createSceneManager("TerrainSceneManager");

			if(m_Viewports.find(name) != m_Viewports.end())
				return;
			AddViewport(m_SceneMgr, name, render_window, left , top, width , height,Ogre::ColourValue(),static_cast<int>(m_Viewports.size()));
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

	void OgreGraphicsSystem::RemoveViewport(const std::string &name, const std::string &win_name)
	{
		if(m_Viewports.find(name) != m_Viewports.end() && m_Viewports[name].m_OgreViewport != NULL)
		{
			m_Windows[win_name]->removeViewport(m_Viewports[name].m_ZDepth);
		}
	}

	void OgreGraphicsSystem::OnDrawLine(DrawLineMessagePtr message)
	{
		Vec4 color = message->GetColor();
		Ogre::ColourValue ogre_color(color.x,color.y,color.z,color.w);
		if(DebugDrawer::getSingletonPtr())
			DebugDrawer::getSingleton().drawLine(Convert::ToOgre(message->GetStart()),Convert::ToOgre(message->GetEnd()),ogre_color);		
	}


	void OgreGraphicsSystem::OnDrawCircle(DrawCircleMessagePtr message)
	{
		Vec4 color = message->GetColor();
		Ogre::ColourValue ogre_color(color.x,color.y,color.z,color.w);
		if(DebugDrawer::getSingletonPtr())
		{

			DebugDrawer::getSingleton().drawCircle(Convert::ToOgre(message->GetCenter()),message->GetRadius(),message->GetSegments(),ogre_color,message->GetFilled());		
		}
	}

	void OgreGraphicsSystem::OnInitializeTextBox(CreateTextBoxMessagePtr message)
	{
		/*Vec4 color = message->m_Color;
		Ogre::ColourValue ogre_color(color.x,color.y,color.z,color.w);

		if(!TextRenderer::getSingleton().hasTextBox(message->m_BoxID))
		TextRenderer::getSingleton().addTextBox(message->m_BoxID, message->m_Text, message->m_PosX, message->m_PosY, message->m_Width, message->m_Width, ogre_color);
		else
		TextRenderer::getSingleton().setText(message->m_BoxID,message->m_Text);*/
	}
}





