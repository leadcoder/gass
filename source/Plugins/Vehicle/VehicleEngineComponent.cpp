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

#include "VehicleEngineComponent.h"

#include <memory>
#include "Core/Math/GASSMath.h"
#include "Sim/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/Interface/GASSIMissionSceneManager.h"
#include "Sim/Interface/GASSIPhysicsSuspensionComponent.h"
#include "Sim/Interface/GASSIPhysicsBodyComponent.h"
#include "Sim/Interface/GASSISoundComponent.h"
#include "Sim/Messages/GASSPlatformMessages.h"

namespace GASS
{
	VehicleWheel::VehicleWheel(SceneObjectPtr  wheel) : m_Velocity(0),
		m_AngularVelocity (0),
		m_WheelObject(wheel)
	{

	}

	void VehicleWheel::Init()
	{
		SceneObjectPtr wheel_obj = m_WheelObject.lock();
		if(wheel_obj)
		{
			wheel_obj->RegisterForMessage(REG_TMESS(VehicleWheel::OnPhysicsMessage,PhysicsVelocityEvent,0));
		}
	}

	VehicleWheel::~VehicleWheel()
	{
		//SceneObjectPtr wheel_obj(m_WheelObject,NO_THROW);
		//if(wheel_obj)
		//	wheel_obj->UnregisterForMessage(UNREG_TMESS(VehicleWheel::OnPhysicsMessage,PhysicsVelocityEvent));
	}

	void VehicleWheel::OnPhysicsMessage(PhysicsVelocityEventPtr message)
	{
		//fetch wheel rpm
		Vec3 vel  = message->GetLinearVelocity();
		m_Velocity = static_cast<float>(vel.x);
		Vec3 ang_vel  = message->GetAngularVelocity();
		m_AngularVelocity = static_cast<float>(ang_vel.x);
	//	std::cout << "anglvel:" << ang_vel.x << " " << ang_vel.y << " " << ang_vel.z << std::endl;
	}

	VehicleEngineComponent::VehicleEngineComponent() :
		m_InputToThrottle("Throttle"),
		m_InputToSteer("Steer"),
		
		m_AngularVelocity(0,0,0)
		
	{
		m_SteerCtrl = PIDControl(100,1,1);
		m_GearBoxRatio.resize(6);
		m_GearBoxRatio[0] = -16.42f;
		m_GearBoxRatio[1] = 0;
		m_GearBoxRatio[2] = 16.42f;
		m_GearBoxRatio[3] = 12.15f;
		m_GearBoxRatio[4] = 11.52f;
		m_GearBoxRatio[5] = 11.17f;
		//m_GearBoxRatio[6] = 0.71;
	}

	VehicleEngineComponent::~VehicleEngineComponent()
	{

	}

	void VehicleEngineComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register<VehicleEngineComponent>();
		GetClassRTTI()->SetMetaData(std::make_shared<ClassMetaData>("VehicleEngineComponent", OF_VISIBLE));

		RegisterGetSet("Wheels", &VehicleEngineComponent::GetWheels, &VehicleEngineComponent::SetWheels);

		RegisterGetSet("EngineType", &VehicleEngineComponent::GetEngineType, &VehicleEngineComponent::SetEngineType,PF_VISIBLE,"Engine type (tank or car?)");
		RegisterGetSet("Automatic", &VehicleEngineComponent::GetAutomatic, &VehicleEngineComponent::SetAutomatic,PF_VISIBLE | PF_EDITABLE,"Use automatic gearbox");
		RegisterGetSet("InvertDrivetrainOutput", &VehicleEngineComponent::GetInvert, &VehicleEngineComponent::SetInvert,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("BrakeTorque", &VehicleEngineComponent::GetBrakeTorque, &VehicleEngineComponent::SetBrakeTorque,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("ConstantTorque", &VehicleEngineComponent::GetConstantTorque, &VehicleEngineComponent::SetConstantTorque,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("DeclutchTimeChangeGear", &VehicleEngineComponent::GetDeclutchTimeChangeGear, &VehicleEngineComponent::SetDeclutchTimeChangeGear,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("ClutchTimeChangeGear", &VehicleEngineComponent::GetClutchTimeChangeGear, &VehicleEngineComponent::SetClutchTimeChangeGear,PF_VISIBLE | PF_EDITABLE,"");

		RegisterGetSet("MaxRPM", &VehicleEngineComponent::GetMaxRPM, &VehicleEngineComponent::SetMaxRPM,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("MinRPM", &VehicleEngineComponent::GetMinRPM, &VehicleEngineComponent::SetMinRPM,PF_VISIBLE | PF_EDITABLE,"");

		RegisterGetSet("RPMGearChangeUp", &VehicleEngineComponent::GetRPMGearChangeUp, &VehicleEngineComponent::SetRPMGearChangeUp,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("RPMGearChangeDown", &VehicleEngineComponent::GetRPMGearChangeDown, &VehicleEngineComponent::SetRPMGearChangeDown,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("Power", &VehicleEngineComponent::GetPower, &VehicleEngineComponent::SetPower,PF_VISIBLE | PF_EDITABLE,"");

		RegisterGetSet("SteerPID", &VehicleEngineComponent::GetSteerPID, &VehicleEngineComponent::SetSteerPID,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("GearRatio", &VehicleEngineComponent::GetGearRatio, &VehicleEngineComponent::SetGearRatio,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("SmoothRPMOutput", &VehicleEngineComponent::GetSmoothRPMOutput, &VehicleEngineComponent::SetSmoothRPMOutput,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("Debug", &VehicleEngineComponent::GetDebug, &VehicleEngineComponent::SetDebug,PF_VISIBLE | PF_EDITABLE,"");
		RegisterMember("TurnRPMAmount", &VehicleEngineComponent::m_TurnRPMAmount,PF_VISIBLE | PF_EDITABLE,"");
		RegisterGetSet("MaxTurnVel", &VehicleEngineComponent::GetMaxTurnVel, &VehicleEngineComponent::SetMaxTurnVel,PF_VISIBLE | PF_EDITABLE,"");
	}

	void VehicleEngineComponent::OnInitialize()
	{
		Component::OnInitialize();
		GetSceneObject()->RegisterForMessage(REG_TMESS(VehicleEngineComponent::OnInput,InputRelayEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(VehicleEngineComponent::OnPhysicsMessage,PhysicsVelocityEvent,0));

		RegisterForPostUpdate<IMissionSceneManager>();

		m_Initialized = true;

		//Get wheels from children
		SetWheels(m_WheelObjects);

		//Play engine sound
		auto sound = GetSceneObject()->GetFirstComponentByClass<ISoundComponent>();
		if (sound)
		{
			sound->SetPlay(true);
		}
	}


	void VehicleEngineComponent::OnDelete()
	{

	}

	PIDControl VehicleEngineComponent::GetSteerPID() const
	{
		return m_SteerCtrl;
	}

	void VehicleEngineComponent::SetSteerPID(const PIDControl &pid)
	{
		m_SteerCtrl = pid;
	}

	void VehicleEngineComponent::SetEngineType(const std::string &type)
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
	std::string VehicleEngineComponent::GetEngineType()const
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

	bool VehicleEngineComponent::GetAutomatic() const
	{
		return m_Automatic;
	}

	void VehicleEngineComponent::SetAutomatic(bool value)
	{
		m_Automatic = value;
	}

	float VehicleEngineComponent::GetBrakeTorque() const
	{
		return m_MaxBrakeTorque;
	}

	void VehicleEngineComponent::SetBrakeTorque(float value)
	{
		m_MaxBrakeTorque = value;
	}

	float VehicleEngineComponent::GetDeclutchTimeChangeGear() const
	{
		return m_DeclutchTimeChangeGear;
	}

	void VehicleEngineComponent::SetDeclutchTimeChangeGear(float value)
	{
		m_DeclutchTimeChangeGear = value;
	}

	float VehicleEngineComponent::GetClutchTimeChangeGear() const
	{
		return m_ClutchTimeChangeGear;
	}

	void VehicleEngineComponent::SetClutchTimeChangeGear(float value)
	{
		m_ClutchTimeChangeGear = value;
	}

	float VehicleEngineComponent::GetMaxRPM() const
	{
		return m_MaxRPM;
	}

	void VehicleEngineComponent::SetMaxRPM(float value)
	{
		m_MaxRPM = value;
	}

	float VehicleEngineComponent::GetMinRPM() const
	{
		return m_MinRPM;
	}

	void VehicleEngineComponent::SetMinRPM(float value)
	{
		m_MinRPM = value;
	}


	float VehicleEngineComponent::GetRPMGearChangeUp() const
	{
		return m_RPMGearChangeUp;
	}

	void VehicleEngineComponent::SetRPMGearChangeUp(float value)
	{
		m_RPMGearChangeUp = value;
	}

	float VehicleEngineComponent::GetRPMGearChangeDown() const
	{
		return m_RPMGearChangeDown;
	}

	void VehicleEngineComponent::SetRPMGearChangeDown(float value)
	{
		m_RPMGearChangeDown = value;
	}



	float VehicleEngineComponent::GetPower() const
	{
		return m_Power;
	}

	void VehicleEngineComponent::SetPower(float value)
	{
		m_Power = value;
	}


	void VehicleEngineComponent::SetGearRatio(const std::vector<float> &gear_data)
	{
		m_GearBoxRatio = gear_data;

		//find neutral
		for(size_t i = 0; i < m_GearBoxRatio.size(); i++)
		{
			if(m_GearBoxRatio[i] == 0.0)
			{
				m_NeutralGear = static_cast<int>(i);
			}
		}
		m_Gear = m_NeutralGear;
	}

	float VehicleEngineComponent::GetConstantTorque() const
	{
		return m_ConstantTorque;
	}

	void VehicleEngineComponent::SetConstantTorque(float value)
	{
		m_ConstantTorque = value;

	}

	std::vector<float> VehicleEngineComponent::GetGearRatio() const
	{
		return m_GearBoxRatio;
	}

	std::vector<SceneObjectRef> VehicleEngineComponent::GetWheels() const
	{
		return m_WheelObjects;
	}

	void VehicleEngineComponent::SetWheels(const std::vector<SceneObjectRef> &wheels)
	{
		m_WheelObjects = wheels;
		//Update wheel list
		if(m_Initialized)
		{
			m_VehicleWheels.clear();
			for(size_t i = 0; i < m_WheelObjects.size(); i++)
			{
				VehicleWheelPtr wheel(new VehicleWheel(m_WheelObjects[i].GetRefObject()));
				wheel->Init();
				m_VehicleWheels.push_back(wheel);
			}
		}
	}


	void VehicleEngineComponent::OnPhysicsMessage(PhysicsVelocityEventPtr message)
	{
		Vec3 ang_vel  = message->GetAngularVelocity();
		m_AngularVelocity = ang_vel;
		Vec3 velocity  = message->GetLinearVelocity();
		m_VehicleSpeed = static_cast<float>(velocity.Length());
	}

	void VehicleEngineComponent::OnInput(InputRelayEventPtr message)
	{
		std::string name = message->GetController();
		float value = message->GetValue();

		if (name == m_InputToThrottle)
		{

			//let there be a dead span in input
			m_DesiredThrottle = value;
			//if(fabs(m_DesiredThrottle) < 0.1)
			//m_DesiredThrottle = 0;
		}
		else if (name == m_InputToSteer)
		{
			//let there be a dead span in input
			m_DesiredSteer = value;
			//if(fabs(m_DesiredSteer) < 0.1)
			//	m_DesiredSteer = 0;
		}
	}

	void VehicleEngineComponent::SceneManagerTick(double delta)
	{
		m_CurrentTime += delta;
		//m_Throttle =  DampThrottle(delta, m_DesiredThrottle,m_Throttle, m_ThrottleAccel);

		//fade out in case we don't get any input from joystick
		if(fabs(m_DesiredThrottle) < 0.1)
			m_DesiredThrottle = m_DesiredThrottle*0.9f;

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
		UpdateExhaustFumes(delta);
		UpdateInstruments(delta);

		if(m_Debug)
		{
			std::stringstream debug_ss;
			debug_ss << "Speed(m/s): "<< m_VehicleSpeed << "\n";
			debug_ss << "Gear: "<< m_Gear<< "\n";
			debug_ss << "Throttle: "<< throttle<< "\n";
			debug_ss << "RPM: "<< m_VehicleEngineRPM << "\n";
			debug_ss << "Clutch: "<< m_Clutch << "\n";
			const std::string engine_data = debug_ss.str();
			GASS_PRINT(engine_data)
		}
	}
	void VehicleEngineComponent::UpdateSound(double /*delta*/)
	{
		//Play engine sound
		float pitch = GetNormRPM() + 1.0f;
		auto sound = GetSceneObject()->GetFirstComponentByClass<ISoundComponent>();
		if (sound)
		{
			sound->SetPitch(pitch);
		}
	}

	void VehicleEngineComponent::UpdateExhaustFumes(double /*delta*/)
	{
		//float emission = GetNormRPM()*30;
		//GetSceneObject()->PostRequest(std::make_shared<ParticleSystemParameterRequest>(ParticleSystemParameterRequest::EMISSION_RATE,0,emission));
	}

	void VehicleEngineComponent::UpdateInstruments(double /*delta*/)
	{
		GetSceneObject()->PostEvent(std::make_shared<VehicleEngineStatusMessage>(m_RPM,m_VehicleSpeed,m_Gear));
	}

	float VehicleEngineComponent::GetNormRPM() const
	{

		float ret = (m_RPM-m_MinRPM)/(m_MaxRPM-m_MinRPM);
		//float ret = (m_RPM-m_MinRPM)/(m_RPMGearChangeUp-m_MinRPM);
		if(ret < 0)  ret = 0;
		return ret;
	}

	void VehicleEngineComponent::UpdateSteering(double delta)
	{
		float norm_rpm = m_TurnRPMAmount*GetNormRPM() + (1-m_TurnRPMAmount);
		//if tank, try to keep angular velocity to steer factor
		//m_SteerCtrl.setGain(1000,1,1);

		m_SteerCtrl.Set(-m_DesiredSteer*m_MaxTurnVel);
		//limit pid to max turn force
		m_SteerCtrl.SetOutputLimit(m_MaxTurnForce*norm_rpm);
		auto turn_torque = static_cast<float>(m_SteerCtrl.Update(m_AngularVelocity.y, delta));

		//damp
		//if(fabs(m_DesiredSteer) < 0.1)
		//	m_DesiredSteer = m_DesiredSteer*0.9;
		GetSceneObject()->GetFirstComponentByClass<IPhysicsBodyComponent>()->AddTorque(Vec3(0,turn_torque,0),true);

		/*
		GASS_PRINT("Speed(m/s): "<< m_VehicleSpeed << "Hull angle velocity:" << m_AngularVelocity.y << "\n Desired steer velocity" << m_DesiredSteer << "\n Torq" << turn_torque << " Norm rpm:" << norm_rpm << " MaxTorq:" << m_MaxTurnForce)
		*/

		/*if(m_Debug)
		{
			GASS_PRINT("Speed(m/s): "<< m_VehicleSpeed << "Hull angle velocity:" << m_AngularVelocity << "\n Desired steer velocity" << m_DesiredSteer)
		}*/
	}

	float VehicleEngineComponent::DampThrottle(float delta, float desired_throttle,float current_throttle, float throttle_accel)
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

	float VehicleEngineComponent::ClampThrottle(float throttle)
	{
		float current_gear_ratio =  m_GearBoxRatio[m_Gear];
		if(throttle < 0 && current_gear_ratio > 0) //we are moving foward and want to brake, set throttle to zero
		{
			//m_Clutch = 1.0 - fabs(throttle); //also, use clutch when braking
			throttle = 0;

		}
		else if(throttle > 0 && current_gear_ratio < 0) //we are moving backward and want to brake, set throttle to zero
		{
			//m_Clutch = 1.0 - throttle; //also, use clutch when braking
			throttle = 0;

		}

		if(m_AutoShiftStart)
		{
			throttle = m_Clutch*throttle;
		}
		return fabs(throttle);
	}


	float VehicleEngineComponent::GetBreakTorq(float throttle)
	{
		float brake_torque = m_ConstantTorque;
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

	float VehicleEngineComponent::GetDesiredWheelVelocity(float throttle)
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

	float VehicleEngineComponent::GetWheelTorqFromEngine()
	{

		float current_gear_ratio =  m_GearBoxRatio[m_Gear];

		//max power at half of max_rpm
		float angle= (m_VehicleEngineRPM / m_MaxRPM)*static_cast<float>(GASS_PI);
		float current_engine_power = sin(angle)*m_Power;

		float wheel_torque;
		wheel_torque = (current_engine_power)*fabs(current_gear_ratio)*m_Clutch;

		return wheel_torque;

	}

	void VehicleEngineComponent::UpdateDriveTrain(double delta,float throttle, float speed, float brake_torque)
	{
		float wheel_torque = GetWheelTorqFromEngine();
		float wheel_vel =  GetDesiredWheelVelocity(throttle);
		int num_wheels = 0;

		//send physics data to wheels and update mean wheel rpm
		if(m_Invert)
			wheel_vel = -wheel_vel;

		m_WheelRPM = 0;
		for(size_t i = 0; i < m_VehicleWheels.size(); i++)
		{
			VehicleWheelPtr wheel = m_VehicleWheels[i];
			SceneObjectPtr wheel_obj = wheel->m_WheelObject.lock();

			if(wheel_obj)
			{
				PhysicsSuspensionComponentPtr suspension = wheel_obj->GetFirstComponentByClass<IPhysicsSuspensionComponent>();
				suspension->SetMaxDriveTorque(wheel_torque + brake_torque);
				suspension->SetDriveVelocity(wheel_vel);
				m_WheelRPM += AngleVel2RPM(wheel->m_AngularVelocity);
				num_wheels++;
			}
		}
		if(num_wheels > 0)
			m_WheelRPM = m_WheelRPM/float(num_wheels);

		if(m_Invert)
			m_WheelRPM = -m_WheelRPM;


		float current_gear_ratio =  m_GearBoxRatio[m_Gear];
		//give feedback to engine, when clutch down throttle is directly mapped to rpm, this is not correct and alternatives are to be tested
		//this could be ok if we ha separate clutch input
		//m_VehicleEngineRPM = fabs(m_WheelRPM*current_gear_ratio*m_Clutch) + (1-m_Clutch)*m_MaxRPM*fabs(throttle);

		//give feedback to engine, when clutch down engine rmp is decreased by 1000rpm each second...throttle is released during shifting
		m_VehicleEngineRPM = fabs(m_WheelRPM*current_gear_ratio*m_Clutch) + (1.0f - m_Clutch) * (m_VehicleEngineRPM-1000 * static_cast<float>(delta));

		//what rpm should we expose to other components?
		if(m_SmoothRPMOutput)
		{
			m_RPM = fabs(speed*current_gear_ratio*20);
		}
		else
		{
			m_RPM = m_VehicleEngineRPM;
		}
	}

	void VehicleEngineComponent::UpdateGearShift(float throttle, float rpm, double time)
	{
		int number_of_gears = static_cast<int>(m_GearBoxRatio.size());
		if(m_AutoClutchStart == 0 && m_AutoShiftStart == 0)
		{
			if(m_Automatic)
			{

				if(m_Gear == m_NeutralGear)
				{
					if(throttle > 0.03 && m_Gear+1 < number_of_gears)
					{
						m_AutoShiftStart = static_cast<float>(time) - m_ClutchTimeChangeGear;
						m_FutureGear = m_Gear+1;
					}
					else if(throttle < -0.03 && m_Gear-1 >= 0)
					{
						m_AutoShiftStart = static_cast<float>(time) - m_ClutchTimeChangeGear;
						m_FutureGear = m_Gear-1;
					}
				}

				else if(m_Gear > m_NeutralGear) //forward gear
				{
					if(rpm > m_RPMGearChangeUp && m_Gear+1 < number_of_gears)
					{
						m_AutoShiftStart = static_cast<float>(time);
						m_FutureGear = m_Gear + 1;
					}
					else if(rpm < m_RPMGearChangeDown && throttle < 0.01)
					{
						m_AutoShiftStart = static_cast<float>(time);
						m_FutureGear = m_Gear - 1;
					}
				}
				else //reverse gear
				{
					if(rpm > m_RPMGearChangeUp && m_Gear-1 >= 0)
					{
						m_AutoShiftStart = static_cast<float>(time);
						m_FutureGear = m_Gear - 1;
					}
					else if(rpm < m_RPMGearChangeDown)
					{
						m_AutoShiftStart = static_cast<float>(time);
						m_FutureGear = m_Gear + 1;
					}
				}
			}
			else
			{
				if(m_ShiftUp && m_Gear < number_of_gears-1)
				{
					m_AutoShiftStart = static_cast<float>(time);
					m_FutureGear = m_Gear + 1;
				}
				else if(m_ShiftDown && m_Gear > 1)
				{
					m_AutoShiftStart = static_cast<float>(time);
					m_FutureGear = m_Gear - 1;
				}
			}
		}

		float desired_clutch = 0;
		if(m_AutoShiftStart)
		{
			float t = static_cast<float>(time) - m_AutoShiftStart;
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

	float VehicleEngineComponent::RPM2AngleVel(float rpm)
	{
		// Convert to radians and seconds
		return rpm*2* static_cast<float>(GASS_PI)/60.0f;
	}

	float VehicleEngineComponent::AngleVel2RPM(float rps)
	{
		//Convert to radians and minutes
		return rps*60.0f/(2* static_cast<float>(GASS_PI));
	}


	void VehicleEngineComponent::SetSmoothRPMOutput(const bool &value)
	{
		m_SmoothRPMOutput = value;
	}
	bool VehicleEngineComponent::GetSmoothRPMOutput() const
	{
		return m_SmoothRPMOutput;
	}
}
