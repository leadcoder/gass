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

#pragma once

#include "Sim/Interface/GASSIRenderWindow.h"
#include "Plugins/OSG/OSGCommon.h"

namespace GASS
{
	class OSGGraphicsSystem;
	class OSGViewport;
	typedef GASS_SHARED_PTR<OSGViewport> OSGViewportPtr;

	class OSGRenderWindow  : public IRenderWindow
	{
	public:
		typedef std::vector<OSGViewportPtr> OSGViewportVector;

		OSGRenderWindow(OSGGraphicsSystem* system, osg::ref_ptr<osg::GraphicsContext> win);
		~OSGRenderWindow();
		
		//IRenderWindow
		virtual unsigned int GetWidth() const;
		virtual unsigned int GetHeight() const;
		virtual void* GetHWND() const;
		OSGGraphicsSystem* GetSystem() const{return m_System;}
		osg::ref_ptr<osg::GraphicsContext> GetOSGWindow() const {return m_Window;}
		ViewportPtr CreateViewport(const std::string &name, float  left, float top, float width, float height);
		OSGViewportVector GetViewports() const {return m_Viewports;}
	private:
		osg::ref_ptr<osg::GraphicsContext> m_Window;
		OSGViewportVector m_Viewports;
		OSGGraphicsSystem* m_System;
	};
	typedef GASS_SHARED_PTR<OSGRenderWindow> OSGRenderWindowPtr;
}
