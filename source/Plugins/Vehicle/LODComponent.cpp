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

#include "LODComponent.h"

#include <memory>
#include "Sim/Messages/GASSPlatformMessages.h"
#include "Sim/Interface/GASSICameraComponent.h"
#include "Sim/Interface/GASSIViewport.h"
#include "Sim/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"


namespace GASS
{
	LODComponent::LODComponent() : 
		m_CameraPosition(0,0,0),
		m_ObjectPosition(0,0,0)
		
	{

	}

	LODComponent::~LODComponent()
	{

	}

	void LODComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register<LODComponent>();
		RegisterGetSet("MediumLODDistance", &LODComponent::GetMediumLODDistance, &LODComponent::SetMediumLODDistance);
		RegisterGetSet("LowLODDistance", &LODComponent::GetLowLODDistance, &LODComponent::SetLowLODDistance);
	}

	void LODComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(LODComponent::OnObjectMoved,TransformationChangedEvent,0));
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(LODComponent::OnCameraChanged,CameraChangedEvent,0));
		//get active camera
		
		SceneObjectPtr camera = GetSceneObject()->GetScene()->GetRootSceneObject()->GetFirstChildByName("FreeCamera",false);
		m_ActiveCameraObject = camera;
		if(!camera)
		{
			GASS_LOG(LWARNING) << "Failed to find free camera for LODComponent";
		}
		
		if(camera)
		{
			camera->RegisterForMessage(REG_TMESS( LODComponent::OnCameraMoved,TransformationChangedEvent,0));
		}
	}

	void LODComponent::OnDelete()
	{
		SimEngine::Get().GetSimSystemManager()->UnregisterForMessage(UNREG_TMESS(LODComponent::OnCameraChanged,CameraChangedEvent));

		SceneObjectPtr cam = m_ActiveCameraObject.lock();
		if(cam)
		{
			cam->UnregisterForMessage(UNREG_TMESS( LODComponent::OnCameraMoved,TransformationChangedEvent));
		}
	}
	
	void LODComponent::OnCameraChanged(CameraChangedEventPtr message)
	{
		CameraComponentPtr camera = message->GetViewport()->GetCamera();
		SceneObjectPtr cam_obj = GASS_DYNAMIC_PTR_CAST<BaseSceneComponent>(camera)->GetSceneObject();
		
		SceneObjectPtr prev_cam = m_ActiveCameraObject.lock();
		if(prev_cam)
		{
			prev_cam->UnregisterForMessage(UNREG_TMESS( LODComponent::OnCameraMoved,TransformationChangedEvent));
		}
		cam_obj->RegisterForMessage(REG_TMESS( LODComponent::OnCameraMoved,TransformationChangedEvent,0));
		m_ActiveCameraObject = cam_obj;
	}

	void LODComponent::OnCameraMoved(TransformationChangedEventPtr message)
	{
		m_CameraPosition = message->GetPosition();
		UpdateLOD();
	}


	void LODComponent::OnObjectMoved(TransformationChangedEventPtr message)
	{
		m_ObjectPosition = message->GetPosition();
		UpdateLOD();
	}

	void LODComponent::UpdateLOD()
	{
		GASS::Float distance = (m_CameraPosition-m_ObjectPosition).Length();

		LODMessage::LODLevel level = LODMessage::LOD_HIGH;
		if(distance > m_MediumLODDistance)
		{
			level = LODMessage::LOD_MEDIUM;
			if(distance > m_LowLODDistance)
			{
				level = LODMessage::LOD_LOW;
			}
		}
		if(m_CurrentLevel != level)
		{
			GetSceneObject()->PostEvent(std::make_shared<LODMessage>(level));
			m_CurrentLevel = level;
		}
	}
}
