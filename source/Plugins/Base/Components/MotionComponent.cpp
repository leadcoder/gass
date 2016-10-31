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


#include "Plugins/Base/Components/MotionComponent.h"
#include "Core/Math/GASSMath.h"
#include "Plugins/Base/GASSCoreSceneManager.h"
#include "Sim/Interface/GASSIViewport.h"
#include "Sim/Interface/GASSICameraComponent.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSGraphicsSystemMessages.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/Math/GASSQuaternion.h"
#include "Sim/Interface/GASSIMissionSceneManager.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Utils/GASSCollisionHelper.h"

//move
#include "Sim/Messages/GASSGraphicsSceneMessages.h"
#include "Core/Utils/GASSColorRGB.h"

namespace GASS
{
	MotionComponent::MotionComponent() : m_Acceleration(10),
		m_MaxSpeed(20),
		m_MaxTurnSpeed(1),
		m_ThrottleInput(0),
		m_SteerInput(0),
		m_CurrentSpeed(0),
		m_Debug(false),
		m_BreakInput(0),
		m_GroundClamp(true)
	{

	}

	MotionComponent::~MotionComponent()
	{

	}

	void MotionComponent::RegisterReflection()                         // static
	{
		ComponentFactory::GetPtr()->Register("MotionComponent",new Creator<MotionComponent, Component>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("MotionComponent", OF_VISIBLE)));
		RegisterProperty<Float>("Acceleration", &GASS::MotionComponent::GetAcceleration, &GASS::MotionComponent::SetAcceleration,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Acceleration [m/s2]",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<Float>("MaxSpeed", &GASS::MotionComponent::GetMaxSpeed, &GASS::MotionComponent::SetMaxSpeed,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Max Speed [m/s]",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<Float>("MaxTurnSpeed", &GASS::MotionComponent::GetMaxTurnSpeed, &GASS::MotionComponent::SetMaxTurnSpeed,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Angular Max Speed [deg/s]",PF_VISIBLE | PF_EDITABLE)));
		//RegisterProperty<MotionModeBinder>("Mode", &GASS::MotionComponent::GetMode, &GASS::MotionComponent::SetMode,
		//	EnumerationProxyPropertyMetaDataPtr(new EnumerationProxyPropertyMetaData("Motion Mode",PF_VISIBLE,&MotionModeBinder::GetStringEnumeration)));

		RegisterProperty<bool>("GroundClamp", &GASS::MotionComponent::GetGroundClamp, &GASS::MotionComponent::SetGroundClamp,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("GroundClamp", PF_VISIBLE | PF_EDITABLE)));

		RegisterProperty<bool>("Debug", &GASS::MotionComponent::GetDebug, &GASS::MotionComponent::SetDebug,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Debug", PF_VISIBLE | PF_EDITABLE)));
	}

	void MotionComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(MotionComponent::OnInput, InputRelayEvent, 0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(MotionComponent::OnTransMessage, TransformationChangedEvent, 0));
		//register for updates
		SceneManagerListenerPtr listener = shared_from_this();
		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<IMissionSceneManager>()->Register(listener);

		//try to get height_map
		m_Heightmap = GetSceneObject()->GetScene()->GetRootSceneObject()->GetFirstComponentByClass<IHeightmapTerrainComponent>(true);
	}

	void MotionComponent::OnTransMessage(TransformationChangedEventPtr message)
	{
		m_CurrentPos = message->GetPosition();
		m_CurrentRot = message->GetRotation();

		
		//ground clamp?
		if (m_GroundClamp)
		{

			//get vehicle corner pos
			Float vehicle_with = 2.0;
			Float vehicle_length = 4.0;
			Float vehicle_step_height = 1.0;

			Vec3 forward = -m_CurrentRot.GetZAxis();
			Vec3 side = m_CurrentRot.GetXAxis();

			Vec3 p1 = m_CurrentPos + forward * vehicle_length*0.5;
			Vec3 p2 = m_CurrentPos - forward * vehicle_length*0.5 + side*vehicle_with*0.5;
			Vec3 p3 = m_CurrentPos - forward * vehicle_length*0.5 - side*vehicle_with*0.5;
			Vec3 normal;
			Vec3 gp1, gp2, gp3;

			p1.y += vehicle_step_height;
			p2.y += vehicle_step_height;
			p3.y += vehicle_step_height;

			const Float ray_dist = 100.0;


			//check ground and static geometry for now
			GeometryFlags flags = static_cast<GeometryFlags>(GEOMETRY_FLAG_GROUND | GEOMETRY_FLAG_STATIC_OBJECT);/* | GEOMETRY_FLAG_PAGED_LOD);*/

			/*if (m_Heightmap)
			{
				p1.y = m_Heightmap->GetHeightAtWorldLocation(p1.x, p1.z);
				p2.y = m_Heightmap->GetHeightAtWorldLocation(p2.x, p2.z);
			}*/
			
			//do tripod ground clamp
			ScenePtr scene = GetSceneObject()->GetScene();
			if (CollisionHelper::GetGroundData(scene, p1, ray_dist, flags, gp1, normal) &&
				CollisionHelper::GetGroundData(scene, p2, ray_dist, flags, gp2, normal) &&
				CollisionHelper::GetGroundData(scene, p3, ray_dist, flags, gp3, normal))
			{

				Vec3 v1 = gp2 - gp1;
				Vec3 v2 = gp3 - gp1;
				Vec3 y_dir = -Math::Cross(v1, v2);
				y_dir.Normalize();

				//pick average height
				m_CurrentPos.y = (gp1.y + ((gp2.y + gp3.y)*0.5))*0.5;

				//Generate new rotation
				Vec3 x_dir = gp2 - gp3;
				x_dir.Normalize();
				Vec3 z_dir = Math::Cross(x_dir, y_dir);
				z_dir.Normalize();
				m_CurrentRot.FromAxes(x_dir, y_dir, z_dir);
			}
		}
	}

	void MotionComponent::OnInput(InputRelayEventPtr message)
	{
		std::string name = message->GetController();
		float value = message->GetValue();
		if(name == "Throttle")
		{
			m_ThrottleInput = value;
		}
		else if(name == "Steer")
		{
			m_SteerInput = value;
		}
		else if (name == "Break")
		{
			m_BreakInput = value;
		}
	}

	void MotionComponent::SceneManagerTick(double delta_time)
	{
		StepPhysics(delta_time);
	}

	void MotionComponent::StepPhysics(double delta_time)
	{
		//Float turn_speed_x = 0;
		Float turn_speed_y = 0;
		
		//turn_speed_x = -m_PitchInput*Math::Deg2Rad(m_MaxTurnSpeed);
		turn_speed_y = m_SteerInput*Math::Deg2Rad(m_MaxTurnSpeed);
		
		m_CurrentSpeed = m_CurrentSpeed + m_ThrottleInput*m_Acceleration*delta_time;

		if (m_BreakInput > 0)
		{ 
			if (m_CurrentSpeed > 0)
			{
				m_CurrentSpeed = m_CurrentSpeed - m_BreakInput*m_Acceleration*delta_time;
				if (m_CurrentSpeed < 0)
					m_CurrentSpeed = 0;
			}
			else if (m_CurrentSpeed < 0)
			{
				m_CurrentSpeed = m_CurrentSpeed + m_BreakInput*m_Acceleration*delta_time;
				if (m_CurrentSpeed > 0)
					m_CurrentSpeed = 0;
			}
		}

		if (m_CurrentSpeed > m_MaxSpeed)
			m_CurrentSpeed = m_MaxSpeed;

		if (m_CurrentSpeed < -m_MaxSpeed)
			m_CurrentSpeed = -m_MaxSpeed;
		
		Vec3 forward_vel;
		
		forward_vel = -m_CurrentRot.GetZAxis();
		forward_vel.Normalize();
		forward_vel = forward_vel * m_CurrentSpeed;

		Vec3 new_pos = m_CurrentPos + forward_vel*delta_time;
		Quaternion rot;

		Vec3 up_vec= m_CurrentRot.GetYAxis();
		rot.FromAngleAxis(turn_speed_y, Vec3(0, 1, 0));
		Quaternion new_rot = m_CurrentRot * rot;
		
		int from_id = GASS_PTR_TO_INT(this);
		GetSceneObject()->PostRequest(WorldRotationRequestPtr(new WorldRotationRequest(new_rot, from_id)));
		GetSceneObject()->PostRequest(WorldPositionRequestPtr(new WorldPositionRequest(new_pos, from_id)));
		GetSceneObject()->PostEvent(PhysicsVelocityEventPtr(new PhysicsVelocityEvent(Vec3(0, 0, -m_CurrentSpeed), Vec3(0, 0, 0), from_id)));
		
		if(m_Debug)
		{
			GASS_PRINT("m_CurrentSpeed:" << m_CurrentSpeed);
			GASS_PRINT("m_ThrottleInput:" << m_ThrottleInput);
			GASS_PRINT("m_SteerInput:" << m_SteerInput);
			GASS_PRINT("m_BreakInput:" << m_BreakInput);
			GASS_PRINT("new_pos.y:" << new_pos.y);

			/*Vec3 new_vel = -new_rot.GetZAxis()*10;
			Vec3 old_vel = -m_CurrentRot.GetZAxis() * 10;
			Vec3 xdir = m_CurrentRot.GetXAxis() * 10;
			GASS_DRAW_LINE(m_CurrentPos, (m_CurrentPos + up_vec), ColorRGBA(1, 0, 0, 1));
			GASS_DRAW_LINE(m_CurrentPos, (m_CurrentPos + old_vel), ColorRGBA(0, 1, 0, 1));
			GASS_DRAW_LINE(m_CurrentPos, (m_CurrentPos + xdir), ColorRGBA(0, 0, 1, 1));*/
		}

		//reset!
		m_SteerInput = 0;
	}
}
