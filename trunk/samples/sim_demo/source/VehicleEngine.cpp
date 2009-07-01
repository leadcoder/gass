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

#include "VehicleEngine.h"
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

	VehicleWheel::VehicleWheel(SceneObjectPtr  wheel) : m_Velocity(0), 
		m_AngularVelocity (0),
		m_WheelObject(wheel)
	{
		if(wheel)
			wheel->RegisterForMessage(SceneObject::OBJECT_MESSAGE_PHYSICS, MESSAGE_FUNC(VehicleWheel::OnPhysicsMessage));
	}

	VehicleWheel::~VehicleWheel()
	{
		SceneObjectPtr wheel_obj(m_WheelObject,boost::detail::sp_nothrow_tag());
		if(wheel_obj)
			wheel_obj->UnregisterForMessage(SceneObject::OBJECT_MESSAGE_PHYSICS, MESSAGE_FUNC(VehicleWheel::OnPhysicsMessage));

	}

	void VehicleWheel::OnPhysicsMessage(MessagePtr message)
	{
		//fetch wheel rpm
		Vec3 vel  = boost::any_cast<Vec3>(message->GetData("Velocity"));
		m_Velocity = vel.x;
		Vec3 ang_vel  = boost::any_cast<Vec3>(message->GetData("AngularVelocity"));
		m_AngularVelocity = ang_vel.x;
	}

	VehicleEngine::VehicleEngine() :m_Initialized(false), m_VehicleSpeed(0)
	{
		m_RPM = 0;
		m_AutoShiftStart = 0;
		m_ThrottleAccel = 2;
		m_TurnForce = 10;
		m_MaxTurnForce = 200;
		m_FakeRPMOutput = false;
		m_Invert = false;

		m_InputToThrottle = "Throttle";
		m_InputToSteer = "Steering";

		m_GearBoxRatio.resize(6);
		m_GearBoxRatio[0] = -16.42;
		m_GearBoxRatio[1] = 0;
		m_GearBoxRatio[2] = 16.42;
		m_GearBoxRatio[3] = 12.15;
		m_GearBoxRatio[4] = 11.52;
		m_GearBoxRatio[5] = 11.17;
		m_EngineType = ET_TANK;
		//m_GearBoxRatio[6] = 0.71;

		m_NeutralGear = 1;
		m_Gear = 2;
		m_Clutch = 1;
		m_Automatic = 1;
	
		m_RPMGearChangeUp = 1500;
		m_RPMGearChangeDown = 700;
		m_MaxBrakeTorque = 1000;
		m_MinRPM = 500;
		m_MaxRPM = 4000;

		m_DeclutchTimeChangeGear = 0.5;
		m_ClutchTimeChangeGear = 0.5;
		m_AutoClutchStart = 0;
		m_CurrentTime = 0;
		m_DesiredThrottle = 0;
		m_DesiredSteer = 0;
		m_VehicleEngineRPM = 0;

		m_Power = 0.2;
		m_SteerCtrl = PIDControl(1,1,1);
		m_AngularVelocity = Vec3(0,0,0);
	}

	VehicleEngine::~VehicleEngine()
	{

	}

	void VehicleEngine::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("VehicleEngine",new Creator<VehicleEngine, IComponent>);
		RegisterProperty<std::vector<std::string>>("Wheels", &VehicleEngine::GetWheels, &VehicleEngine::SetWheels);
		RegisterProperty<std::string>("EngineType", &GetEngineType, &SetEngineType);
		RegisterProperty<bool>("Automatic", &GetAutomatic, &SetAutomatic);
		RegisterProperty<float>("BrakeTorque", &GetBrakeTorque, &SetBrakeTorque);
		RegisterProperty<float>("DeclutchTimeChangeGear", &GetDeclutchTimeChangeGear, &SetDeclutchTimeChangeGear);
		RegisterProperty<float>("ClutchTimeChangeGear", &GetClutchTimeChangeGear, &SetClutchTimeChangeGear);

		RegisterProperty<float>("MaxRPM", &GetMaxRPM, &SetMaxRPM);
		RegisterProperty<float>("MinRPM", &GetMinRPM, &SetMinRPM);

		RegisterProperty<float>("RPMGearChangeUp", &GetRPMGearChangeUp, &SetRPMGearChangeUp);
		RegisterProperty<float>("RPMGearChangeDown", &GetRPMGearChangeDown, &SetRPMGearChangeDown);
		RegisterProperty<float>("Power", &GetPower, &SetPower);
		RegisterProperty<float>("TurnForce", &GetTurnForce, &SetTurnForce);
		RegisterProperty<std::vector<float>>("GearRatio", &GetGearRatio, &SetGearRatio);
		//RegisterProperty<bool>("FakeRPMOutput", &GetFakeRPMOutput, &SetFakeRPMOutput);
	}


	void VehicleEngine::SetEngineType(const std::string &type)
	{
		if(type == "car")
		{
			m_EngineType = ET_CAR;
		}
		else if(type == "tank")
		{
			m_EngineType = ET_TANK;
		}
	}
	std::string VehicleEngine::GetEngineType()const
	{
		std::string type;
		switch(m_EngineType )
		{
		case ET_CAR:
			type = "car";
			break;
		case ET_TANK:
			type = "tank";
			break;
		}
		return type;
	}

	bool VehicleEngine::GetAutomatic() const
	{
		return m_Automatic;
	}

	void VehicleEngine::SetAutomatic(bool value)
	{
		m_Automatic = value;
	}

	float VehicleEngine::GetBrakeTorque() const
	{
		return m_MaxBrakeTorque;
	}

	void VehicleEngine::SetBrakeTorque(float value)
	{
		m_MaxBrakeTorque = value;
	}

	float VehicleEngine::GetDeclutchTimeChangeGear() const
	{
		return m_DeclutchTimeChangeGear;
	}

	void VehicleEngine::SetDeclutchTimeChangeGear(float value)
	{
		m_DeclutchTimeChangeGear = value;
	}

	float VehicleEngine::GetClutchTimeChangeGear() const
	{
		return m_ClutchTimeChangeGear;
	}

	void VehicleEngine::SetClutchTimeChangeGear(float value)
	{
		m_ClutchTimeChangeGear = value;
	}

	float VehicleEngine::GetMaxRPM() const
	{
		return m_MaxRPM;
	}

	void VehicleEngine::SetMaxRPM(float value)
	{
		m_MaxRPM = value;
	}

	float VehicleEngine::GetMinRPM() const
	{
		return m_MinRPM;
	}

	void VehicleEngine::SetMinRPM(float value)
	{
		m_MinRPM = value;
	}


	float VehicleEngine::GetRPMGearChangeUp() const
	{
		return m_RPMGearChangeUp;
	}

	void VehicleEngine::SetRPMGearChangeUp(float value)
	{
		m_RPMGearChangeUp = value;
	}

	float VehicleEngine::GetRPMGearChangeDown() const
	{
		return m_RPMGearChangeDown;
	}

	void VehicleEngine::SetRPMGearChangeDown(float value)
	{
		m_RPMGearChangeDown = value;
	}


	
	float VehicleEngine::GetPower() const
	{
		return m_Power;
	}

	void VehicleEngine::SetPower(float value)
	{
		m_Power = value;
	}

	float VehicleEngine::GetTurnForce() const
	{
		return m_TurnForce;
	}

	void VehicleEngine::SetTurnForce(float value)
	{
		m_TurnForce = value;
	}

	void VehicleEngine::SetGearRatio(const std::vector<float> &gear_data)
	{
		m_GearBoxRatio = gear_data;

		//find neutral
		for(int i = 0; i < m_GearBoxRatio.size(); i++)
		{
			if(m_GearBoxRatio[i] == 0.0)
			{
				m_NeutralGear = i;
			}
		}
		m_Gear = m_NeutralGear;
	}

	std::vector<float> VehicleEngine::GetGearRatio() const
	{
		return m_GearBoxRatio;
	}

	std::vector<std::string> VehicleEngine::GetWheels() const
	{
		return m_WheelNames;
	}

	void VehicleEngine::SetWheels(const std::vector<std::string> wheels)
	{
		m_WheelNames = wheels;
		//Update wheel list
		if(m_Initialized)
		{
			for(int i = 0; i < m_WheelNames.size(); i++)
			{
				SceneObjectVector objects;
				GetSceneObject()->GetObjectUnderRoot()->GetObjectsByName(objects,m_WheelNames[i],false);
				if(objects.size() > 0)
				{
					VehicleWheelPtr wheel(new VehicleWheel(objects.front()));
					m_Wheels.push_back(wheel);
				}
			}
		}
	}

	void VehicleEngine::OnCreate()
	{
		ControlSetting* cs = SimEngine::Get().GetControlSettingsManager()->GetControlSetting("VehicleEngineInputSettings");
		if(cs)
			cs->GetMessageManager()->RegisterForMessage(ControlSetting::CONTROLLER_MESSAGE_NEW_INPUT, MESSAGE_FUNC(VehicleEngine::OnInput));
		else 
			Log::Warning("Failed to find control settings: VehicleEngineInputSettings");
		GetSceneObject()->RegisterForMessage(SceneObject::OBJECT_MESSAGE_LOAD_USER_COMPONENTS, MESSAGE_FUNC(VehicleEngine::OnLoad));
		GetSceneObject()->RegisterForMessage(SceneObject::OBJECT_MESSAGE_UNLOAD_COMPONENTS, MESSAGE_FUNC(VehicleEngine::OnUnload));
		GetSceneObject()->RegisterForMessage(SceneObject::OBJECT_MESSAGE_PHYSICS, MESSAGE_FUNC(VehicleEngine::OnPhysicsMessage));
		SimEngine::GetPtr()->GetRuntimeController()->Register(UPDATE_FUNC(VehicleEngine::Update));
		m_Initialized = true;
	}

	void VehicleEngine::OnLoad(MessagePtr message)
	{
		//Get wheels from children
		SetWheels(m_WheelNames);

		//Play engine sound
		MessagePtr sound_msg(new Message(SceneObject::OBJECT_MESSAGE_SOUND_PARAMETER));
		sound_msg->SetData("Parameter",SceneObject::PLAY);
		GetSceneObject()->PostMessage(sound_msg);
	}

	void VehicleEngine::OnUnload(MessagePtr message)
	{
		

	}

	void VehicleEngine::OnPhysicsMessage(MessagePtr message)
	{
		Vec3 ang_vel  = boost::any_cast<Vec3>(message->GetData("AngularVelocity"));
		m_AngularVelocity = ang_vel;
		Vec3 velocity  = boost::any_cast<Vec3>(message->GetData("Velocity"));
		m_VehicleSpeed = velocity.FastLength();
	}

	void VehicleEngine::OnInput(MessagePtr message)
	{
		std::string name = boost::any_cast<std::string>(message->GetData("Controller"));
		float value = boost::any_cast<float>(message->GetData("Value"));

		if (name == "Throttle")
		{

			//let there be a dead span in input
			m_DesiredThrottle = value;
			if(fabs(m_DesiredThrottle) < 0.1) 
				m_DesiredThrottle = 0;
		}

		else if (name == "Steer")
		{
			m_DesiredSteer = value;
			//let there be a dead span in input
			m_DesiredSteer = value;
			if(fabs(m_DesiredSteer) < 0.1) 
				m_DesiredSteer = 0;
		}
	
	}


	void VehicleEngine::Update(double delta)
	{
		m_CurrentTime += delta;
		//m_Throttle =  DampThrottle(delta, m_DesiredThrottle,m_Throttle, m_ThrottleAccel);

		//Direct mapping, use the above function to damp input
		float throttle = m_DesiredThrottle;
		
		//select gear if in automatic otherwise only handle clutch timing
		UpdateGearShift(throttle, m_VehicleEngineRPM,m_CurrentTime);
		float brake_q = GetBreakTorq(throttle);
		//clamp throttle 0-1 and zero out if we want to break
		throttle = ClampThrottle(throttle);
		UpdateDriveTrain(delta,throttle, m_VehicleSpeed, brake_q);

		//clamp rpm
		if(m_VehicleEngineRPM > m_MaxRPM) m_VehicleEngineRPM = m_MaxRPM;
		if(m_VehicleEngineRPM < m_MinRPM) m_VehicleEngineRPM = m_MinRPM;
		
		//TODO: Simulate hydrostatic transmission instead
		if(m_EngineType == ET_TANK)
		{
			UpdateSteering(delta);
		}

		UpdateSound(delta);
		
		/*char dtxt[256];
		sprintf(dtxt,"Gear: %d Throttle %f RPM:%f Clutch:%f",m_Gear,throttle,m_VehicleEngineRPM,m_Clutch);
		std::string engine_data = dtxt;
		MessagePtr debug_msg(new Message(SimSystemManager::SYSTEM_MESSAGE_DEBUG_PRINT));
		debug_msg->SetData("Text",engine_data);
		SimEngine::Get().GetSystemManager()->SendImmediate(debug_msg);*/
	}
	void VehicleEngine::UpdateSound(double delta)
	{
		//Play engine sound
		float pitch = GetNormRPM() + 1.0;
		MessagePtr sound_msg(new Message(SceneObject::OBJECT_MESSAGE_SOUND_PARAMETER));
		sound_msg->SetData("Parameter",SceneObject::PITCH);
		sound_msg->SetData("Value",pitch);
		GetSceneObject()->PostMessage(sound_msg);

	}

	float VehicleEngine::GetNormRPM()
	{
		
		float ret = (m_RPM-m_MinRPM)/(m_MaxRPM-m_MinRPM);
		//float ret = (m_RPM-m_MinRPM)/(m_RPMGearChangeUp-m_MinRPM);
		if(ret < 0)  ret = 0;
		return ret;
	}

	void VehicleEngine::UpdateSteering(double delta)
	{
		//if tank, try to keep angular velocity to steer factor
		m_SteerCtrl.setGain(m_TurnForce,0.1,0);
		m_SteerCtrl.set(-m_DesiredSteer);
		//limit pid to max turn force
		m_SteerCtrl.setOutputLimit(m_MaxTurnForce);
		float turn_torque = m_SteerCtrl.update(m_AngularVelocity.y,delta);
		
		MessagePtr force_msg(new Message(SceneObject::OBJECT_MESSAGE_PHYSICS_BODY_PARAMETER));
		force_msg->SetData("Parameter",SceneObject::TORQUE);
		force_msg->SetData("Value",Vec3(0,turn_torque,0));
		GetSceneObject()->PostMessage(force_msg);
		/*char dtxt[256];
		sprintf(dtxt,"Speed(km/h): %f\n Hull ang vel:%f %f %f\n Q:%f \n DesiredSteer vel %f",m_VehicleSpeed*3.6f, m_AngularVelocity.x,m_AngularVelocity.y,m_AngularVelocity.z,turn_torque,-m_DesiredSteer);
		std::string engine_data = dtxt;
		MessagePtr debug_msg(new Message(SimSystemManager::SYSTEM_MESSAGE_DEBUG_PRINT));
		debug_msg->SetData("Text",engine_data);
		SimEngine::Get().GetSystemManager()->SendImmediate(debug_msg);*/
	}

	float VehicleEngine::DampThrottle(float delta, float desired_throttle,float current_throttle, float throttle_accel)
	{
		//damp throttle to avoid to full throttle to quick
		if(current_throttle < desired_throttle)
		{
			current_throttle = current_throttle + throttle_accel*delta;
			if(current_throttle > desired_throttle) current_throttle = desired_throttle;
		}
		else if(current_throttle > desired_throttle)
		{
			current_throttle = current_throttle - throttle_accel*delta;
			if(current_throttle < desired_throttle) current_throttle = desired_throttle;
		}
		return current_throttle;
	}

	float VehicleEngine::ClampThrottle(float throttle)
	{
		float current_gear_ratio =  m_GearBoxRatio[m_Gear];
		if(throttle < 0 && current_gear_ratio > 0) //we are moving foward and want to brake, set throttle to zero
		{
			throttle = 0;
		}
		else if(throttle > 0 && current_gear_ratio < 0) //we are moving backward and want to brake, set throttle to zero
		{
			throttle = 0;
		}

		if(m_AutoShiftStart)
		{
			throttle = m_Clutch*throttle;
		}
		return fabs(throttle);
	}


	float VehicleEngine::GetBreakTorq(float throttle)
	{
		float brake_torque = 0;
		float current_gear_ratio =  m_GearBoxRatio[m_Gear];
		//Park brake
		if(current_gear_ratio == 0)
		{
			brake_torque = m_MaxBrakeTorque;
		}
		
		//brake
		if(throttle < 0 && current_gear_ratio > 0)
		{
			brake_torque = m_MaxBrakeTorque* fabs(throttle);
			
		}
		else if(throttle > 0 && current_gear_ratio < 0)
		{
			brake_torque = m_MaxBrakeTorque* fabs(throttle);
		
		}
		return brake_torque;
	}

	float VehicleEngine::GetDesiredWheelVelocity(float throttle)
	{
		throttle = fabs(throttle);
		float current_gear_ratio =  m_GearBoxRatio[m_Gear];
		float wheel_vel;
		if(current_gear_ratio == 0)
		{
			wheel_vel = 0;
		}
		else wheel_vel = RPM2AngleVel(m_MaxRPM/current_gear_ratio)*fabs(throttle);
		return wheel_vel;
	}

	float VehicleEngine::GetWheelTorqFromEngine()
	{
		
		float current_gear_ratio =  m_GearBoxRatio[m_Gear];

		//max power at half of max_rpm 
		float angle= (m_VehicleEngineRPM / m_MaxRPM)*MY_PI;
		float current_engine_power = sin(angle)*m_Power;
		
		float wheel_torque;
		wheel_torque = (current_engine_power)*fabs(current_gear_ratio)*m_Clutch;
		
		return wheel_torque;

	}

	void VehicleEngine::UpdateDriveTrain(double delta,float throttle, float speed, float brake_torque)
	{
		float wheel_torque = GetWheelTorqFromEngine();
		float wheel_vel =  GetDesiredWheelVelocity(throttle);
		int num_wheels = 0;

		/*char dtxt[256];
		sprintf(dtxt,"Wheel q: %f v:%f",wheel_torque,wheel_vel);
	
		std::string engine_data = dtxt;
		MessagePtr debug_msg(new Message(SimSystemManager::SYSTEM_MESSAGE_DEBUG_PRINT));
		debug_msg->SetData("Text",engine_data);
		SimEngine::Get().GetSystemManager()->SendImmediate(debug_msg);*/

		//send physics data to wheels and update mean wheel rpm
		MessagePtr force_msg(new Message(SceneObject::OBJECT_MESSAGE_PHYSICS_JOINT_PARAMETER));
		force_msg->SetData("Parameter",SceneObject::AXIS2_FORCE);
		force_msg->SetData("Value",wheel_torque+brake_torque);

		MessagePtr vel_msg(new Message(SceneObject::OBJECT_MESSAGE_PHYSICS_JOINT_PARAMETER));
			
		vel_msg->SetData("Parameter",SceneObject::AXIS2_VELOCITY);
		vel_msg->SetData("Value",wheel_vel);
	
		m_WheelRPM = 0;
		for(int i = 0; i < m_Wheels.size(); i++)
		{
			VehicleWheelPtr wheel = m_Wheels[i];
			SceneObjectPtr wheel_obj(wheel->m_WheelObject);
			wheel_obj->PostMessage(force_msg);
			wheel_obj->PostMessage(vel_msg);

			m_WheelRPM += AngleVel2RPM(wheel->m_AngularVelocity);
			num_wheels++;
		}
		if(num_wheels > 0)
			m_WheelRPM = m_WheelRPM/float(num_wheels);

		float current_gear_ratio =  m_GearBoxRatio[m_Gear];
		//give feedback to engine, when clutch down throttle is directly mapped to rpm, this is not correct and alternatives are to be tested
		//this could be ok if we ha separate clutch input
		//m_VehicleEngineRPM = fabs(m_WheelRPM*current_gear_ratio*m_Clutch) + (1-m_Clutch)*m_MaxRPM*fabs(throttle);
		
		//give feedback to engine, when clutch down engine rmp is decreased by 1000rpm each second...throttle is released during shifting
		m_VehicleEngineRPM = fabs(m_WheelRPM*current_gear_ratio*m_Clutch) + (1.0-m_Clutch)*(m_VehicleEngineRPM-1000*delta);

	
		/*sprintf(dtxt,"Wheel vel: %f",m_WheelRPM);
		engine_data = dtxt;
		MessagePtr debug_msg2(new Message(SimSystemManager::SYSTEM_MESSAGE_DEBUG_PRINT));
		debug_msg2->SetData("Text",engine_data);
		SimEngine::Get().GetSystemManager()->SendImmediate(debug_msg2);*/

		//what rpm should we expose to other components?
		if(m_FakeRPMOutput)
		{
			m_RPM = fabs(speed*current_gear_ratio*20);
		}
		else
		{
			m_RPM = m_VehicleEngineRPM;
		}
	}

	void VehicleEngine::UpdateGearShift(float throttle, float rpm, double time)
	{
		int number_of_gears = m_GearBoxRatio.size();
		if(m_AutoClutchStart == 0 && m_AutoShiftStart == 0)
		{
			if(m_Automatic)
			{
				
				if(m_Gear == m_NeutralGear)
				{
					if(throttle > 0.03 && m_Gear+1 < number_of_gears)
					{
						m_AutoShiftStart = time-m_ClutchTimeChangeGear;
						m_FutureGear = m_Gear+1;
					}
					else if(throttle < -0.03 && m_Gear-1 >= 0)
					{
						m_AutoShiftStart = time-m_ClutchTimeChangeGear;
						m_FutureGear = m_Gear-1;
					}
				}
				
				else if(m_Gear > m_NeutralGear) //forward gear
				{
					if(rpm > m_RPMGearChangeUp && m_Gear+1 < number_of_gears) 
					{
						m_AutoShiftStart = time;
						m_FutureGear = m_Gear + 1;
					}
					else if(rpm < m_RPMGearChangeDown)
					{
						m_AutoShiftStart = time;
						m_FutureGear = m_Gear - 1;
					}
				}
				else //reverse gear
				{
					if(rpm > m_RPMGearChangeUp && m_Gear-1 >= 0) 
					{
						m_AutoShiftStart = time;
						m_FutureGear = m_Gear - 1;
					}
					else if(rpm < m_RPMGearChangeDown)
					{
						m_AutoShiftStart = time;
						m_FutureGear = m_Gear + 1;
					}
				}
			}
			else
			{
				if(m_ShiftUp && m_Gear < number_of_gears-1)
				{
					m_AutoShiftStart = time;
					m_FutureGear = m_Gear + 1;
				}
				else if(m_ShiftDown && m_Gear > 1)
				{
					m_AutoShiftStart = time;
					m_FutureGear = m_Gear - 1;
				}
			}
		}

		float desired_clutch = 0;
		if(m_AutoShiftStart)
		{
			float t = time-m_AutoShiftStart;
			// We are in a shifting operation
			if(m_Gear != m_FutureGear)
			{
				// We are in the pre-shift phase; apply the clutch
				if(t >= m_DeclutchTimeChangeGear)
				{
					// Declutch is ready, change gear
					m_Gear = m_FutureGear;
					// Trigger gear shift sample
					desired_clutch=0.0f;
				} 
				else
				{
					// Change clutch over time (depress pedal means clutch goes from 1->0

					desired_clutch=1.0f-(t/m_DeclutchTimeChangeGear);
				}
			}
			else
			{
				if(m_Gear == m_NeutralGear)
				{
					desired_clutch = 0;
					m_AutoShiftStart=0;
				}
				else
				{
					// We are in the post-shift phase
					if(t >= m_ClutchTimeChangeGear+m_DeclutchTimeChangeGear)
					{
						m_AutoShiftStart=0;
						desired_clutch=1.0f;
					}
					else
					{
						// Change clutch
						desired_clutch=((t-m_DeclutchTimeChangeGear)/m_ClutchTimeChangeGear);
					}
				}
			}
			if(desired_clutch>1)
			{
				desired_clutch=1;
			}
			m_Clutch = desired_clutch;
		}
	}

	float VehicleEngine::RPM2AngleVel(float rpm)
	{
		// Convert to radians and seconds
		return rpm*2*MY_PI/60.0f;
	}

	float VehicleEngine::AngleVel2RPM(float rps)
	{
		//Convert to radians and minut
		return rps*60.0f/(2*MY_PI);
	}
}
