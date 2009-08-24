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

#include "TrackComponent.h"
#include "Core/Math/Quaternion.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/Message.h"
#include "Core/Utils/Log.h"
#include "Sim/Scenario/Scene/ScenarioScene.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Systems/Resource/IResourceSystem.h"
#include "Sim/SimEngine.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Sim/Scheduling/IRuntimeController.h"
#include "Sim/Systems/Input/ControlSettingsManager.h"
#include "Sim/Systems/Input/ControlSetting.h"


namespace GASS
{
	TrackComponent::TrackComponent() : m_Initialized(false), m_AnimationValue(0), m_AnimationSpeedFactor(1)
	{
	}

	TrackComponent::~TrackComponent()
	{

	}

	void TrackComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("TrackComponent",new Creator<TrackComponent, IComponent>);
		RegisterProperty<std::string>("DriveWheel", &TrackComponent::GetDriveWheel, &TrackComponent::SetDriveWheel);
		RegisterProperty<float>("AnimationSpeedFactor", &TrackComponent::GetAnimationSpeedFactor, &TrackComponent::SetAnimationSpeedFactor);
	}

	void TrackComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(SceneObject::OBJECT_RM_LOAD_SIM_COMPONENTS, MESSAGE_FUNC(TrackComponent::OnLoad));
		//register for physics messages on engine?
	}

	void TrackComponent::OnLoad(MessagePtr message)
	{
		m_Initialized = true;
		SetDriveWheel(m_DriveWheelName);
	}

	std::string TrackComponent::GetDriveWheel() const
	{
		return m_DriveWheelName;
	}

	void TrackComponent::SetDriveWheel(const std::string &wheel)
	{
		m_DriveWheelName = wheel;
		if(m_Initialized)
		{
			SceneObjectVector objects;
			GetSceneObject()->GetObjectUnderRoot()->GetObjectsByName(objects,m_DriveWheelName,false);
			if(objects.size() > 0)
			{
				m_DriveWheel = (objects.front());
				objects.front()->RegisterForMessage(SceneObject::OBJECT_NM_PHYSICS_VELOCITY, MESSAGE_FUNC(TrackComponent::OnDriveWheelPhysicsMessage));
			}
		}
	}

	void TrackComponent::OnDriveWheelPhysicsMessage(MessagePtr message)
	{
		Vec3 ang_vel  = boost::any_cast<Vec3>(message->GetData("AngularVelocity"));
		//Vec2 speed(ang_vel.x,0);
		m_AnimationValue += (ang_vel.x*m_AnimationSpeedFactor);
		Vec2 speed(m_AnimationValue,0);

		MessagePtr mesh_msg(new Message(SceneObject::OBJECT_RM_MESH_PARAMETER));
		mesh_msg->SetData("Parameter",SceneObject::ANIMATE_TEX_COORD);
		mesh_msg->SetData("Speed",speed);
		GetSceneObject()->PostMessage(mesh_msg);


		float emission = fabs(ang_vel.x)*2;

		if(fabs(ang_vel.x) < 10)
			emission  = 0;
		if(emission >222)
			emission =222;
		MessagePtr particle_msg(new Message(SceneObject::OBJECT_RM_PARTICLE_SYSTEM_PARAMETER));
		particle_msg->SetData("Parameter",SceneObject::EMISSION_RATE);
		particle_msg->SetData("Emitter",int(0));
		particle_msg->SetData("Rate",emission);
		GetSceneObject()->PostMessage(particle_msg);


		MessagePtr particle_duration_msg(new Message(SceneObject::OBJECT_RM_PARTICLE_SYSTEM_PARAMETER));
		
		/*float duration = fabs(ang_vel.x)*0.05;

		if(duration > 1.6)  
			duration = 1.6;
		particle_duration_msg->SetData("Parameter",SceneObject::PARTICLE_LIFE_TIME);
		particle_duration_msg->SetData("Emitter",int(0));
		particle_duration_msg->SetData("TimeToLive",duration);*/
		
		//GetSceneObject()->PostMessage(particle_duration_msg);
		
		//std::cout << "speed:" << speed.x << std::endl;

	}
}
