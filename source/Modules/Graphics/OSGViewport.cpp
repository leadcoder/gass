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

#include "Modules/Graphics/OSGViewport.h"
#include <osgViewer/Viewer>

#include "Core/Common.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"

#include "Modules/Graphics/OSGGraphicsSystem.h"
#include "Modules/Graphics/Components/OSGCameraComponent.h"
#include "Modules/Graphics/OSGGraphicsSceneManager.h"
#include "Modules/Graphics/Components/OSGCameraManipulatorComponent.h"
#include "Core/Utils/GASSException.h"

namespace GASS
{
	OSGViewport::OSGViewport(const std::string &name,osgViewer::View* view,OSGRenderWindow* window) : m_Name(name), 
		m_OSGView(view), 
		m_Window(window)
	{
		
	}

	OSGViewport::~OSGViewport()
	{

	}

	void OSGViewport::Init()
	{
	}

	CameraComponentPtr OSGViewport::GetCamera() const
	{
		return CameraComponentPtr(m_Camera);
	}

	void OSGViewport::SetCamera(CameraComponentPtr camera)
	{
		if(OSGCameraComponentPtr prev_cam_comp = GASS_DYNAMIC_PTR_CAST<OSGCameraComponent>(m_Camera.lock()))
			prev_cam_comp->SetOSGCamera(nullptr);
		
		m_Camera = camera;
		OSGCameraComponentPtr cam_comp = GASS_DYNAMIC_PTR_CAST<OSGCameraComponent>(camera);
		cam_comp->SetOSGCamera(m_OSGView->getCamera());

		
		//set scene data
		OSGGraphicsSceneManagerPtr sm = cam_comp->GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<OSGGraphicsSceneManager>();
		m_OSGView->setSceneData(sm->GetOSGRootNode());

		//view attached to scene node!
		ViewportPtr viewport = shared_from_this();
		SystemMessagePtr cam_message(new CameraChangedEvent(viewport));
		SimEngine::Get().GetSimSystemManager()->PostMessage(cam_message);

		auto man = cam_comp->GetSceneObject()->GetFirstComponentByClass<OSGCameraManipulatorComponent>();
		if(man)
		{
			m_OSGView->setCameraManipulator(man->GetManipulator(), false);
			// this should be moved to manipulator?
			//m_OSGView->getCamera()->setComputeNearFarMode(osgUtil::CullVisitor::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);
			 // configure the near/far so we don't clip things that are up close
			//m_OSGView->getCamera()->setNearFarRatio(0.000007);
			//m_OSGView->getCamera()->setNearFarRatio(0.0001);
			
		}
		else 
		{
			m_OSGView->setCameraManipulator(nullptr);
		}
	}
}