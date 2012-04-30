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

#include "LODComponent.h"
#include "GameMessages.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/Scene/GASSScene.h"
#include "Sim/Scene/GASSSceneObject.h"

#include "Sim/Systems/Resource/GASSIResourceSystem.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "Sim/Scheduling/GASSIRuntimeController.h"
#include "Sim/Systems/Input/GASSControlSettingsManager.h"
#include "Sim/Systems/Input/GASSControlSetting.h"


namespace GASS
{
	LODComponent::LODComponent() : m_LowLODDistance(20) , 
		m_MediumLODDistance(10),
		m_CameraPosition(0,0,0),
		m_ObjectPosition(0,0,0)
	{

	}

	LODComponent::~LODComponent()
	{

	}

	void LODComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("LODComponent",new Creator<LODComponent, IComponent>);
		RegisterProperty<float>("MediumLODDistance", &LODComponent::GetMediumLODDistance, &LODComponent::SetMediumLODDistance);
		RegisterProperty<float>("LowLODDistance", &LODComponent::GetLowLODDistance, &LODComponent::SetLowLODDistance);
	}

	void LODComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(LODComponent::OnLoad,LoadGameComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(LODComponent::OnUnload,UnloadComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(LODComponent::OnObjectMoved,TransformationNotifyMessage,0));
	}

	void LODComponent::OnLoad(LoadGameComponentsMessagePtr message)
	{
		GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS( LODComponent::OnChangeCamera,CameraChangedNotifyMessage,0));
		//get active camera
		
		SceneObjectPtr camera = GetSceneObject()->GetScene()->GetRootSceneObject()->GetFirstChildByName("FreeCamera",false);
		m_ActiveCameraObject = camera;
		if(!camera)
		{
			LogManager::getSingleton().stream() << "WARNING:Failed to find free camera for LODComponent";
		}
		
		if(camera)
		{
			camera->RegisterForMessage(REG_TMESS( LODComponent::OnCameraMoved,TransformationNotifyMessage,0));
		}
	}

	void LODComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
		GetSceneObject()->GetScene()->UnregisterForMessage(UNREG_TMESS( LODComponent::OnChangeCamera,CameraChangedNotifyMessage));

		SceneObjectPtr cam(m_ActiveCameraObject,boost::detail::sp_nothrow_tag());
		if(cam)
		{
			cam->UnregisterForMessage(UNREG_TMESS( LODComponent::OnCameraMoved,TransformationNotifyMessage));
		}
	}
	
	
	void LODComponent::OnChangeCamera(CameraChangedNotifyMessagePtr message)
	{
		SceneObjectPtr cam_obj = message->GetCamera();
		
		SceneObjectPtr prev_cam(m_ActiveCameraObject,boost::detail::sp_nothrow_tag());
		if(prev_cam)
		{
			prev_cam->UnregisterForMessage(UNREG_TMESS( LODComponent::OnCameraMoved,TransformationNotifyMessage));
		}
		cam_obj->RegisterForMessage(REG_TMESS( LODComponent::OnCameraMoved,TransformationNotifyMessage,0));
		m_ActiveCameraObject = cam_obj;
	}

	void LODComponent::OnCameraMoved(TransformationNotifyMessagePtr message)
	{
		m_CameraPosition = message->GetPosition();
		UpdateLOD();
	}


	void LODComponent::OnObjectMoved(TransformationNotifyMessagePtr message)
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
			GetSceneObject()->PostMessage(MessagePtr(new LODMessage(level)));
			m_CurrentLevel = level;
		}
	}

}
