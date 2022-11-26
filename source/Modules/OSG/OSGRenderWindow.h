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
#include "Modules/OSG/OSGCommon.h"

namespace GASS
{
	class OSGGraphicsSystem;
	class OSGViewport;
	using OSGViewportPtr = std::shared_ptr<OSGViewport>;

	class OSGRenderWindow  : public IRenderWindow
	{
	public:
		using OSGViewportVector = std::vector<OSGViewportPtr>;

		OSGRenderWindow(OSGGraphicsSystem* system, osg::ref_ptr<osg::GraphicsContext> win);
		~OSGRenderWindow() override;
		
		//IRenderWindow
		unsigned int GetWidth() const override;
		unsigned int GetHeight() const override;
		void* GetHWND() const override;
		ViewportVector GetViewports() const override;
		ViewportPtr CreateViewport(const std::string &name, float  left, float top, float width, float height) override;

		OSGGraphicsSystem* GetSystem() const{return m_System;}
		osg::ref_ptr<osg::GraphicsContext> GetOSGWindow() const {return m_Window;}
	private:
		osg::ref_ptr<osg::GraphicsContext> m_Window;
		ViewportVector m_Viewports;
		OSGGraphicsSystem* m_System;
	};
	using OSGRenderWindowPtr = std::shared_ptr<OSGRenderWindow>;
}
