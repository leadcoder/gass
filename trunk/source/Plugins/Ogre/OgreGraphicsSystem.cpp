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
#include "Core/MessageSystem/Message.h"
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
	OgreGraphicsSystem::OgreGraphicsSystem(void): m_Window(NULL),
		m_CreateMainWindowOnInit(true),
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
		RegisterProperty<std::vector<std::string> >("PostFilters", &GASS::OgreGraphicsSystem::GetPostFilters, &GASS::OgreGraphicsSystem::SetPostFilters);
		RegisterProperty<bool>("CreateMainWindowOnInit", &GASS::OgreGraphicsSystem::GetCreateMainWindowOnInit, &GASS::OgreGraphicsSystem::SetCreateMainWindowOnInit);
		RegisterProperty<TaskGroup>("TaskGroup", &GASS::OgreGraphicsSystem::GetTaskGroup, &GASS::OgreGraphicsSystem::SetTaskGroup);
	}

	void OgreGraphicsSystem::OnCreate()
	{
		int address = (int) this;
		GetSimSystemManager()->RegisterForMessage(SimSystemManager::SYSTEM_RM_INIT,					MESSAGE_FUNC( OgreGraphicsSystem::OnInit));
		GetSimSystemManager()->RegisterForMessage(SimSystemManager::SYSTEM_RM_CREATE_RENDER_WINDOW,	MESSAGE_FUNC( OgreGraphicsSystem::OnCreateRenderWindow ));
		GetSimSystemManager()->RegisterForMessage(SimSystemManager::SYSTEM_NM_WINDOW_MOVED_OR_RESIZED, MESSAGE_FUNC( OgreGraphicsSystem::OnWindowMovedOrResized ));
		GetSimSystemManager()->RegisterForMessage(SimSystemManager::SYSTEM_RM_DEBUG_PRINT,				MESSAGE_FUNC( OgreGraphicsSystem::OnDebugPrint));
	}



	//Create custom load
	/*void OgreGraphicsSystem::Load(TiXmlElement *elem)
	{
		TiXmlElement *attrib = elem->FirstChildElement();
		while(attrib)
		{
			std::string attrib_name = attrib->Value();

			if(attrib_name == "AddViewport")
			{
				ViewportData vpd;
				rl.m_SMName = attrib->Attribute("SceneManager");
				rl.m_WindowName = attrib->Attribute("Window");
				rl.m_LeftX = atof(attrib->Attribute("LeftX"));
				rl.m_RightX = atof(attrib->Attribute("RightX"));
				rl.m_TopY = atof(attrib->Attribute("TopY"));
				rl.m_BottomY = atof(attrib->Attribute("BottomY"));
			}
			else
			{
				std::string attrib_val = attrib->FirstAttribute()->Value();
				SetProperty(attrib_name,attrib_val);
			}
			attrib  = attrib->NextSiblingElement();
		}
	}*/

	void OgreGraphicsSystem::OnInit(MessagePtr message)
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
		TaskGroup group = m_TaskGroup;
		if(m_Root->getRenderSystem()->getName().find("GL") != Ogre::String::npos)
			group = MAIN_TASK_GROUP;

		SimEngine::GetPtr()->GetRuntimeController()->Register(boost::bind( &OgreGraphicsSystem::Update, this, _1 ),group);

		if(m_CreateMainWindowOnInit)
		{
			m_Window = m_Root->initialise(true);
			m_SceneMgr = m_Root->createSceneManager("TerrainSceneManager");
			Camera* cam = m_SceneMgr->createCamera("DefaultViewportCamera0");
			cam->setPosition(Vector3(0,2,0));
			cam->lookAt(Vector3(0,2,-1));
			cam->setNearClipDistance(1.2f);
			cam->setFarClipDistance(25000);
			// Create one viewport, entire window
			Viewport *vp = m_Window->addViewport(cam);
			vp->setBackgroundColour(ColourValue(0.5,0,0));
			// Alter the camera aspect ratio to match the viewport
			cam->setAspectRatio( Real(vp->getActualWidth()) / Real(vp->getActualHeight()));

			size_t windowHnd = 0;
			m_Window->getCustomAttribute("WINDOW", &windowHnd);

			//IInputSystem*  is = GetOwner()->GetFirstSystem<IInputSystem>();
			//is->SetWindow(windowHnd);
			MessagePtr window_msg(new Message(SimSystemManager::SYSTEM_NM_MAIN_WINDOW_CREATED));
			window_msg->SetData("RenderHandle",(int)windowHnd);
			window_msg->SetData("MainHandle",(int)windowHnd);
			GetSimSystemManager()->SendImmediate(window_msg);




		}
		else
		{
			m_Root->initialise(false);
		}
		//MessagePtr update_msg(new Message(SystemManager::SYSTEM_RM_UPDATE,(int) this));
		//update_msg->m_Timer = 1.0/100.0f; //update with 100hz
		//m_Owner->GetMessageManager()->PostMessage(update_msg);
	}


	void OgreGraphicsSystem::OnDebugPrint(MessagePtr message)
	{
		std::string debug_text = boost::any_cast<std::string>(message->GetData("Text"));
		m_DebugTextBox->Print(debug_text.c_str());
		m_DebugTextBox->SetActive(true);
	}

	void OgreGraphicsSystem::OnCreateRenderWindow(MessagePtr message)
	{
		std::string name = boost::any_cast<std::string>(message->GetData("Name"));
		int height = boost::any_cast<int>(message->GetData("Height"));
		int width = boost::any_cast<int>(message->GetData("Width"));
		int handel = boost::any_cast<int>(message->GetData("Handle"));

		if(m_Window == NULL) // first window
		{
			int main_handel = boost::any_cast<int>(message->GetData("MainHandle"));
			Ogre::NameValuePairList miscParams;
			miscParams["externalWindowHandle"] = Ogre::StringConverter::toString((size_t)handel);
			m_Window = Ogre::Root::getSingleton().createRenderWindow(name,width, height, false, &miscParams);

			m_SceneMgr = m_Root->createSceneManager("TerrainSceneManager");
			Camera* cam = m_SceneMgr->createCamera("DefaultViewportCamera0");
			cam->setPosition(Vector3(0,2,0));
			cam->lookAt(Vector3(0,2,-1));
			cam->setNearClipDistance(0.02f);
			cam->setFarClipDistance(5000);
			// Create one viewport, entire window
			Viewport *vp = m_Window->addViewport(cam);
			vp->setBackgroundColour(ColourValue(0.5,0,0));
			// Alter the camera aspect ratio to match the viewport
			cam->setAspectRatio( Real(vp->getActualWidth()) / Real(vp->getActualHeight()));

			MessagePtr window_msg(new Message(SimSystemManager::SYSTEM_NM_MAIN_WINDOW_CREATED));
			window_msg->SetData("RenderHandle",(int)handel);
			window_msg->SetData("MainHandle",main_handel);
			GetSimSystemManager()->SendImmediate(window_msg);


		}
	}

	void OgreGraphicsSystem::OnWindowMovedOrResized(MessagePtr message)
	{
		m_Window->windowMovedOrResized();
		/*int handel = boost::any_cast<int>(message->GetData("Handle"));
		size_t main_window_handel = 0;
		m_Window->getCustomAttribute("WINDOW", &main_window_handel);

		if(handel == main_window_handel)
		{

		}*/
	}




	void OgreGraphicsSystem::SetActiveSceneManger(Ogre::SceneManager *sm)
	{
		m_Window->removeAllViewports();
		AddViewport(sm,m_Window, 0, 0, 1, 1, Ogre::ColourValue (0,0,0));
	}

	void OgreGraphicsSystem::GetMainWindowInfo(unsigned int &width, unsigned int &height, int &left, int &top)
	{
		unsigned int depth;
		if(m_Window)
			m_Window->getMetrics(width, height, depth, left, top);
	}

	void OgreGraphicsSystem::Update(double delta_time)
	{
		//boost::shared_ptr<UpdateMessage> update_msg = boost::static_pointer_cast<UpdateMessage>( message);
		//if(((int) update_msg->m_Tick) % 10 == 0)
		static int tick = 0;
		//std::cout << " tick:" << tick++ <<std::endl;

		//set thread priority to highest!!
		WindowEventUtilities::messagePump();
		m_Root->renderOneFrame();

		m_DebugTextBox->SetActive(true);
		m_DebugTextBox->UpdateTextBox();

		//Update all scene managers
		/*for(int  i = 0 ;  m_SceneManagers.size(); i++)
		{
			m_SceneManagers[i]->Update(delta_time);
		}*/
	}

	void OgreGraphicsSystem::AddViewport(Ogre::SceneManager *sm, Ogre::RenderWindow* win, float left , float top, float width , float height,Ogre::ColourValue colour)
	{
		int num_viewports = win->getNumViewports();
		std::stringstream ss;
		ss << "DefaultViewportCamera" << num_viewports;
		std::string name;
		ss >> name;

		Ogre::Camera* cam;
		if(sm->hasCamera(name))
			sm->getCamera(name);
		else
			cam = sm->createCamera(name);

		cam->setPosition(Ogre::Vector3(0,0,0));
		cam->setNearClipDistance(0.02f);
		cam->setFarClipDistance(5000);
		assert(cam && win);
		Ogre::Viewport* vp = win->addViewport(cam, num_viewports, left , top, width , height);

		// Create one viewport, entire window
		vp->setBackgroundColour(colour);
		//Alter the camera aspect ratio to match the viewport
		cam->setAspectRatio( Ogre::Real(vp->getActualWidth())/Ogre::Real(vp->getActualHeight()));

		if(m_PostProcess)
		{
			m_PostProcess.reset();
		}
		m_PostProcess = OgrePostProcessPtr(new OgrePostProcess(vp));
		m_PostProcess->SetActiveCompositors(GetPostFilters());

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
}





