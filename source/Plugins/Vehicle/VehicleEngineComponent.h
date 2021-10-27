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

#ifndef VEHICLE_ENGINE_COMPONENT_H
#define VEHICLE_ENGINE_COMPONENT_H

#include "Sim/GASSCommon.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/GASSSceneObjectRef.h"
#include "Sim/GASSComponent.h"
#include "Sim/Messages/GASSInputMessages.h"
#include "Core/Utils/GASSPIDControl.h"

namespace GASS
{


	class SceneObject;
	using SceneObjectPtr = std::shared_ptr<SceneObject>;
	using SceneObjectWeakPtr = std::weak_ptr<SceneObject>;

	class VehicleWheel :  public GASS_ENABLE_SHARED_FROM_THIS<VehicleWheel> ,public IMessageListener
	{
	public:
		VehicleWheel(SceneObjectPtr  wheel);
		~VehicleWheel() override;
		void Init();
		void OnPhysicsMessage(PhysicsVelocityEventPtr message);

		//private: //public for fast access
		SceneObjectWeakPtr m_WheelObject;
		float m_Velocity;
		float m_AngularVelocity;
	};
	using VehicleWheelPtr = std::shared_ptr<VehicleWheel>;

	enum EngineType
	{
		ET_CAR,
		ET_TANK,
	};

	class VehicleEngineComponent :  public Reflection<VehicleEngineComponent,Component>
	{
	public:
		VehicleEngineComponent();
		~VehicleEngineComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;
		void SceneManagerTick(double delta) override;
		float GetRPM() const {return m_RPM;}
		int GetGear() const {return m_Gear;}
		float GetNormRPM() const;
	private:
		std::vector<SceneObjectRef> GetWheels() const;
		void SetWheels(const std::vector<SceneObjectRef> &wheels);
		
		void OnPhysicsMessage(PhysicsVelocityEventPtr message);
		void OnInput(InputRelayEventPtr message);


		//set/get attributes
		void SetEngineType(const std::string &type);
		std::string GetEngineType()const;
		bool GetAutomatic() const;
		void SetAutomatic(bool value);
		float GetBrakeTorque() const;
		void SetBrakeTorque(float value);
		float GetConstantTorque() const;
		void SetConstantTorque(float value);
		
		
		float GetDeclutchTimeChangeGear() const;
		void SetDeclutchTimeChangeGear(float value);
		float GetClutchTimeChangeGear() const;
		void SetClutchTimeChangeGear(float value);
		float GetMaxRPM() const;
		void SetMaxRPM(float value);
		float GetMinRPM() const;
		void SetMinRPM(float value);
		float GetRPMGearChangeUp() const;
		void SetRPMGearChangeUp(float value);
		float GetRPMGearChangeDown() const;
		void SetRPMGearChangeDown(float value);
		float GetPower() const;
		void SetPower(float value);
		//float GetTurnForce() const;
		//void SetTurnForce(float value);
		PIDControl GetSteerPID() const;
		void SetSteerPID(const PIDControl &pid);

		void SetGearRatio(const std::vector<float> &gear_data);
		std::vector<float> GetGearRatio() const;
		void SetSmoothRPMOutput(const bool &value);
		bool GetSmoothRPMOutput() const;
		bool GetDebug() const {return m_Debug;}
		void SetDebug(bool value) {m_Debug =value;}
		
		

		//Update clutch and gear selection
		void UpdateGearShift(float throttle, float rpm, double time);

		//used to slow down throttle input
		float DampThrottle(float delta, float desired_throttle,float current_throttle, float throttle_accel);

		float ClampThrottle(float throttle);
		float GetWheelTorqFromEngine();
		float GetDesiredWheelVelocity(float throttle);
		float GetBreakTorq(float throttle);

		void UpdateDriveTrain(double delta,float throttle, float speed, float brake_torque);
		void UpdateSteering(double delta);
		void UpdateSound(double delta);
		void UpdateExhaustFumes(double delta);
		void UpdateInstruments(double delta);

		//Helpers
		float RPM2AngleVel(float rpm);
		float AngleVel2RPM(float rps);

		void SetInvert(bool value) {m_Invert = value;}
		bool GetInvert() const {return m_Invert;}

		float GetMaxTurnVel()const {return m_MaxTurnVel;}
		void SetMaxTurnVel(float value){m_MaxTurnVel =value;}

		//members attributes
		std::string m_VehicleEngineComponentType;
		float m_Power{0.2f};
		bool m_SmoothRPMOutput{true};
		float m_MaxBrakeTorque{1000};
		float m_ConstantTorque{0};
		float m_MinRPM{500};
		float m_MaxRPM{4000};
		float m_WheelRPM{0};
		float m_VehicleEngineRPM{0};
		float m_ThrottleAccel{2};
		
		float m_RPM{0};
		std::string m_InputToThrottle;
		std::string m_InputToSteer;
		bool m_Invert{false};
		float m_VehicleSpeed{0};

		//input holders
		int m_ShiftDown{0};
		int m_ShiftUp{0};
		float m_DesiredThrottle{0};
		float m_DesiredSteer{0};

		//Auto gear shift settings
		float m_ClutchTimeChangeGear{0.5f};
		float m_DeclutchTimeChangeGear{0.5f};
		float m_RPMGearChangeUp{1500};
		float m_RPMGearChangeDown{700};
		bool m_Automatic{1};
		float m_Clutch{1};

		//Auto gear shift helpers
		float m_AutoClutchStart{0};
		float m_AutoShiftStart{0};
		int m_FutureGear{0};

		//gear box
		std::vector<float> m_GearBoxRatio;
		int m_NeutralGear{1};
		//float m_GearBoxSpeed[MAX_GEARS];
		int m_Gear{2}; //current gear
		double m_CurrentTime{0};

		//Steering vars
		//float m_TurnForce;
		float m_MaxTurnForce{200};
		float m_MaxTurnVel{1.0f};
		float m_TurnRPMAmount{1.0f};

		std::vector<SceneObjectRef> m_WheelObjects;
		std::vector<VehicleWheelPtr> m_VehicleWheels;
		Vec3 m_AngularVelocity;
		PIDControl m_SteerCtrl;
		bool m_Initialized{false};
		bool m_Debug{false};
		EngineType m_EngineType{ET_TANK};

	};
}
#endif
