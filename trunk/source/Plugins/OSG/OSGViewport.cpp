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
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Plugins/Ogre/OgreViewport.h"
#include "Plugins/Ogre/OgreGraphicsSystem.h"
#include "Plugins/Ogre/OgrePostProcess.h"
#include "Core/Utils/GASSException.h"
#include <osgViewport>

namespace GASS
{
	OSGViewport::OSGViewport(const std::string &name,osg::Viewport* vp,OSGRenderWindow* window) : m_Name(name), 
		m_OSGViewport(vp), 
		m_Window(window)
	{
		
	}

	OSGViewport::~OSGViewport()
	{

	}

	void OSGViewport::Init()
	{
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(OgreViewport::OnChangeCamera,ChangeCameraRequest,0));
	}

	void OSGViewport::OnChangeCamera(ChangeCameraRequestPtr message)
	{
		const std::string vp_name = message->GetViewport();
		if(vp_name == m_Name || vp_name == "")
		{
			SetCamera(message->GetCamera());
		}
	}

	CameraComponentPtr OSGViewport::GetCamera() const
	{
		return CameraComponentPtr(m_Camera);
	}

	void OSGViewport::SetCamera(CameraComponentPtr camera)
	{
		m_Camera = camera;
		OSGCameraComponentPtr cam_comp = DYNAMIC_CAST<OSGCameraComponent>(camera);
		m_OSGViewport->setCamera(cam_comp->GetOgreCamera());
		ViewportPtr viewport = shared_from_this();
		SystemMessagePtr cam_message(new CameraChangedEvent(viewport));
		SimEngine::Get().GetSimSystemManager()->PostMessage(cam_message);
	}
}





