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

#include "Sim/Interface/GASSIViewport.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"
#include "Plugins/OSG/OSGCommon.h"
#include "Plugins/OSG/OSGRenderWindow.h"

namespace GASS
{
	class OSGViewport : public IViewport, public GASS_ENABLE_SHARED_FROM_THIS<OSGViewport>, public IMessageListener
	{
		friend class OSGRenderWindow;
	public:
		OSGViewport(const std::string &name,osgViewer::View* view, OSGRenderWindow* window);
		~OSGViewport() override;
		CameraComponentPtr GetCamera() const override;
		void SetCamera(CameraComponentPtr camera) override;
		std::string GetName() const override {return m_Name;}
		osg::Camera* GetOSGCamera() const {return m_OSGCamera;}
	private:
		void Init();
		osg::ref_ptr<osg::Camera> m_OSGCamera; 
		std::string m_Name;
		OSGRenderWindow* m_Window;
		CameraComponentWeakPtr m_Camera;
		osgViewer::View* m_OSGView;
	};
	using OSGViewportPtr = std::shared_ptr<OSGViewport>;
}
