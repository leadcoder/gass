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

#ifndef AUTO_AIM_COMPONENT_H
#define AUTO_AIM_COMPONENT_H

#include "Sim/Components/GASSBaseSceneComponent.h"
#include "Sim/Scene/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Scene/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Scene/GASSCoreSceneObjectMessages.h"
#include "Sim/Scene/GASSSceneObjectLink.h"
#include "Sim/GASSCommon.h"
#include "Sim/Scheduling/GASSITaskListener.h"
#include "Plugins/Game/GameMessages.h"
#include "Utils/PIDControl.h"

namespace GASS
{

	class SceneObject;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr;

	class AutoAimComponent :  public Reflection<AutoAimComponent,BaseSceneComponent>
	{
	public:
		AutoAimComponent();
		virtual ~AutoAimComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
	private:
		void SetBarrelObject(const SceneObjectLink &value);
		SceneObjectLink GetBarrelObject() const;
		void SetTurretObject(const SceneObjectLink &value);
		SceneObjectLink GetTurretObject() const;
		
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
		

		Float GetPitchAngle(const Vec3 v1,const Vec3 v2);
		Float GetAngleOnPlane(const Vec3 &plane_normal,const Vec3 &v1,const Vec3 &v2);
		
		void OnLoad(LoadComponentsMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);

		void OnTurretHingeUpdate(HingeJointNotifyMessagePtr message);	
		void OnBarrelHingeUpdate(HingeJointNotifyMessagePtr message);
		void OnTurretTransformation(TransformationNotifyMessagePtr message);
		void OnBarrelTransformation(TransformationNotifyMessagePtr message);
		void OnBaseTransformation(TransformationNotifyMessagePtr message);
		//void OnPhysicsMessage(VelocityNotifyMessagePtr message);
		void OnAimAtPosition( AimAtPositionMessagePtr message);
		void OnActivateAutoAim(ActivateAutoAimMessagePtr message);
		void SceneManagerTick(double delta_time);
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
		PIDControl m_YawPID;
		PIDControl m_PitchPID;
		bool m_Active;
		//Float m_AngularVelocity;
		SceneObjectLink  m_TurretObject;
		SceneObjectLink  m_BarrelObject;
		Vec3 m_AimPoint;
		bool m_GotNewAimPoint;
		int m_CurrentAimPriority;
	};
	typedef boost::shared_ptr<AutoAimComponent> AutoAimComponentPtr;
}
#endif
