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

#ifndef ADAPTIVE_STEER_COMPONENT_H
#define ADAPTIVE_STEER_COMPONENT_H

#include "Sim/GASSCommon.h"
#include "Sim/GASSComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Messages/GASSInputMessages.h"

namespace GASS
{

	class SceneObject;
	using SceneObjectPtr = std::shared_ptr<SceneObject>;
	using SceneObjectWeakPtr = std::weak_ptr<SceneObject>;

	class AdaptiveSteerComponent :  public Reflection<AdaptiveSteerComponent,Component>
	{
	public:
		AdaptiveSteerComponent();
		~AdaptiveSteerComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
	private:
		void OnJointUpdate(ODEPhysicsHingeJointEventPtr message);
		void OnInput(InputRelayEventPtr message);
		void OnVelocityMessage(PhysicsVelocityEventPtr message);
		void SetSteerForce(float value) {m_SteerForce = value;}
		float GetSteerForce() const {return m_SteerForce;}
		void SetMaxSteerAngleAtSpeed(Vec2 value) {m_MaxSteerAngleAtSpeed = value;}
		Vec2 GetMaxSteerAngleAtSpeed() const {return m_MaxSteerAngleAtSpeed;}
		void SetMinSteerAngleAtSpeed(Vec2 value) {m_MinSteerAngleAtSpeed = value;}
		Vec2 GetMinSteerAngleAtSpeed() const {return m_MinSteerAngleAtSpeed;}
		void SetSpeedMultiplier(float value) {m_Speed = value;}
		float GetSpeedMultiplier() const {return m_Speed;}
		int GetDynamicInputPower() const {return m_DynamicInputPower;}
		void SetDynamicInputPower(int value) {m_DynamicInputPower =value;}
		float GetMaxSteerVelocity() const { return m_MaxSteerVelocity; }
		void SetMaxSteerVelocity(float value) { m_MaxSteerVelocity = value; }

		float m_Speed{1};
		float m_SteerForce{100};
		Vec2 m_MaxSteerAngleAtSpeed; 
		Vec2 m_MinSteerAngleAtSpeed; 
		
		float m_MaxSteerVelocity{1};
		float m_CurrentAngle{0};
		float m_DesiredAngle{0};
		float m_VehicleSpeed{0};
		int m_DynamicInputPower{4};
	};
}
#endif
