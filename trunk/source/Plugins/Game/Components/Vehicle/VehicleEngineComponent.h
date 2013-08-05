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

#ifndef VEHICLE_ENGINE_COMPONENT_H
#define VEHICLE_ENGINE_COMPONENT_H

#include "Sim/GASSCommon.h"
#include "Sim/Interface/GASSITerrainComponent.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/GASSSceneObjectRef.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Plugins/Game/GameMessages.h"
#include "Utils/PIDControl.h"

namespace GASS
{


	class SceneObject;
	typedef SPTR<SceneObject> SceneObjectPtr;
	typedef WPTR<SceneObject> SceneObjectWeakPtr;

	class VehicleWheel :  public SHARE_CLASS<VehicleWheel> ,public IMessageListener
	{
	public:
		VehicleWheel(SceneObjectPtr  wheel);
		virtual ~VehicleWheel();
		void Init();
		void OnPhysicsMessage(VelocityNotifyMessagePtr message);

		//private: //public for fast access
		SceneObjectWeakPtr m_WheelObject;
		float m_Velocity;
		float m_AngularVelocity;
	};
	typedef SPTR<VehicleWheel> VehicleWheelPtr;

	enum EngineType
	{
		ET_CAR,
		ET_TANK,
	};

	class VehicleEngineComponent :  public Reflection<VehicleEngineComponent,BaseSceneComponent>
	{
	public:
		VehicleEngineComponent();
		virtual ~VehicleEngineComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();
		void SceneManagerTick(double delta);
		float GetTorque(float throttle);
		float GetRPM(){return m_RPM;}
		int GetGear(){return m_Gear;}
		float GetNormRPM();
	private:

		ADD_ATTRIBUTE(float,TurnRPMAmount)
		std::vector<SceneObjectRef> GetWheels() const;
		void SetWheels(const std::vector<SceneObjectRef> &wheels);

		
		void OnPhysicsMessage(VelocityNotifyMessagePtr message);
		void OnInput(InputControllerMessagePtr message);


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
		float m_Power;
		bool m_SmoothRPMOutput;
		float m_MaxBrakeTorque;
		float m_ConstantTorque;
		float m_MinRPM;
		float m_MaxRPM;
		float m_WheelRPM;
		float m_VehicleEngineRPM;
		float m_ThrottleAccel;
		
		float m_RPM;
		std::string m_InputToThrottle;
		std::string m_InputToSteer;
		bool m_Invert;
		float m_VehicleSpeed;

		//input holders
		int m_ShiftDown;
		int m_ShiftUp;
		float m_DesiredThrottle;
		float m_DesiredSteer;

		//Auto gear shift settings
		float m_ClutchTimeChangeGear;
		float m_DeclutchTimeChangeGear;
		float m_RPMGearChangeUp;
		float m_RPMGearChangeDown;
		int m_Automatic;
		float m_Clutch;

		//Auto gear shift helpers
		float m_AutoClutchStart;
		float m_AutoShiftStart;
		int m_FutureGear;

		//gear box
		std::vector<float> m_GearBoxRatio;
		int m_NeutralGear;
		//float m_GearBoxSpeed[MAX_GEARS];
		int m_Gear; //current gear
		double m_CurrentTime;

		//Steering vars
		//float m_TurnForce;
		float m_MaxTurnForce;
		float m_MaxTurnVel;

		std::vector<SceneObjectRef> m_WheelObjects;
		std::vector<VehicleWheelPtr> m_VehicleWheels;
		Vec3 m_AngularVelocity;
		PIDControl m_SteerCtrl;
		bool m_Initialized;
		bool m_Debug;
		EngineType m_EngineType;

	};
}
#endif
