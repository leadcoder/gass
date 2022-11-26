/****************************************************************************
* This file is part of GASS.                                                *
* See https://github.com/leadcoder/gass                                     *
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
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
#include "Modules/OSG/OSGImGuiHandler.h"
#include "Modules/OSG/OSGRenderWindow.h"
#include "Modules/OSG/OSGViewport.h"
#include "Modules/OSG/OSGGraphicsSystem.h"
#include "Modules/OSG/OSGInputSystem.h"
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/StateSetManipulator>

#if defined(WIN32) && !defined(__CYGWIN__) 
	#include <osgViewer/api/Win32/GraphicsWindowWin32>
	using WindowHandle = HWND; 
	using WindowData = osgViewer::GraphicsWindowWin32::WindowData; 
#elif defined(__APPLE__) // Assume using Carbon on Mac. 
	#include <osgViewer/api/Carbon/GraphicsWindowCarbon> 
	typedef WindowRef WindowHandle; 
	typedef osgViewer::GraphicsWindowCarbon::WindowData WindowData; 
#else // all other unix 
	#include <osgViewer/api/X11/GraphicsWindowX11> 
	typedef Window WindowHandle; 
	typedef osgViewer::GraphicsWindowX11::WindowData WindowData; 
#endif 

namespace GASS
{
	OSGRenderWindow::OSGRenderWindow(OSGGraphicsSystem* system, osg::ref_ptr<osg::GraphicsContext> win) : m_System(system), 
		m_Window(win)
	{

	}

	OSGRenderWindow::~OSGRenderWindow()
	{

	}

	void* OSGRenderWindow::GetHWND() const
	{
		void* win_handle = nullptr;
		#if defined(WIN32) && !defined(__CYGWIN__) 	
			auto* win32_window = (osgViewer::GraphicsWindowWin32*)(m_Window.get());
			win_handle = (void*) win32_window->getHWND();
		#else
			osgViewer::GraphicsWindowX11* x11_window = (osgViewer::GraphicsWindowX11*)(m_Window.get());
			win_handle = (void*)x11_window->getWindow();
		#endif
		return win_handle;
	}

	ViewportPtr OSGRenderWindow::CreateViewport(const std::string &name, float  left, float top, float width, float height)
	{
		//convert dimensions normalized to pixel
		auto win_width = static_cast<float>(m_Window->getTraits()->width);
		auto win_height = static_cast<float>(m_Window->getTraits()->height);

		int p_left = static_cast<int>(left*win_width);
		int p_top = static_cast<int>(top*win_height);

		int p_width = static_cast<int>(width*win_width);
		int p_height = static_cast<int>(height*win_height);
		auto* osg_vp = new osg::Viewport(p_left, p_top, p_width,p_height);
		osg::ref_ptr<osg::Camera> camera = new osg::Camera;
	    camera->setGraphicsContext(m_Window.get());
		camera->setDrawBuffer(GL_BACK);
		camera->setReadBuffer(GL_BACK);
		camera->setViewport(osg_vp);
		

		//camera->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
		camera->getOrCreateStateSet()->setGlobalDefaults();

		auto* view = new osgViewer::View;
		view->setName(name);
		view->setCamera(camera);
		
		//move this to scene manager
		//view->setLightingMode(osg::View::SKY_LIGHT); 
		view->setLightingMode(osg::View::NO_LIGHT);
		view->getDatabasePager()->setDoPreCompile( true );
		view->getDatabasePager()->setTargetMaximumNumberOfPageLOD(100);
		
		OSGGraphicsSystemPtr system = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<OSGGraphicsSystem>();
		
		// add some OSG handlers, maybe only in first view?
		auto* stats = new osgViewer::StatsHandler();
		stats->setKeyEventTogglesOnScreenStats('y');
		stats->setKeyEventPrintsOutStats('¤');

		view->addEventHandler(stats);

		//view->addEventHandler(new osgViewer::WindowSizeHandler());
		view->addEventHandler(new osgViewer::ThreadingHandler());
		view->addEventHandler(new osgViewer::LODScaleHandler());


	

		auto* ssm =  new osgGA::StateSetManipulator(view->getCamera()->getOrCreateStateSet());
		ssm->setKeyEventCyclePolygonMode('p');
		ssm->setKeyEventToggleTexturing('o');
		view->addEventHandler(ssm);
		
		system->GetViewer()->addView(view);

		system->GetViewer()->realize();
		view->addEventHandler(new OSGImGuiHandler());

		if (auto is = SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystemByClass<OSGInputSystem>(true))
			view->addEventHandler(new OSGInputHandler(is.get()));

		OSGViewportPtr vp(new OSGViewport(name,view, this));
		
		vp->Init();

		

		m_Viewports.push_back(vp);
		return vp;
	}

	unsigned int OSGRenderWindow::GetWidth() const 
	{
		return m_Window->getTraits()->width;
	}

	unsigned int OSGRenderWindow::GetHeight() const 
	{
		return m_Window->getTraits()->height;
	}

	ViewportVector OSGRenderWindow::GetViewports() const
	{
		return m_Viewports;
	}
}