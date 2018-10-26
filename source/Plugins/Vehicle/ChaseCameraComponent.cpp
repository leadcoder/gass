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

#include "ChaseCameraComponent.h"
#include "Core/Math/GASSMath.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/Interface/GASSICameraComponent.h"
#include "Sim/Interface/GASSIMissionSceneManager.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"
#include "Sim/Messages/GASSPlatformMessages.h"

namespace GASS
{
	ChaseCameraComponent::ChaseCameraComponent() : m_PreferredViewport(""),
		m_Velocity(0,0,0),
		m_DampingConstant(8.0),
    	m_SpringConstant(16.0),
	    m_OffsetDistance(5.0),
		m_OffsetHeight(1.0)
	{

	}

	ChaseCameraComponent::~ChaseCameraComponent()
	{

	}

	void ChaseCameraComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("ChaseCameraComponent",new Creator<ChaseCameraComponent, Component>);
		RegisterProperty<std::string>("PreferredViewport", &ChaseCameraComponent::GetPreferredViewport, &ChaseCameraComponent::SetPreferredViewport);
		RegisterProperty<SceneObjectRef>("InputHandlerObject", &ChaseCameraComponent::GetInputHandlerObject, &ChaseCameraComponent::SetInputHandlerObject);
		RegisterProperty<Float>("DampingConstant", &ChaseCameraComponent::GetDampingConstant, &ChaseCameraComponent::SetDampingConstant);
		RegisterProperty<Float>("SpringConstant", &ChaseCameraComponent::GetSpringConstant, &ChaseCameraComponent::SetSpringConstant);
		RegisterProperty<Float>("DampingConstant", &ChaseCameraComponent::GetDampingConstant, &ChaseCameraComponent::SetDampingConstant);
		RegisterProperty<Float>("OffsetDistance", &ChaseCameraComponent::GetOffsetDistance, &ChaseCameraComponent::SetOffsetDistance);
		RegisterProperty<Float>("OffsetHeight", &ChaseCameraComponent::GetOffsetHeight, &ChaseCameraComponent::SetOffsetHeight);
	}

	void ChaseCameraComponent::OnInitialize()
	{
		BaseSceneComponent::InitializeSceneObjectRef();
		BaseSceneComponent::OnInitialize();
		
		if(!m_InputHandlerObject.IsValid())
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"No InputHandlerObject found", " ChaseCameraComponent::OnInitialize");

		m_InputHandlerObject->RegisterForMessage(REG_TMESS(ChaseCameraComponent::OnEnter,EnterVehicleRequest,0));
		m_InputHandlerObject->RegisterForMessage(REG_TMESS(ChaseCameraComponent::OnExit,ExitVehicleRequest,0));
		RegisterForPostUpdate<IMissionSceneManager>();
	}

	void ChaseCameraComponent::OnDelete()
	{
		
	}

	void ChaseCameraComponent::SetPreferredViewport(const std::string &viewport)
	{
		m_PreferredViewport = viewport;
	}

	std::string ChaseCameraComponent::GetPreferredViewport() const
	{
		return m_PreferredViewport;
	}

	void ChaseCameraComponent::OnEnter(EnterVehicleRequestPtr message)
	{
		CameraComponentPtr camera = GetSceneObject()->GetFirstComponentByClass<ICameraComponent>();
		camera->ShowInViewport(m_PreferredViewport);
	}

	void ChaseCameraComponent::OnExit(ExitVehicleRequestPtr message)
	{

	}

	void ChaseCameraComponent::SceneManagerTick(double delta_time)
	{
		//Get parent location
		LocationComponentPtr target_location  =  GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		Vec3 target_pos = target_location->GetWorldPosition();
		Quaternion target_rot = target_location->GetWorldRotation();

		LocationComponentPtr camera_location  =  GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		Vec3 eye_pos = camera_location->GetWorldPosition();

		Vec3 dir = target_rot.GetRotationMatrix().GetZAxis();
		dir.y = 0;
		dir.Normalize();
		Vec3 ideal_position = target_pos + dir * m_OffsetDistance;
		ideal_position.y += m_OffsetHeight;
		Vec3 displacement = eye_pos - ideal_position;
		Vec3 spring_acceleration = (-m_SpringConstant * displacement) - 
			(m_DampingConstant * m_Velocity);

		m_Velocity += spring_acceleration * delta_time;
		eye_pos += m_Velocity * delta_time;
		
		camera_location->SetWorldPosition(eye_pos);
		
		Vec3 camera_dir = eye_pos - target_pos;
		
		camera_dir.Normalize();
		Vec3 left = Vec3::Cross(Vec3(0,1,0),camera_dir);
		left.Normalize();
		Vec3 up = Vec3::Cross(camera_dir,left);
		up.Normalize();
		Mat4 rot_mat;
		rot_mat.SetRotationByAxis(left, up, camera_dir);
		Quaternion new_rot;
		new_rot.FromRotationMatrix(rot_mat);
		GetSceneObject()->PostRequest(WorldRotationRequestPtr(new WorldRotationRequest(new_rot)));
	}
}
