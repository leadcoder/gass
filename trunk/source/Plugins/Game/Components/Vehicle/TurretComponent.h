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

	class TurretComponent :  public Reflection<TurretComponent,BaseSceneComponent>, public ITaskListener
	{
	public:
		TurretComponent();
		virtual ~TurretComponent();
		static void RegisterReflection();
		virtual void OnCreate();
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
		void SetMaxMinAngle(const Vec2 &value) {m_MaxAngle = value.x;m_MinAngle = value.y;}
		Vec2 GetMaxMinAngle() const {return Vec2(m_MaxAngle,m_MinAngle);}
		
		
		void OnLoad(LoadGameComponentsMessagePtr message);
		void OnInput(ControllerMessagePtr message);
		void OnJointUpdate(HingeJointNotifyMessagePtr message);
		void OnTransformation(TransformationNotifyMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		void OnPhysicsMessage(VelocityNotifyMessagePtr message);
		TaskGroup GetTaskGroup() const;
		void Update(double delta_time);
		std::string m_Controller;
		float m_MaxSteerVelocity;
		float m_MaxSteerAngle;
		float m_SteerForce;
		float m_CurrentAngle;
		float m_MinAngle;
		float m_MaxAngle;
		Mat4 m_Transformation;
		Vec3 m_DesiredDir;
		PIDControl m_TurnPID;
		bool m_Active;
		float m_TurnInput;
		Float m_AngularVelocity;
		Float m_RotValue;
	};
	typedef boost::shared_ptr<TurretComponent> TurretComponentPtr;
}
#endif
