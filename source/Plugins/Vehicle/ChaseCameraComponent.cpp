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
		m_DampingConstant(8),
    	m_SpringConstant(16),
	    m_OffsetDistance(5.0),
		m_OffsetHeight(1.0),
		m_CameraLocation(NULL),
		m_ChaseObjectLocation(NULL)
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
		if(!m_InputHandlerObject.IsValid())
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"No InputHandlerObject found", " ChaseCameraComponent::OnInitialize");

		m_InputHandlerObject->RegisterForMessage(REG_TMESS(ChaseCameraComponent::OnEnter,EnterVehicleRequest,0));
		m_InputHandlerObject->RegisterForMessage(REG_TMESS(ChaseCameraComponent::OnExit,ExitVehicleRequest,0));

		//Assume target is parent
		m_ChaseObjectLocation = GetSceneObject()->GetParentSceneObject()->GetFirstComponentByClass<ILocationComponent>().get();
		GASSAssert(m_ChaseObjectLocation, "Failed to find ChaseObjectLocation in ChaseCameraComponent::OnInitialize");
		//Assume our scene object also hold the camera
		m_CameraLocation = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>().get();
		GASSAssert(m_CameraLocation, "Failed to find CameraLocation in ChaseCameraComponent::OnInitialize");

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
		const Vec3 world_up_vec(0, 1, 0);
		//Get parent location
		
		const Vec3 object_pos = m_ChaseObjectLocation->GetWorldPosition();
		const Quaternion object_rot = m_ChaseObjectLocation->GetWorldRotation();

		const Vec3 camera_pos = m_CameraLocation->GetWorldPosition();

		//Calculate projected object direction
		const Vec3 proj_object_z_axis = world_up_vec.Cross(object_rot.GetRotationMatrix().GetZAxis()).NormalizedCopy().Cross(world_up_vec);
		
		//Calculate the desired camera position
		const Vec3 desired_camera_pos = object_pos + proj_object_z_axis * m_OffsetDistance + world_up_vec * m_OffsetHeight;
		
		//Update the velocity vector using the spring parameters
		const Vec3 displacement = camera_pos - desired_camera_pos;
		const Vec3 spring_acceleration = (-m_SpringConstant * displacement) - (m_DampingConstant * m_Velocity);
		m_Velocity += spring_acceleration * delta_time;
		
		//Update camera position
		const Vec3 new_camera_pos = camera_pos + m_Velocity * delta_time;
		m_CameraLocation->SetWorldPosition(new_camera_pos);
		
		//Update camera orientation from new camera position
		const Vec3 camera_z_axis = (new_camera_pos - object_pos).NormalizedCopy();
		const Vec3 camera_x_axis = Vec3::Cross(world_up_vec, camera_z_axis).NormalizedCopy();
		const Vec3 camera_y_axis = Vec3::Cross(camera_z_axis, camera_x_axis).NormalizedCopy();
		m_CameraLocation->SetWorldRotation(Quaternion(camera_x_axis, camera_y_axis, camera_z_axis));
	}
}
