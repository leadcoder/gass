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


#include "Plugins/Base/Components/TopCamControlComponent.h"
#include "Plugins/Base/GASSCoreSceneManager.h"
#include "Core/Math/GASSMath.h"
#include "Sim/Interface/GASSIViewport.h"
#include "Sim/Interface/GASSICameraComponent.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/GASSSimSystemManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"

namespace GASS
{
	TopCamControlComponent::TopCamControlComponent() : m_ZoomSpeed(5),
		m_MaxWindowSize(520),
		m_MinWindowSize(10),
		m_Pos(0,0,0),
		m_Rot(0,0,0),
		m_EnablePanInput(false),
		m_ScrollBoostInput(0),
		m_ZoomInput(0),
		m_ScrollUpInput(0),
		m_ScrollDownInput(0),
		m_Active(false),
		m_CurrentWindowSize(45),
		m_ControlSettingName("FreeCameraInputSettings"),
		m_FixedHeight(0)
	{

	}

	TopCamControlComponent::~TopCamControlComponent()
	{

	}

	void TopCamControlComponent::RegisterReflection()                         // static
	{
		ComponentFactory::Get().Register<TopCamControlComponent>();
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("TopCamControlComponent", OF_VISIBLE)));
		RegisterProperty<float>("MaxWindowSize", &GASS::TopCamControlComponent::GetMaxWindowSize, &GASS::TopCamControlComponent::SetMaxWindowSize);
		RegisterProperty<float>("MinWindowSize", &GASS::TopCamControlComponent::GetMinWindowSize, &GASS::TopCamControlComponent::SetMinWindowSize);
		RegisterProperty<float>("FixedHeight", &GASS::TopCamControlComponent::GetFixedHeight, &GASS::TopCamControlComponent::SetFixedHeight);
		RegisterProperty<float>("WindowSize", &GASS::TopCamControlComponent::GetWindowSize, &GASS::TopCamControlComponent::SetWindowSize);
	}

	void TopCamControlComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(TopCamControlComponent::PositionChange, PositionRequest ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(TopCamControlComponent::RotationChange,RotationRequest ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(TopCamControlComponent::OnCameraParameter,CameraParameterRequest,0));
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(TopCamControlComponent::OnInput,ControllSettingsMessage,0));
		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS( TopCamControlComponent::OnCameraChanged, CameraChangedEvent, 0 ));

		//register for updates
		RegisterForPostUpdate<CoreSceneManager>();
	}

	void TopCamControlComponent::OnDelete()
	{
		SimEngine::Get().GetSimSystemManager()->UnregisterForMessage(UNREG_TMESS( TopCamControlComponent::OnCameraChanged, CameraChangedEvent));
	}

	void TopCamControlComponent::OnCameraChanged(CameraChangedEventPtr message)
	{
		SceneObjectPtr cam_obj = GASS_DYNAMIC_PTR_CAST<BaseSceneComponent>(message->GetViewport()->GetCamera())->GetSceneObject();
		if(GetSceneObject() == cam_obj)
		{
			m_Active = true;
		}
		else
			m_Active = false;
	}

	void TopCamControlComponent::PositionChange(MessagePtr message)
	{
		if(message->GetSenderID() != GASS_PTR_TO_INT(this))
		{
			PositionRequestPtr pos_mess = GASS_STATIC_PTR_CAST<PositionRequest>(message);
			m_Pos = pos_mess->GetPosition();
		}
	}

	void TopCamControlComponent::RotationChange(MessagePtr message)
	{
		if(message->GetSenderID() != GASS_PTR_TO_INT(this))
		{

		}
	}

	void TopCamControlComponent::OnInput(ControllSettingsMessagePtr message)
	{
		if(!m_Active)
			return;
		if(message->GetSettings() != m_ControlSettingName) // only hog our settings
			return;

		std::string name = message->GetController();
		float value = message->GetValue();

		if(name == "FreeCameraSpeedBoost")
		{
			if(value > 0)
				m_ScrollBoostInput = true;
			else
				m_ScrollBoostInput = false;
		}
		/*else if(name == "FreeCameraEnableRot")
		{
			if(value > 0)
				m_EnableZoomInput = true;
			else
				m_EnableZoomInput = false;
		}*/
		else if(name == "2dCameraEnablePan")
		{
			if(value > 0)
				m_EnablePanInput = true;
			else
				m_EnablePanInput = false;
		}
		else if(name == "2dCameraPanV")
		{
			m_ScrollUpInput = value;
			//std::cout << "Input: " << name << "Value:" << value << std::endl;
		}
		else if(name == "2dCameraPanH")
		{
			m_ScrollDownInput = value;
		}
		else if(name == "2dCameraZoom")
		{
			m_ZoomInput = value;

		}
		else if(name == "FreeCameraZoom")
		{
			//m_ZoomInput = value;

		}
	}


	void TopCamControlComponent::SceneManagerTick(double delta_time)
	{
		if(m_Active)
		{
			UpdateTopCam(delta_time);
		}
	}

	void TopCamControlComponent::UpdateTopCam(double delta)
	{
		ScenePtr scene = GetSceneObject()->GetScene();
		Vec3 up(0,1,0);
		Vec3 north (0,0,-1);
		Vec3 east (1,0,0);

		float speed_factor = 0;
		if(up.y > 0)
			m_Rot = Vec3(-GASS_PI / 2.f, 0, 0);
		else if(up.z > 0)
			m_Rot = Vec3(0,0,0);
		speed_factor = m_CurrentWindowSize;

		if(m_ScrollBoostInput)
			speed_factor *= 2;



		Vec3 filter;
		filter.x = fabs(north.x) + fabs(east.x);
		filter.y = fabs(north.y) + fabs(east.y);
		filter.z = fabs(north.z) + fabs(east.z);

		up = up*m_FixedHeight;
		m_Pos = (m_Pos*filter) + up;



		Vec3 move_north = north*delta*speed_factor * m_ScrollUpInput;
		Vec3 move_east = east*delta*speed_factor * m_ScrollDownInput;

		if(m_EnablePanInput)
			m_Pos = ((m_Pos + move_north) + move_east);


		//
		//if(m_EnableZoomInput)
		m_CurrentWindowSize += speed_factor * m_ZoomSpeed * m_ZoomInput * static_cast<float>(delta);
		//m_Fov -= time_step*m_FovChangeSpeed*m_ControlSetting->Get("TopCamControlComponentZoom");
		if(m_CurrentWindowSize < m_MinWindowSize) m_CurrentWindowSize = m_MinWindowSize;
		if(m_CurrentWindowSize > m_MaxWindowSize) m_CurrentWindowSize = m_MaxWindowSize;

		int from_id = GASS_PTR_TO_INT(this);
		GetSceneObject()->PostRequest(PositionRequestPtr(new PositionRequest(m_Pos,from_id)));

		GetSceneObject()->PostRequest(RotationRequestPtr(new RotationRequest(Quaternion::CreateFromEulerYXZ(m_Rot),from_id)));

		CameraParameterRequestPtr cam_msg(new CameraParameterRequest(CameraParameterRequest::CAMERA_ORTHO_WIN_SIZE,m_CurrentWindowSize,0,from_id));
		GetSceneObject()->PostRequest(cam_msg);
		m_ZoomInput *= 0.9f;
	}


	void TopCamControlComponent::OnCameraParameter(CameraParameterRequestPtr message)
	{
		CameraParameterRequest::CameraParameterType type = message->GetParameter();
		int this_id = GASS_PTR_TO_INT(this);
		if(message->GetSenderID() != this_id)
		{
			switch(type)
			{
			case CameraParameterRequest::CAMERA_FOV:
				{
				}
				break;
			case CameraParameterRequest::CAMERA_ORTHO_WIN_SIZE:
				{
					m_CurrentWindowSize = message->GetValue1();

				}
				break;
			case CameraParameterRequest::CAMERA_CLIP_DISTANCE:
				{
				}
				break;
			}
		}
	}
}
