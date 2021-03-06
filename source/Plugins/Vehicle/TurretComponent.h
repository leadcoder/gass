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

#ifndef TURRET_COMPONENT_H
#define TURRET_COMPONENT_H


#include "Sim/GASSCommon.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Messages/GASSInputMessages.h"
#include "Sim/Interface/GASSIPhysicsHingeJointComponent.h"
#include "Core/Utils/GASSPIDControl.h"

namespace GASS
{

	class SceneObject;
	typedef GASS_SHARED_PTR<SceneObject> SceneObjectPtr;
	typedef GASS_WEAK_PTR<SceneObject> SceneObjectWeakPtr;

	class TurretComponent :  public Reflection<TurretComponent,BaseSceneComponent>
	{
	public:
		TurretComponent();
		~TurretComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;
		void SceneManagerTick(double delta_time) override;
	private:
		Vec3 GetDesiredAimDirection(double delta_time);
		std::string GetController() const {return m_Controller;}
		void SetController(const std::string &value) {m_Controller = value;}
		void SetMaxSteerVelocity(float value) {m_MaxSteerVelocity = value;}
		float GetMaxSteerVelocity() const {return m_MaxSteerVelocity;}
		void SetMaxSteerAngle(float value) {m_MaxSteerAngle = value;}
		float GetMaxSteerAngle() const {return m_MaxSteerAngle;}
		void SetSteerForce(float value) {m_SteerForce = value;}
		float GetSteerForce() const {return m_SteerForce;}
		void SetMaxMinAngle(const Vec2 &value) {m_MaxAngle = static_cast<float>(value.x); m_MinAngle = static_cast<float>(value.y);}
		Vec2 GetMaxMinAngle() const {return Vec2(m_MaxAngle,m_MinAngle);}
		Float GetPitchAngle(const Vec3 v1,const Vec3 v2) const;
		Float GetAngleOnPlane(const Vec3 &plane_normal,const Vec3 &v1,const Vec3 &v2) const;

		void OnInput(InputRelayEventPtr message);
		void OnJointUpdate(ODEPhysicsHingeJointEventPtr message);
		void OnTransformation(TransformationChangedEventPtr message);
		void OnParentTransformation(TransformationChangedEventPtr message);
		void OnPhysicsMessage(PhysicsVelocityEventPtr message);
		
		std::string m_Controller;
		float m_MaxSteerVelocity;
		float m_MaxSteerAngle;
		float m_SteerForce;
		float m_CurrentAngle;
		float m_MinAngle;
		float m_MaxAngle;
		Mat4 m_Transformation;
		Mat4 m_ParentTransformation;
		Mat4 m_RelTrans;
		Mat4 m_AimTrans;
		Vec3 m_DesiredDir;
		PIDControl m_TurnPID;
		bool m_Active;
		float m_TurnInput;
		Float m_AngularVelocity;
		Float m_RotValue;

		PhysicsHingeJointComponentPtr m_Hinge;
	};
	typedef GASS_SHARED_PTR<TurretComponent> TurretComponentPtr;
}
#endif
