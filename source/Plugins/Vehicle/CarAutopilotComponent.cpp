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

#include "CarAutopilotComponent.h"
#include "Core/Math/GASSMath.h"
#include "Core/Math/GASSQuaternion.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSGraphicsSceneMessages.h"
#include "Sim/Messages/GASSPlatformMessages.h"
#include "Sim/Messages/GASSInputMessages.h"
#include "Sim/Interface/GASSIMissionSceneManager.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include <float.h>

#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"


namespace GASS
{
	CarAutopilotComponent::CarAutopilotComponent()  : m_ThrottleInput("Throttle"),
		m_SteerInput("Steer"),
		m_DesiredPosRadius(0),
		m_DesiredPos(0,0,0),
		m_CurrentPos(0,0,0),
		m_DesiredSpeed(0),
		m_Enable(false),
		m_WPReached(false),
		m_VehicleSpeed(0,0,0),
		m_BrakeDistanceFactor(1.0),
		m_InvertBackWardSteering(true),
		m_Support3PointTurn(true),
		m_FaceDirection(0,0,0),
		m_HasDir(false),
		m_MaxReverseDistance(5),
		m_PlatformType(PT_CAR),
		m_HasCollision(false),
		m_CollisionPoint(0,0,0),
		m_CollisionDist(0),
		m_CollisionAvoidance(false)
	{
		m_TurnPID.setGain(2.0,0.02,0.01);
		m_TrottlePID.setGain(1.0,0,0);

	}

	CarAutopilotComponent::~CarAutopilotComponent()
	{

	}

	void CarAutopilotComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register<CarAutopilotComponent>();
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("CarAutopilotComponent", OF_VISIBLE)));

		RegisterGetSet("SteerInput", &CarAutopilotComponent::GetSteerInput, &CarAutopilotComponent::SetSteerInput,PF_VISIBLE,"Input mapping for steer");
		RegisterGetSet("ThrottleInput", &CarAutopilotComponent::GetThrottleInput, &CarAutopilotComponent::SetThrottleInput,PF_VISIBLE,"Input mapping for throttle");
		RegisterMember("DesiredSpeed", &CarAutopilotComponent::m_DesiredSpeed,PF_VISIBLE  | PF_EDITABLE,"Desired speed");
		RegisterMember("Enable", &CarAutopilotComponent::m_Enable,PF_VISIBLE  | PF_EDITABLE,"Enable/Disable this component");
		RegisterMember("DesiredPosRadius", &CarAutopilotComponent::m_DesiredPosRadius,PF_VISIBLE  | PF_EDITABLE,"Enable/Disable this component");
		RegisterMember("BrakeDistanceFactor", &CarAutopilotComponent::m_BrakeDistanceFactor,PF_VISIBLE  | PF_EDITABLE,"Multiplier for default linear brake distance (1m at 1m/s) ");
		RegisterMember("MaxReverseDistance", &CarAutopilotComponent::m_MaxReverseDistance,PF_VISIBLE  | PF_EDITABLE,"Max waypoint distance to use reverse");

		RegisterMember("TurnPID", &CarAutopilotComponent::m_TurnPID,PF_VISIBLE  | PF_EDITABLE,"Steer PID regulator values");
		RegisterMember("TrottlePID", &CarAutopilotComponent::m_TrottlePID,PF_VISIBLE  | PF_EDITABLE,"Throttle PID regulator values");

		RegisterMember("Support3PointTurn", &CarAutopilotComponent::m_Support3PointTurn,PF_VISIBLE  | PF_EDITABLE,"Enable/Disable Three Point Turn");

		RegisterMember("InvertBackWardSteering", &CarAutopilotComponent::m_InvertBackWardSteering,PF_VISIBLE  | PF_EDITABLE,"");

		RegisterMember("CollisionAvoidance", &CarAutopilotComponent::m_CollisionAvoidance, PF_VISIBLE | PF_EDITABLE,"Try to avoid collision with other entities");
	}

	void CarAutopilotComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(CarAutopilotComponent::OnGotoPosition,GotoPositionRequest,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(CarAutopilotComponent::OnSetDesiredSpeed,DesiredSpeedMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(CarAutopilotComponent::OnFaceDirectionRequest,FaceDirectionRequest,0));

		GetSceneObject()->RegisterForMessage(REG_TMESS(CarAutopilotComponent::OnPhysicsMessage,PhysicsVelocityEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(CarAutopilotComponent::OnTransMessage,TransformationChangedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(CarAutopilotComponent::OnRadarEvent,VehicleRadarEvent,0));

		GetSceneObject()->RegisterForMessage(REG_TMESS(CarAutopilotComponent::OnSensorEvent,SensorMessage,0));

		RegisterForPostUpdate<IMissionSceneManager>();
		
		PlatformComponentPtr platform = GetSceneObject()->GetFirstComponentByClass<IPlatformComponent>();
		if(platform)
		{
			m_PlatformType = platform->GetType();
		}
		m_Terrain = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<ICollisionSceneManager>().get();
	}

	void CarAutopilotComponent::OnDelete()
	{

	}

	void CarAutopilotComponent::OnGotoPosition(GotoPositionRequestPtr message)
	{
		Vec3 pos = message->GetPosition();
		m_DesiredPos.Set(pos.x,pos.y,pos.z);
		m_WPReached = false;
		m_HasDir = false;
		m_Enable = true;
	}

	void CarAutopilotComponent::OnRadarEvent(VehicleRadarEventPtr message)
	{
		m_HasCollision = message->m_HasIsect;
		m_CollisionPoint = message->m_IsectPos;
		m_CollisionDist = message->m_IsectDist;
	}

	void CarAutopilotComponent::OnSensorEvent(SensorMessagePtr message)
	{
		//only save closest entity?
		m_ProximityData = message->GetDetectionVector();
	}

	void CarAutopilotComponent::OnFaceDirectionRequest(FaceDirectionRequestPtr message)
	{
		m_FaceDirection = message->GetDirection();
		m_HasDir = true;
	}

	void CarAutopilotComponent::OnSetDesiredSpeed(DesiredSpeedMessagePtr message)
	{
		//std::stringstream ss;
		//ss << "SPeed" << message->GetSpeed();
		//LogManager::getSingleton().logMessage(ss.str(), LML_NORMAL);
		m_DesiredSpeed = message->GetSpeed();
		m_TrottlePID.setIntCap(200);
		m_TrottlePID.setIntSum(0);
	}

	void CarAutopilotComponent::OnTransMessage(TransformationChangedEventPtr message)
	{
		m_CurrentPos = message->GetPosition();
		Quaternion  rot = message->GetRotation();
		m_Transformation.MakeTransformationRT(rot, m_CurrentPos);
		//rot.ToRotationMatrix(m_Transformation);
		//m_Transformation.SetTranslation(m_CurrentPos);
	}

	void CarAutopilotComponent::OnPhysicsMessage(PhysicsVelocityEventPtr message)
	{
		Vec3 ang_vel  = message->GetAngularVelocity();
		m_AngularVelocity = ang_vel;
		m_VehicleSpeed  = message->GetLinearVelocity();
	}

	void CarAutopilotComponent::SceneManagerTick(double delta)
	{
		if(m_Enable)
		{
			_UpdateDrive(delta);
		}
	}
#if 0
	void CarAutopilotComponent::_UpdateDrive(double delta_time)
	{
		Vec3 target_pos  = m_DesiredPos;
		target_pos.y = m_CurrentPos.y;
		Float desired_speed	= m_DesiredSpeed;
		Vec3 current_dir = -m_Transformation.GetZAxis();
		float current_speed = static_cast<float>(-m_VehicleSpeed.z);
		Vec3 current_pos = m_CurrentPos;
		Vec3 target_dir = target_pos - current_pos;
		Float target_dist = target_dir.Length();

		if (m_CollisionAvoidance && m_ProximityData.size() > 0)
		{
			Float min_dist = FLT_MAX;
			DetectionData dd = m_ProximityData[0];
			for(size_t  i = 0; i < m_ProximityData.size();i++)
			{
				Float dist = (m_CurrentPos - m_ProximityData[i].Pos).Length();
				if(dist < min_dist)
				{
					min_dist = dist;
					dd = m_ProximityData[i];
				}
			}

			const Float col_radie1 = 3;
			const Float col_radie2 = 3;
			const Float col_radie = col_radie1 + col_radie2;

			Vec3 target_to_col_dir = target_pos - dd.Pos;
			Float target_to_col_dist = target_to_col_dir.Length();
			if(target_to_col_dist < col_radie) //inside collision sphere
			{
				//always turn to right side of sphere

				Vec3 per_pend_target_dir = target_dir;
				per_pend_target_dir.x = -target_dir.z;
				per_pend_target_dir.z = target_dir.x;
				per_pend_target_dir.Normalize();

				//check if behind
				Vec3 col_dir =  dd.Pos - m_CurrentPos;
				col_dir.Normalize();
				Vec3 drive_dir = -m_Transformation.GetZAxis();
				const Float cos_angle = Vec3::Dot(drive_dir,col_dir);
				if(cos_angle > -0.1)
				{
					target_pos = target_pos + per_pend_target_dir*col_radie;
					target_dir = target_pos - current_pos;
					target_dist = target_pos.Length();
				}
			}
		}

		if(target_dist > 0)// && dist_to_wp > m_DesiredPosRadius)
		{
			target_dir.y = 0;
			target_dir.Normalize();

			current_dir.y = 0;
			current_dir.Normalize();

			Vec3 cross = Vec3::Cross(current_dir,target_dir);
			float cos_angle = static_cast<float>(Vec3::Dot(current_dir,target_dir));

			if(cos_angle > 1)
				cos_angle = 1;
			if(cos_angle < -1)
				cos_angle = -1;
			float angle_to_target_dir = static_cast<float>(Math::Rad2Deg(acos(cos_angle)));
			if(cross.y < 0)
				angle_to_target_dir *= -1;


			m_TurnPID.setIntCap(60.0);
			m_TurnPID.set(0);
			float turn = static_cast<float>(m_TurnPID.update(angle_to_target_dir, delta_time));

			/*GASS_PRINT("angle_to_target_dir:" << angle_to_target_dir);
			GASS_PRINT("turn:" << turn);
			GASS_PRINT("intsum:" << m_TurnPID.getIntSum());*/

			// damp speed if we have to turn sharp
			if(m_Support3PointTurn && fabs(angle_to_target_dir) > 90 && target_dist > m_MaxReverseDistance)// do three point turn if more than 20 meters turn on point
			{
				desired_speed *= -1;
				if(m_InvertBackWardSteering && current_speed < 0)  //check that not rolling forward
					turn *=-1;
				//else
				//	turn = 0;

			}
			else if(fabs(angle_to_target_dir) > 80 && target_dist < m_MaxReverseDistance)// do three point turn or just reverse?
			{
				desired_speed *= -1;
				if(m_Support3PointTurn && m_InvertBackWardSteering)
				{
					if(current_speed < 0 && fabs(angle_to_target_dir) < 120) //if less than 110 deg do three point turn
						turn *=-1;
					else //damp turn, we try to reverse!
						turn *= 0.03f;
				}
				//else if(!m_InvertBackWardSteering)
				//	turn *=-1;
				else if(!m_InvertBackWardSteering)
				{
					m_TurnPID.set(180);
					turn = static_cast<float>(m_TurnPID.update(angle_to_target_dir, delta_time));
				}

			}
			else
			{
				//slow down if we are turning sharp and speed is to high
				Float min_steer_speed = 5;
				Float max_steer_speed = 20;

				if(current_speed > min_steer_speed)
				{
					Float w = (current_speed - min_steer_speed)/(max_steer_speed - min_steer_speed);
					desired_speed = desired_speed * (1.0 - w) + w * (desired_speed * fabs(cos_angle));
				}

				if(m_InvertBackWardSteering && current_speed < 0) //you want to go forward but rolling backward, invert steering
					turn *=-1;
			}

			//Linear damp speed if we are inside radius from waypoint,
			//the radius is dynamic and is based on the desired speed of the vehicle.
			//The vehicle will have longer brake distance at high speed
			//and this is compensated by taking the speed in consideration.
			//The user can tweak this radius with the m_BrakeDistanceFactor based
			//on the properties of the vehicle, better brakes == lower m_BrakeDistanceFactor value
			//By default this value is 1 which means that the vehicle can come to rest
			//after 1m if traveling at 1 m/s. This formula should probably be expanded to support
			//more non linear behavior
			Float radius = fabs(desired_speed)*m_BrakeDistanceFactor;
			if(target_dist > 0 && target_dist < radius)
			{
				 desired_speed = desired_speed * (target_dist/radius);
			}

			if(m_DesiredPosRadius > 0  && target_dist < m_DesiredPosRadius)
			{
				desired_speed = 0;
				turn = 0;
				m_WPReached = true;
			}

			//do collision check
#if(0)
			if(m_HasCollision)
			{
				//Check dist
				//Vec3 col_vec =  m_CollisionPoint - m_CurrentPos;

				//Float dist_to_col = col_vec.Length();

				if(m_CollisionDist < 20) //slow down
				{
					Float speed_interp = m_CollisionDist/20;
					desired_speed = desired_speed*speed_interp;
					//desired_speed = -1;
					//turn = 0.0;//speed_interp;
				}
				if(m_CollisionDist < 5) //go back?
				{
					desired_speed = -2.6;
					if(current_speed < 0)
						turn = -1;
					else
						turn = 1;
				}

				/*if(m_CollisionDist < 0.5) //reverse?
				{
					desired_speed = -2.3;
					turn = -0.2;
				}*/
				/*std::stringstream ss;
				ss  <<  GetSceneObject()->GetName();
				ss  <<  "m_CollisionDist" << m_CollisionDist;
				GetSceneObject()->PostRequest(TextCaptionRequestPtr(new TextCaptionRequest(ss.str())));*/

			}
			else
			{
				/*std::stringstream ss;
				ss  <<  GetSceneObject()->GetName();
				ss  <<  "NO COL";
				GetSceneObject()->PostRequest(TextCaptionRequestPtr(new TextCaptionRequest(ss.str())));*/
			}
#endif

			m_TrottlePID.set(desired_speed);
			float throttle = static_cast<float>(m_TrottlePID.update(current_speed, delta_time));
			
		/*	GASS_PRINT("throttle:" << throttle);
			GASS_PRINT("desired_speed:" << desired_speed);
			GASS_PRINT("speed:" << current_speed);
			GASS_PRINT("intsum:" << m_TrottlePID.getIntSum());*/
			
			if(throttle > 1) throttle = 1;
			if(throttle < -1) throttle = -1;

			if(turn > 1) turn  = 1;
			if(turn < -1) turn  = -1;

			float break_value = 0.0;
			if(m_WPReached) //apply desired end rotation if we have reached end location
			{
				if(m_HasDir)
				{
					Vec3 up_vec = Vec3::Cross(current_dir,m_FaceDirection);
					float cos_face_angle = static_cast<float>(Vec3::Dot(current_dir,m_FaceDirection));

					if(cos_face_angle > 1)
						cos_face_angle = 1;
					if(cos_face_angle < -1)
						cos_face_angle = -1;
					float angle_to_face = static_cast<float>(Math::Rad2Deg(acos(cos_face_angle)));
					if(up_vec.y < 0)
						angle_to_face *= -1;
					m_TurnPID.set(0);
					turn = static_cast<float>(m_TurnPID.update(angle_to_face, delta_time));
					throttle = 0;
				}
				if(m_PlatformType == PT_HUMAN)
					throttle = 0;

				if(m_PlatformType != PT_HUMAN)
				{
					throttle = 0;
					turn = 0;
				}

				break_value = 1.0f;
				
			}

			GetSceneObject()->SendImmediateEvent(InputRelayEventPtr(new InputRelayEvent("",m_ThrottleInput,throttle,CT_AXIS)));
			GetSceneObject()->SendImmediateEvent(InputRelayEventPtr(new InputRelayEvent("",m_SteerInput,-turn,CT_AXIS)));
			GetSceneObject()->SendImmediateEvent(InputRelayEventPtr(new InputRelayEvent("", "Break", break_value, CT_AXIS)));
		}
		else
		{
			GetSceneObject()->SendImmediateEvent(InputRelayEventPtr(new InputRelayEvent("",m_ThrottleInput,0,CT_AXIS)));
			GetSceneObject()->SendImmediateEvent(InputRelayEventPtr(new InputRelayEvent("",m_SteerInput,0,CT_AXIS)));
			GetSceneObject()->SendImmediateEvent(InputRelayEventPtr(new InputRelayEvent("", "Break", 1.0f, CT_AXIS)));
		}
	}
#endif

	double GetAngleOnPlane(const Vec3& plane_normal, const Vec3& v1, const Vec3& v2)
	{
		Vec3d cross = Vec3d::Cross(v1, v2);
		double cos_angle = Vec3d::Dot(v1, v2);
		if (cos_angle > 1) cos_angle = 1;
		if (cos_angle < -1) cos_angle = -1;
		double angle = Math::Rad2Deg(acos(cos_angle));
		if (Vec3::Dot(plane_normal, cross) > 0)
			angle *= -1;
		return angle;
	}


	CarAutopilotComponent::NavState CarAutopilotComponent::_DecideNavState(const double angle_to_target_dir, const double target_dist, const double turn_radius) const
	{
		const bool target_inside_turn_radius = target_dist < turn_radius&& fabs(angle_to_target_dir) > 45;
		const bool target_behinde = fabs(angle_to_target_dir) > 90;
		const bool reverse_possible = target_dist < m_MaxReverseDistance;//&& fabs(angle_to_target_dir) > 90;
		
		NavState nav_state = UNDEF;
		if (target_inside_turn_radius)
		{
			if (m_MaxReverseDistance > 0)
			{
				//std::cout << "angle_to_target_dir:" << fabs(angle_to_target_dir) << "\n";
				if (fabs(angle_to_target_dir) > 45)
				{
					if (reverse_possible && fabs(angle_to_target_dir) > 135)
						nav_state = REVERSE_TO_TARGET;
					else
						nav_state = REVERSE_AWAY_FROM_TARGET;
				}
			}
			else
			{
				nav_state = FORWARD_AWAY_FROM_TARGET;
			}
		}
		else if (reverse_possible && target_behinde)
		{
			nav_state = REVERSE_TO_TARGET;
		}
		else if (m_Support3PointTurn && target_behinde)
		{
			nav_state = REVERSE_AWAY_FROM_TARGET;
		}
		else
		{
			nav_state = FORWARD_TO_TARGET;
		}
		return nav_state;
	}

	double Clampd(double value, double min_val, double max_val)
	{
		return std::min(max_val, std::max(min_val, value));
	}

	double CarAutopilotComponent::_UpdateSteerInput(const CarAutopilotComponent::NavState nav_state, const double delta_time, const double angle_to_target_dir)
	{
		m_TurnPID.setIntCap(60.0);
		m_TurnPID.set(0);

		const double reverse_angle_to_target = angle_to_target_dir > 0 ? -(angle_to_target_dir - 180) : -(angle_to_target_dir + 180);
		const double current_speed = -m_VehicleSpeed.z;

		double steer_input = 0;

		if (nav_state == FORWARD_TO_TARGET)
		{
			steer_input = -m_TurnPID.update(angle_to_target_dir, delta_time);
		}
		else if (nav_state == REVERSE_TO_TARGET)
		{
			steer_input = m_TurnPID.update(reverse_angle_to_target, delta_time);
		}
		else if (nav_state == FORWARD_AWAY_FROM_TARGET)
		{
			steer_input = m_TurnPID.update(reverse_angle_to_target, delta_time);
		}
		else if (nav_state == REVERSE_AWAY_FROM_TARGET)
		{
			steer_input = -m_TurnPID.update(angle_to_target_dir, delta_time);
		}

		if (m_InvertBackWardSteering && current_speed < 0) //invert reverse steering
			steer_input *= -1;

		steer_input = Clampd(steer_input, -1, 1);
		return steer_input;
	}

	double CarAutopilotComponent::_CalcDesiredSpeed(const CarAutopilotComponent::NavState nav_state, double target_dist, double cos_angle_to_target)
	{
		const double current_speed = -m_VehicleSpeed.z;
		double desired_speed = m_DesiredSpeed;
		if (nav_state == FORWARD_TO_TARGET ||
			nav_state == FORWARD_AWAY_FROM_TARGET)
		{
			//slow down if we are turning sharp and speed is to high
			const double min_steer_speed = 5;
			const double max_steer_speed = 20;
			if (current_speed > min_steer_speed)
			{
				const double w = (current_speed - min_steer_speed) / (max_steer_speed - min_steer_speed);
				desired_speed = desired_speed * (1.0 - w) + w * (desired_speed * fabs(cos_angle_to_target));
			}
		}

		if (nav_state == REVERSE_TO_TARGET || 
			nav_state == REVERSE_AWAY_FROM_TARGET)
		{
			desired_speed *= -1;
		}
	
		//Linear damp speed if we are inside radius from waypoint,
		//the radius is dynamic and is based on the desired speed of the vehicle.
		//The vehicle will have longer brake distance at high speed
		//and this is compensated by taking the speed in consideration.
		//The user can tweak this radius with the m_BrakeDistanceFactor based
		//on the properties of the vehicle, better brakes == lower m_BrakeDistanceFactor value
		//By default this value is 1 which means that the vehicle can come to rest
		//after 1m if traveling at 1 m/s. This formula should probably be expanded to support
		//more non linear behavior
		double radius = fabs(desired_speed) * m_BrakeDistanceFactor;
		if (target_dist > 0 && target_dist < radius)
		{
			desired_speed = desired_speed * (target_dist / radius);
		}

		if (m_DesiredPosRadius > 0 && target_dist < m_DesiredPosRadius)
		{
			desired_speed = 0;
		}

		return desired_speed;
	}
	
	void CarAutopilotComponent::_SendInput(double steer_input, double throttle_input, double brake_input)
	{
		GetSceneObject()->SendImmediateEvent(InputRelayEventPtr(new InputRelayEvent("", m_ThrottleInput, static_cast<float>(throttle_input), CT_AXIS)));
		GetSceneObject()->SendImmediateEvent(InputRelayEventPtr(new InputRelayEvent("", m_SteerInput, static_cast<float>(steer_input), CT_AXIS)));
		GetSceneObject()->SendImmediateEvent(InputRelayEventPtr(new InputRelayEvent("", "Break", static_cast<float>(brake_input), CT_AXIS)));
	}

	void CarAutopilotComponent::_UpdateDrive(double delta_time)
	{
		const Vec3d vehicle_pos = m_CurrentPos;
		const Plane plane = [&] {
			Vec3d local_up_vector(0, 1, 0);
			m_Terrain->GetUpVector(vehicle_pos, local_up_vector);
			return Plane(vehicle_pos, local_up_vector);
		}();
		const Vec3d vehicle_dir = plane.GetProjectedVector(-m_Transformation.GetZAxis()).NormalizedCopy();
		const double vehicle_speed = -m_VehicleSpeed.z;
		const Vec3d target_pos = plane.GetProjectedPoint(m_DesiredPos);
		const Vec3d target_vec = target_pos - vehicle_pos;
		const Vec3d target_dir = target_vec.NormalizedCopy();
		const double target_dist = target_vec.Length();
		
		if (m_DesiredPosRadius > 0 && target_dist < m_DesiredPosRadius)
			m_WPReached = true;

		if (m_WPReached) 
		{
			//apply desired end direction if we have reached target location
			double steer_input = 0.0f;
			if (m_HasDir && m_PlatformType == PT_HUMAN)
			{
				//TODO, refactor and TEST this
				const Vec3d face_dir = plane.GetProjectedVector(m_FaceDirection).NormalizedCopy();
				const double angle_to_face = GetAngleOnPlane(plane.m_Normal, face_dir, vehicle_dir);
				m_TurnPID.set(0);
				steer_input = m_TurnPID.update(angle_to_face, delta_time);
			}
			const double brake_input = 1.0f;
			const double throttle_input = 0.0f;
			_SendInput(steer_input, throttle_input, brake_input);
		}
		else if (target_dist > 0)
		{
			const double angle_to_target = GetAngleOnPlane(plane.m_Normal, target_dir, vehicle_dir);
			//GASS_PRINT("angle_to_target_dir:" << angle_to_target_dir);
			
			const double vehicle_turn_radius = 10;
			const NavState nav_state = _DecideNavState(angle_to_target, target_dist, vehicle_turn_radius);
			const double steer_input = _UpdateSteerInput(nav_state, delta_time, angle_to_target);

			const double cos_angle_to_target = Clampd(Vec3d::Dot(vehicle_dir, target_dir), -1, 1);
			const double desired_speed = _CalcDesiredSpeed(nav_state, target_dist, cos_angle_to_target);
			m_TrottlePID.set(desired_speed);
			const double throttle_input = Clampd(m_TrottlePID.update(vehicle_speed, delta_time), -1, 1);
			const double brake_input = 0.0;
			_SendInput(steer_input, throttle_input, brake_input);
		}
		else
		{
			_SendInput(0, 0, 1);
		}
	}
}
