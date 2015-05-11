/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                        *
*                                                                           *
* Copyright (c) 2008-2015 GASS team. See Contributors.txt for details.      *
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
#include "Plugins/OSG/OSGRenderWindow.h"
#include <string>

namespace GASS
{
	class OSGViewport : public IViewport, public SHARE_CLASS<OSGViewport>, public IMessageListener
	{
		friend class OSGRenderWindow;
	public:
		OSGViewport(const std::string &name,osgViewer::View* view, OSGRenderWindow* window);
		virtual ~OSGViewport();
		virtual CameraComponentPtr GetCamera() const;
		virtual void SetCamera(CameraComponentPtr camera);
		virtual std::string GetName() const {return m_Name;}
		osg::Camera* GetOSGCamera() const {return m_OSGCamera;}
	private:
		void Init();
		void OnChangeCamera(ChangeCameraRequestPtr message);
		osg::ref_ptr<osg::Camera> m_OSGCamera; 
		std::string m_Name;
		OSGRenderWindow* m_Window;
		CameraComponentWeakPtr m_Camera;
		osgViewer::View* m_OSGView;
	};
	typedef SPTR<OSGViewport> OSGViewportPtr;
}
