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
#include "Plugins/Ogre/GASSOgreCommon.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Plugins/Ogre/GASSOgreViewport.h"
#include "Plugins/Ogre/GASSOgreGraphicsSystem.h"
#include "Plugins/Ogre/GASSOgrePostProcess.h"
#include "Plugins/Ogre/Components/GASSOgreCameraComponent.h"

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
		//add post process to all windows, change this to camera effect instead?
		if(m_PostProcess)
		{
			m_PostProcess.reset();
		}
		m_PostProcess = OgrePostProcesGASS_SHARED_PTR(new OgrePostProcess(m_OgreViewport));

	}

	CameraComponentPtr OgreViewport::GetCamera() const
	{
		return CameraComponentPtr(m_Camera);
	}

	void OgreViewport::SetCamera(CameraComponentPtr camera)
	{
		m_Camera = camera;
		OgreCameraComponentPtr cam_comp = GASS_DYNAMIC_PTR_CAST<OgreCameraComponent>(camera);
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