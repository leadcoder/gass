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

#include "Core/Common.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Plugins/Ogre/OgreViewport.h"
#include "Plugins/Ogre/OgreGraphicsSystem.h"
#include "Plugins/Ogre/OgrePostProcess.h"
#include "Core/Utils/GASSException.h"
#include <OgreViewport.h>
#include <OgreSceneManager.h>

namespace GASS
{
	OgreViewport::OgreViewport(const std::string &name,Ogre::Viewport* vp,OgreRenderWindow* window) : m_Name(name), 
		m_OgreViewport(vp), 
		m_Window(window)
	{
	}

	OgreViewport::~OgreViewport()
	{

	}

	void OgreViewport::Init()
	{
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(OgreViewport::OnChangeCamera,ChangeCameraRequest,0));

		//add post process to all windows, change this to camera effect instead?
		if(m_PostProcess)
		{
			m_PostProcess.reset();
		}
		m_PostProcess = OgrePostProcessPtr(new OgrePostProcess(m_OgreViewport));

	}

	void OgreViewport::OnChangeCamera(ChangeCameraRequestPtr message)
	{
		const std::string vp_name = message->GetViewport();
		if(vp_name == m_Name || vp_name == "")
		{
			SetCamera(message->GetCamera());
		}
	}

	CameraComponentPtr OgreViewport::GetCamera() const
	{
		return CameraComponentPtr(m_Camera);
	}

	void OgreViewport::SetCamera(CameraComponentPtr camera)
	{
		m_Camera = camera;
		OgreCameraComponentPtr cam_comp = DYNAMIC_PTR_CAST<OgreCameraComponent>(camera);
		m_OgreViewport->setCamera(cam_comp->GetOgreCamera());

		if(cam_comp->GetMaterialScheme() != "")
		{
			m_OgreViewport->setMaterialScheme(cam_comp->GetMaterialScheme());
		}
		m_PostProcess->SetActiveCompositors(cam_comp->GetPostFilters());
		m_PostProcess->Update(cam_comp);

		ViewportPtr viewport = shared_from_this();
		SystemMessagePtr cam_message(new CameraChangedEvent(viewport));
		SimEngine::Get().GetSimSystemManager()->PostMessage(cam_message);
	}
}