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

#include <boost/bind.hpp>
#include "Plugins/Base/Components/TopCamControlComponent.h"
#include "Plugins/Base/CoreMessages.h"
#include "Sim/Components/Graphics/ILocationComponent.h"
#include "Sim/Components/Graphics/ICameraComponent.h"

#include "Sim/SimEngine.h"
#include "Sim/Systems/Input/ControlSettingsManager.h"
#include "Sim/Systems/Input/ControlSetting.h"
#include "Sim/Common.h"
#include "Sim/Scenario/Scenario.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Scenario/Scene/Messages/GraphicsSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/GraphicsScenarioSceneMessages.h"

#include "Sim/SimEngine.h"
#include "Sim/Scheduling/IRuntimeController.h"


#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/Math/Quaternion.h"
#include "Core/Utils/GASSLogManager.h"

namespace GASS
{
	TopCamControlComponent::TopCamControlComponent() : m_ZoomSpeed(5),
		m_MaxWindowSize(520),
		m_MinWindowSize(10),
		m_ControlSetting (NULL),
		m_Pos(0,0,0),
		m_Rot(0,0,0),
		m_EnablePanInput(false),
		m_ScrollBoostInput(0),
		m_ZoomInput(0),
		m_ScrollUpInput(0),
		m_ScrollDownInput(0),
		m_Active(false),
		m_CurrentWindowSize(45)
	{

	}

	TopCamControlComponent::~TopCamControlComponent()
	{

	}

	void TopCamControlComponent::RegisterReflection()                         // static
	{
		ComponentFactory::GetPtr()->Register("TopCamControlComponent",new Creator<TopCamControlComponent, IComponent>);
		RegisterProperty<float>("MaxWindowSize", &GASS::TopCamControlComponent::GetMaxWindowSize, &GASS::TopCamControlComponent::SetMaxWindowSize);
		RegisterProperty<float>("MinWindowSize", &GASS::TopCamControlComponent::GetMinWindowSize, &GASS::TopCamControlComponent::SetMinWindowSize);
		RegisterProperty<float>("FixedHeight", &GASS::TopCamControlComponent::GetFixedHeight, &GASS::TopCamControlComponent::SetFixedHeight);
		RegisterProperty<float>("WindowSize", &GASS::TopCamControlComponent::GetWindowSize, &GASS::TopCamControlComponent::SetWindowSize);
	}

	void TopCamControlComponent::OnCreate()
	{
		SimEngine::GetPtr()->GetRuntimeController()->Register(this);

		GetSceneObject()->RegisterForMessage(REG_TMESS(TopCamControlComponent::PositionChange, PositionMessage ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(TopCamControlComponent::RotationChange,RotationMessage ,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(TopCamControlComponent::OnInit,LoadCoreComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(TopCamControlComponent::OnUnload,UnloadComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(TopCamControlComponent::OnCameraParameter,CameraParameterMessage,0));


		m_ControlSetting = SimEngine::Get().GetControlSettingsManager()->GetControlSetting("FreeCameraInputSettings");
		assert(m_ControlSetting);
		m_ControlSetting->GetMessageManager()->RegisterForMessage(typeid(ControllerMessage), MESSAGE_FUNC( TopCamControlComponent::OnInput));

		ScenarioPtr scenario = GetSceneObject()->GetSceneObjectManager()->GetScenario();
		scenario->RegisterForMessage(REG_TMESS( TopCamControlComponent::OnChangeCamera, ChangeCameraMessage, 0 ));
	}

	TaskGroup TopCamControlComponent::GetTaskGroup() const
	{
		return MAIN_TASK_GROUP;
	}

	void TopCamControlComponent::OnUnload(MessagePtr message)
	{
		SimEngine::GetPtr()->GetRuntimeController()->Unregister(this);
		m_ControlSetting->GetMessageManager()->UnregisterForMessage(typeid(ControllerMessage), MESSAGE_FUNC( TopCamControlComponent::OnInput));

		ScenarioPtr scenario = GetSceneObject()->GetSceneObjectManager()->GetScenario();
		scenario->UnregisterForMessage(UNREG_TMESS( TopCamControlComponent::OnChangeCamera, ChangeCameraMessage));
	}

	void TopCamControlComponent::OnChangeCamera(MessagePtr message)
	{
		ChangeCameraMessagePtr cc_mess = boost::shared_static_cast<ChangeCameraMessage>(message);
		SceneObjectPtr cam_obj = cc_mess->GetCamera();

		if(GetSceneObject() == cam_obj)
		{
			m_Active = true;
		}
		else
			m_Active = false;
	}

	void TopCamControlComponent::PositionChange(MessagePtr message)
	{
		if(message->GetSenderID() != (int) this)
		{
			PositionMessagePtr pos_mess = boost::shared_static_cast<PositionMessage>(message);
			m_Pos = pos_mess->GetPosition();
		}
	}

	void TopCamControlComponent::RotationChange(MessagePtr message)
	{
		if(message->GetSenderID() != (int) this)
		{
		
		}
	}

	void TopCamControlComponent::OnInput(MessagePtr message)
	{
		if(!m_Active) 
			return;
		ControllerMessagePtr control_mess = boost::shared_static_cast<ControllerMessage>(message);
		std::string name = control_mess->GetController();
		float value = control_mess->GetValue();

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

	void TopCamControlComponent::OnInit(MessagePtr message)
	{
	}

	void TopCamControlComponent::Update(double delta_time)
	{
		if(!m_ControlSetting) return;
		if(m_Active)
		{
			UpdateTopCam(delta_time);
		}
	}

	void TopCamControlComponent::UpdateTopCam(double delta)
	{
		ScenarioPtr scenario = GetSceneObject()->GetSceneObjectManager()->GetScenario();
		Vec3 up(0,1,0);
		Vec3 north (0,0,-1);
		Vec3 east (1,0,0);

		float speed_factor = 0;
		if(up.y > 0)
			m_Rot = Vec3(0,-MY_PI/2.f,0);
		else if(up.z > 0)
			m_Rot = Vec3(0,0,0);
		speed_factor = m_CurrentWindowSize;

		if(m_ScrollBoostInput)
			speed_factor *= 2;

		
		
		Vec3 filter;
		filter.x = abs(north.x) + abs(east.x);
		filter.y = abs(north.y) + abs(east.y);
		filter.z = abs(north.z) + abs(east.z);

		up = up*m_FixedHeight;
		m_Pos = (m_Pos*filter) + up;


		
		Vec3 move_north = north*delta*speed_factor * m_ScrollUpInput;
		Vec3 move_east = east*delta*speed_factor * m_ScrollDownInput;
		
		if(m_EnablePanInput) 
			m_Pos = ((m_Pos + move_north) + move_east);
		
		
		//
		//if(m_EnableZoomInput) 
		m_CurrentWindowSize += speed_factor*m_ZoomSpeed*m_ZoomInput*delta;
		//m_Fov -= time_step*m_FovChangeSpeed*m_ControlSetting->Get("TopCamControlComponentZoom");
		if(m_CurrentWindowSize < m_MinWindowSize) m_CurrentWindowSize = m_MinWindowSize;
		if(m_CurrentWindowSize > m_MaxWindowSize) m_CurrentWindowSize = m_MaxWindowSize;

		int from_id = (int)this;
		MessagePtr pos_msg(new PositionMessage(m_Pos,from_id));
		GetSceneObject()->PostMessage(pos_msg);

		MessagePtr rot_msg(new RotationMessage(m_Rot,from_id));
		GetSceneObject()->PostMessage(rot_msg);

		MessagePtr cam_msg(new CameraParameterMessage(CameraParameterMessage::CAMERA_ORTHO_WIN_SIZE,m_CurrentWindowSize,0,from_id));
		GetSceneObject()->PostMessage(cam_msg);
		m_ZoomInput *= 0.9; 
	}


	void TopCamControlComponent::OnCameraParameter(CameraParameterMessagePtr message)
	{
		CameraParameterMessage::CameraParameterType type = message->GetParameter();
		int this_id = (int)this;
		if(message->GetSenderID() != this_id)
		{
			switch(type)
			{
			case CameraParameterMessage::CAMERA_FOV:
				{
				}
				break;
			case CameraParameterMessage::CAMERA_ORTHO_WIN_SIZE:
				{
					m_CurrentWindowSize = message->GetValue1();

				}
				break;
			case CameraParameterMessage::CAMERA_CLIP_DISTANCE:
				{
				}
				break;
			}
		}
	}

}
