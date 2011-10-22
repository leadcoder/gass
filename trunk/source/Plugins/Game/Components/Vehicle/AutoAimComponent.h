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

#ifndef TURRET_COMPONENT_H
#define TURRET_COMPONENT_H

#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Scenario/Scene/Messages/GraphicsSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/PhysicsSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/CoreSceneObjectMessages.h"
#include "Sim/Systems/Input/ControlSetting.h"
#include "Sim/Common.h"
#include "Sim/Scheduling/ITaskListener.h"
#include "Plugins/Game/GameMessages.h"
#include "Utils/PIDControl.h"

namespace GASS
{

	class SceneObject;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr;

	class AutoAimComponent :  public Reflection<AutoAimComponent,BaseSceneComponent>, public ITaskListener
	{
	public:
		AutoAimComponent();
		virtual ~AutoAimComponent();
		static void RegisterReflection();
		virtual void OnCreate();
	private:
		void SetBarrelHinge(const std::string &name);
		std::string GetBarrelHinge() const;
		void SetTurretHinge(const std::string &name);
		std::string GetTurretHinge() const;
		SceneObjectPtr  GetTurretHingeObject() const;
		SceneObjectPtr  GetBarrelHingeObject() const;

		Vec3 GetDesiredAimDirection(double delta_time);
		std::string GetYawController() const {return m_YawController;}
		void SetYawController(const std::string &value) {m_YawController = value;}
		std::string GetPitchController() const {return m_PitchController;}
		void SetPitchController(const std::string &value) {m_PitchController = value;}
		
		void SetMaxSteerVelocity(float value) {m_MaxSteerVelocity = value;}
		float GetMaxSteerVelocity() const {return m_MaxSteerVelocity;}
		void SetMaxSteerAngle(float value) {m_MaxSteerAngle = value;}
		float GetMaxSteerAngle() const {return m_MaxSteerAngle;}
		void SetSteerForce(float value) {m_SteerForce = value;}
		float GetSteerForce() const {return m_SteerForce;}
		void SetTurretMaxMinAngle(const Vec2 &value) {m_TurretMaxAngle = value.x;m_TurretMinAngle= value.y;}
		Vec2 GetTurretMaxMinAngle() const {return Vec2(m_TurretMaxAngle,m_TurretMinAngle);}
		void SetBarrelMaxMinAngle(const Vec2 &value) {m_BarrelMaxAngle = value.x;m_BarrelMinAngle= value.y;}
		Vec2 GetBarrelMaxMinAngle() const {return Vec2(m_BarrelMaxAngle,m_BarrelMinAngle);}
		
		PIDControl GetPitchPID() const {return m_PitchPID;}
		void SetPitchPID(const PIDControl &value) {m_PitchPID = value;}
		
		PIDControl GetYawPID() const {return m_YawPID;}
		void SetYawPID(const PIDControl &value) {m_YawPID = value;}
		

		Vec3 ProjectVectorOnPlane(const Vec3 plane_normal,const Vec3 &v);
		Float GetPitchAngle(const Vec3 v1,const Vec3 v2);
		Float GetAngleOnPlane(const Vec3 &plane_normal,const Vec3 &v1,const Vec3 &v2);
		
		void OnLoad(LoadGameComponentsMessagePtr message);
		void OnInput(ControllerMessagePtr message);
		void OnTurretHingeUpdate(HingeJointNotifyMessagePtr message);	
		void OnBarrelHingeUpdate(HingeJointNotifyMessagePtr message);
		void OnTurretTransformation(TransformationNotifyMessagePtr message);
		void OnBarrelTransformation(TransformationNotifyMessagePtr message);
		void OnBaseTransformation(TransformationNotifyMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		void OnPhysicsMessage(VelocityNotifyMessagePtr message);
		TaskGroup GetTaskGroup() const;
		void Update(double delta_time);

		std::string m_YawController;
		std::string m_PitchController;
		
		float m_MaxSteerVelocity;
		float m_MaxSteerAngle;
		float m_SteerForce;
		float m_MaxYawTorque;
		float m_MaxPitchTorque;
		float m_TurretAngle;
		float m_BarrelAngle;
		float m_TurretMinAngle;
		float m_TurretMaxAngle;
		float m_BarrelMinAngle;
		float m_BarrelMaxAngle;
		Mat4 m_BaseTransformation;
		Mat4 m_TurretTransformation;
		Mat4 m_BarrelTransformation;
		Mat4 m_StartTransformation;
		Mat4 m_AimTransformation;
		
		PIDControl m_YawPID;
		PIDControl m_PitchPID;
		bool m_Active;
		
		Float m_AngularVelocity;
		

		SceneObjectWeakPtr m_TurretHingeObject;
		SceneObjectWeakPtr m_BarrelHingeObject;

		std::string m_TurretHingeName;
		std::string m_BarrelHingeName;

		Float m_YawValue;
		Float m_PitchValue;

		Float m_YawInput;
		Float m_PitchInput;

			
	};
	typedef boost::shared_ptr<AutoAimComponent> AutoAimComponentPtr;
}
#endif
