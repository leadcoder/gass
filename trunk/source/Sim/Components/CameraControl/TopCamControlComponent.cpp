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
#include "Sim/Components/CameraControl/TopCamControlComponent.h"
#include "Sim/Components/Graphics/ILocationComponent.h"
#include "Sim/Components/Graphics/ICameraComponent.h"

#include "Sim/SimEngine.h"
#include "Sim/Systems/Input/ControlSettingsManager.h"
#include "Sim/Systems/Input/ControlSetting.h"
#include "Sim/Common.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"

#include "Sim/SimEngine.h"
#include "Sim/Scheduling/IRuntimeController.h"


#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/Message.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/Math/Quaternion.h"

namespace GASS
{
	TopCamControlComponent::TopCamControlComponent() :
		m_ZoomSpeed(5),
		m_MaxWindowSize(520),
		m_MinWindowSize(10),
		m_ControlSetting (NULL),
		m_Pos(0,0,0),
		m_Rot(0,0,0),
		m_EnableZoomInput(false),
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
	}

	void TopCamControlComponent::OnCreate()
	{
		SimEngine::GetPtr()->GetRuntimeController()->Register(this);

		GetSceneObject()->RegisterForMessage(SceneObject::OBJECT_RM_POSITION, MESSAGE_FUNC(TopCamControlComponent::PositionChange));
		GetSceneObject()->RegisterForMessage(SceneObject::OBJECT_RM_ROTATION, MESSAGE_FUNC(TopCamControlComponent::RotationChange));
		GetSceneObject()->RegisterForMessage(SceneObject::OBJECT_RM_LOAD_SIM_COMPONENTS, MESSAGE_FUNC(TopCamControlComponent::OnInit));
		GetSceneObject()->RegisterForMessage(SceneObject::OBJECT_RM_UNLOAD_COMPONENTS, MESSAGE_FUNC(TopCamControlComponent::OnUnload));

		m_ControlSetting = SimEngine::Get().GetControlSettingsManager()->GetControlSetting("FreeCameraInputSettings");

		m_ControlSetting->GetMessageManager()->RegisterForMessage(ControlSetting::CONTROLLER_MESSAGE_NEW_INPUT, MESSAGE_FUNC( TopCamControlComponent::OnInput));
		
		m_Scene = GetSceneObject()->GetSceneObjectManager()->GetScenarioScene();
		m_Scene->RegisterForMessage(ScenarioScene::SCENARIO_RM_CHANGE_CAMERA, MESSAGE_FUNC( TopCamControlComponent::OnChangeCamera));
	}

	TaskGroup TopCamControlComponent::GetTaskGroup() const
	{
		return MAIN_TASK_GROUP;
	}

	void TopCamControlComponent::OnUnload(MessagePtr message)
	{
		SimEngine::GetPtr()->GetRuntimeController()->Unregister(this);
	}

	void TopCamControlComponent::OnChangeCamera(MessagePtr message)
	{
		//if(message->m_FromID == (int) this)
		//	return;
		SceneObjectPtr cam_obj = boost::any_cast<SceneObjectPtr>(message->GetData("CameraObject"));

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
		   m_Pos = boost::any_cast<Vec3>(message->GetData("Position"));
	}

	void TopCamControlComponent::RotationChange(MessagePtr message)
	{
		
	}

	void TopCamControlComponent::OnInput(MessagePtr message)
	{
		if(!m_Active) 
			return;
		std::string name = boost::any_cast<std::string>(message->GetData("Controller"));
		float value = boost::any_cast<float>(message->GetData("Value"));

		if(name == "FreeCameraSpeedBoost")
		{
			if(value > 0)
				m_ScrollBoostInput = true;
			else 
				m_ScrollBoostInput = false;
		}
		else if(name == "FreeCameraEnableRot")
		{
			if(value > 0)
				m_EnableZoomInput = true;
			else 
				m_EnableZoomInput = false;
		}
		else if(name == "FreeCameraThrottle")
		{
			m_ScrollUpInput = value;
			//std::cout << "Input: " << name << "Value:" << value << std::endl;
		}
		else if(name == "FreeCameraStrafe")
		{
			m_ScrollDownInput = value;
		}
		else if(name == "FreeCameraHeading")
		{
			m_ZoomInput = value;
			
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
		float speed_factor = 0;
		m_Rot = Vec3(0,-MY_PI/2.f,0);
		speed_factor = m_CurrentWindowSize;
		m_Pos.z -= delta*speed_factor * m_ScrollUpInput;
		m_Pos.x += delta*speed_factor * m_ScrollDownInput;
		m_Pos.y = m_FixedHeight;
		//
		if(m_EnableZoomInput) 
			m_CurrentWindowSize += speed_factor*m_ZoomSpeed*m_ZoomInput*delta;
		//m_Fov -= time_step*m_FovChangeSpeed*m_ControlSetting->Get("TopCamControlComponentZoom");
		if(m_CurrentWindowSize < m_MinWindowSize) m_CurrentWindowSize = m_MinWindowSize;
		if(m_CurrentWindowSize > m_MaxWindowSize) m_CurrentWindowSize = m_MaxWindowSize;

		int from_id = (int)this;
		MessagePtr pos_msg(new Message(SceneObject::OBJECT_RM_POSITION,from_id));
		pos_msg->SetData("Position",m_Pos);
		GetSceneObject()->PostMessage(pos_msg);

		MessagePtr rot_msg(new Message(SceneObject::OBJECT_RM_ROTATION,from_id));
		
		rot_msg->SetData("Rotation",Quaternion(m_Rot));
		GetSceneObject()->PostMessage(rot_msg);

		MessagePtr cam_msg(new Message(SceneObject::OBJECT_RM_CAMERA_PARAMETER,from_id));
		
		cam_msg->SetData("Parameter",SceneObject::CAMERA_ORTHO_WIN_SIZE);
		cam_msg->SetData("Size",m_CurrentWindowSize);
		GetSceneObject()->PostMessage(cam_msg);

		//m_ScrollUpInput = 0;
		//m_ScrollDownInput = 0;
	}

	
}
