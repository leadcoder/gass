/****************************************************************************
* This file is part of GASS.                                                *
* See http://sourceforge.net/projects/gass/                                 *
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

#   pragma warning (disable : 4541)
#include <osgDB/ReadFile>
#include <osgSim/MultiSwitch>
#include <osg/MatrixTransform>
#include <osg/CullSettings>
#include <osg/PositionAttitudeTransform>
#include <osgGA/TrackballManipulator>
#include <osgViewer/CompositeViewer>
#include <osgGA/StateSetManipulator>


#if defined(WIN32) && !defined(__CYGWIN__) 
#include <osgViewer/api/Win32/GraphicsWindowWin32> 
typedef HWND WindowHandle; 
typedef osgViewer::GraphicsWindowWin32::WindowData WindowData; 
#elif defined(__APPLE__) // Assume using Carbon on Mac. 
#include <osgViewer/api/Carbon/GraphicsWindowCarbon> 
typedef WindowRef WindowHandle; 
typedef osgViewer::GraphicsWindowCarbon::WindowData WindowData; 
#else // all other unix 
#include <osgViewer/api/X11/GraphicsWindowX11> 
typedef Window WindowHandle; 
typedef osgViewer::GraphicsWindowX11::WindowData WindowData; 
#endif 



#include "Plugins/OSG/OSGGraphicsSystem.h"
#include "Core/System/SystemFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Sim/Scheduling/IRuntimeController.h"
#include "Sim/Systems/Input/IInputSystem.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/SimEngine.h"
#include <boost/bind.hpp>
#include <osgDB/ReadFile> 




namespace GASS
{
	OSGGraphicsSystem::OSGGraphicsSystem(void) 
	{


	}

	OSGGraphicsSystem::~OSGGraphicsSystem(void)
	{

	}

	void OSGGraphicsSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register("OSGGraphicsSystem",new GASS::Creator<OSGGraphicsSystem, ISystem>);
		RegisterProperty<bool>("CreateMainWindowOnInit", &GASS::OSGGraphicsSystem::GetCreateMainWindowOnInit, &GASS::OSGGraphicsSystem::SetCreateMainWindowOnInit);
		//RegisterProperty<std::string>( "Plugin", NULL, &GASS::OSGGraphicsSystem::AddPlugin);
	}

	void OSGGraphicsSystem::OnCreate()
	{
		SimEngine::GetPtr()->GetRuntimeController()->Register(this);
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OSGGraphicsSystem::OnInit,InitMessage,0));
		GetSimSystemManager()->RegisterForMessage(REG_TMESS(OSGGraphicsSystem::OnCreateRenderWindow,CreateRenderWindowMessage,0));
	}

	void OSGGraphicsSystem::SetActiveData(osg::Group* root)
	{

		osgViewer::ViewerBase::Views views;
		m_Viewer->getViews(views);

		//set same scene in all viewports for the moment
		for(int i = 0; i < views.size(); i++)
		{
			views[i]->setSceneData(root);
		}
		//m_Viewer->realize();
	}

	void OSGGraphicsSystem::OnInit(MessagePtr message)
	{

		m_Viewer = new osgViewer::CompositeViewer();




		if(m_CreateMainWindowOnInit)
		{

			osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
			if (!wsi) 
			{
				osg::notify(osg::NOTICE)<<"Error, no WindowSystemInterface available, cannot create windows."<<std::endl;
				return;
			}

			osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
			traits->x = 100;
			traits->y = 100;
			traits->width = 800;
			traits->height = 600;
			traits->windowDecoration = true;
			traits->doubleBuffer = true;
			traits->sharedContext = 0;
			traits->windowName = "OSG Render window";



			m_GraphicsContext = osg::GraphicsContext::createGraphicsContext(traits.get());
			if (m_GraphicsContext.valid())
			{
				osg::notify(osg::INFO)<<"  GraphicsWindow has been created successfully."<<std::endl;

				//need to ensure that the window is cleared make sure that the complete window is set the correct colour
				//rather than just the parts of the window that are under the camera's viewports
				m_GraphicsContext->setClearColor(osg::Vec4f(0.8f,0.0f,0.0f,1.0f));
				m_GraphicsContext->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}
			else
			{
				osg::notify(osg::NOTICE)<<"  GraphicsWindow has not been created successfully."<<std::endl;
			}

			//osg::ref_ptr<osg::Group> rootNode;
			//rootNode = new osg::Group();
			CreateView(m_Viewer,m_GraphicsContext,  0, 0, traits->width, traits->height);

			m_Viewer->setThreadingModel( osgViewer::Viewer::SingleThreaded );

			m_Viewer->frame();
			size_t windowHnd = 0;

			//there must be a better way to gte the window handle!
#if defined(WIN32) && !defined(__CYGWIN__) 	
			osgViewer::GraphicsWindowWin32* win32_window = (osgViewer::GraphicsWindowWin32*)(m_GraphicsContext.get());
			windowHnd = (size_t) win32_window->getHWND();
#endif
			MessagePtr window_msg(new MainWindowCreatedNotifyMessage((int)windowHnd,(int)windowHnd));
			GetSimSystemManager()->SendImmediate(window_msg);
		}

		/*osgDB::ReaderWriter::Options* opt = osgDB::Registry::instance()->getOptions(); 
		if (opt == NULL) { 
			opt = new osgDB::ReaderWriter::Options(); 
		} 
		opt->setObjectCacheHint(osgDB::ReaderWriter::Options::CACHE_ALL); 
		osgDB::Registry::instance()->setOptions(opt); */

		/*	IInputSystem*  is = GetOwner()->GetFirstSystem<IInputSystem>();
		if(is)
		{
		is->SetWindow(windowHnd);
		}*/

		/*boost::shared_ptr<Message> update_msg(new Message(SimSystemManager::SYSTEM_MESSAGE_UPDATE,(int) this));
		update_msg->m_Timer = 1.0/30.0f; //update with 100hz
		GetMessageManager()->PostMessage(update_msg);*/
	}

	void OSGGraphicsSystem::CreateView(osgViewer::CompositeViewer *viewer,
		//osg::ref_ptr<osg::Group> scene,
		osg::ref_ptr<osg::GraphicsContext> gc,
		int x, int y, int width, int height)
	{
		//		double left,right,top,bottom, near_clip;
		//double far_clip;
		//double aspectratio;
		//double frusht, fruswid, fudge;
		//		bool gotfrustum;
		osgViewer::View* view = new osgViewer::View;
		viewer->addView(view);

		//view->setCameraManipulator(Tman);
		//view->setSceneData(scene.get());
		view->getCamera()->setViewport(new osg::Viewport(x, y, width,height));
		view->getCamera()->setComputeNearFarMode(osgUtil::CullVisitor::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);

		/*osgGA::TrackballManipulator *Tman1 = new osgGA::TrackballManipulator();

		view->setCameraManipulator(Tman1);

		osg::ref_ptr<osgGA::StateSetManipulator> statesetManipulator = new osgGA::StateSetManipulator;
		statesetManipulator->setStateSet(view->getCamera()->getOrCreateStateSet());

		view->addEventHandler( statesetManipulator.get() );*/


		/*	view->getCamera()->getProjectionMatrixAsFrustum(left, right, bottom, top, near_clip, far_clip);
		if (gotfrustum) 
		{
		aspectratio = (double) width/ (double) height; 
		frusht = top - bottom;
		fruswid = right - left;
		fudge = frusht*aspectratio/fruswid;
		right = right*fudge;
		left = left*fudge;
		view->getCamera()->setProjectionMatrixAsFrustum(left,right,
		bottom,top,
		near_clip,far_clip);
		}*/
		view->getCamera()->setGraphicsContext(gc.get());
		// add the state manipulator
		//  osg::ref_ptr<osgGA::StateSetManipulator> statesetManipulator = new osgGA::StateSetManipulator;
		// statesetManipulator->setStateSet(view->getCamera()->getOrCreateStateSet());
		// view->addEventHandler( statesetManipulator.get() );


	}

	void OSGGraphicsSystem::OnCreateRenderWindow(CreateRenderWindowMessagePtr message)
	{
		std::string name = message->GetName();
		int height = message->GetHeight();
		int width = message->GetWidth();
		int handel = message->GetHandle();

		if(!m_GraphicsContext) // first window
		{
			int main_handel = message->GetMainHandle();
			// Init the Windata Variable that holds the handle for the Window to display OSG in.
			osg::ref_ptr<osg::Referenced> windata = new osgViewer::GraphicsWindowWin32::WindowData((HWND)handel);
			osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;



			traits->x = 0;
			traits->y = 0;
			traits->width = width;
			traits->height = height;
			traits->windowDecoration = false;
			traits->doubleBuffer = true;
			traits->sharedContext = 0;
			traits->setInheritedWindowPixelFormat = true;
			traits->inheritedWindowData = windata;



			m_GraphicsContext = osg::GraphicsContext::createGraphicsContext(traits.get());
			if (m_GraphicsContext.valid())
			{
				osg::notify(osg::INFO)<<"  GraphicsWindow has been created successfully."<<std::endl;

				//need to ensure that the window is cleared make sure that the complete window is set the correct colour
				//rather than just the parts of the window that are under the camera's viewports
				m_GraphicsContext->setClearColor(osg::Vec4f(0.8f,0.0f,0.0f,1.0f));
				m_GraphicsContext->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}
			else
			{
				osg::notify(osg::NOTICE)<<"  GraphicsWindow has not been created successfully."<<std::endl;
			}

			//osg::ref_ptr<osg::Group> rootNode;
			//rootNode = new osg::Group();
			CreateView(m_Viewer,m_GraphicsContext,  0, 0, traits->width, traits->height);

			m_Viewer->setThreadingModel( osgViewer::Viewer::SingleThreaded );

			m_Viewer->frame();

			MessagePtr window_msg(new MainWindowCreatedNotifyMessage(message->GetHandle(),message->GetMainHandle()));
			GetSimSystemManager()->SendImmediate(window_msg);
		}
	}


	void OSGGraphicsSystem::GetMainWindowInfo(unsigned int &width, unsigned int &height, int &left, int &top)
	{
		//		unsigned int depth;

		if(m_GraphicsContext.get())
		{
			const osg::GraphicsContext::Traits* traits = m_GraphicsContext->getTraits();
			width = traits->width;
			height = traits->height;
			left = traits->x;
			top = traits->y;
		}
	}


	void OSGGraphicsSystem::OnWindowMovedOrResized(MainWindowMovedOrResizedNotifyMessagePtr message)
	{
		//m_Window->windowMovedOrResized();
		//m_Viewer->getView(0)->
	}

	void OSGGraphicsSystem::Update(double delta_time)
	{
		static int tick = 0;
		m_Viewer->frame(delta_time);

		if(m_Viewer->done())
		{
			//Exit
			Log::Error("Exit");
		}
		//WindowEventUtilities::messagePump();
		//m_Root->renderOneFrame();
	}

	TaskGroup OSGGraphicsSystem::GetTaskGroup() const
	{
		return MAIN_TASK_GROUP;
	}
}




