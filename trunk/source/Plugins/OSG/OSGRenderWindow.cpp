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
#include "Plugins/OSG/OSGRenderWindow.h"
#include "Plugins/OSG/OSGViewport.h"
#include "Plugins/OSG/OSGGraphicsSystem.h"
#include "Core/Utils/GASSException.h"

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
		void* win_handle = NULL;
		#if defined(WIN32) && !defined(__CYGWIN__) 	
			osgViewer::GraphicsWindowWin32* win32_window = (osgViewer::GraphicsWindowWin32*)(m_Window.get());
			win_handle = (void*) win32_window->getHWND();
		#endif
		return win_handle;
	}

	ViewportPtr OSGRenderWindow::CreateViewport(const std::string &name, float  left, float top, float width, float height)
	{
		osg::Viewport* osg_vp = new osg::Viewport(left, top, width,height);
		OSGViewportPtr vp(new OSGViewport(name,osg_vp, this));
		m_Viewports.push_back(vp);
		return vp;
	}


	unsigned int OSGRenderWindow::GetWidth() const {return m_Window->getTraits()->width;}
	unsigned int OSGRenderWindow::GetHeight() const {return m_Window->getTraits()->height;}
	

}





