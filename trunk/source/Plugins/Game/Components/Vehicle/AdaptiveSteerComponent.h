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

#ifndef ADAPTIVE_STEER_COMPONENT_H
#define ADAPTIVE_STEER_COMPONENT_H

#include "Sim/Components/GASSBaseSceneComponent.h"
#include "Sim/Scene/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Scene/GASSCoreSceneObjectMessages.h"
#include "Sim/Scene/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Systems/Input/GASSControlSetting.h"
#include "Sim/GASSCommon.h"
#include "Plugins/Game/GameMessages.h"


namespace GASS
{

	class SceneObject;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr;

	class AdaptiveSteerComponent :  public Reflection<AdaptiveSteerComponent,BaseSceneComponent>
	{
	public:
		AdaptiveSteerComponent();
		virtual ~AdaptiveSteerComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
	private:
		void OnLoad(LoadComponentsMessagePtr message);
		void OnJointUpdate(HingeJointNotifyMessagePtr message);
		void OnInput(ControllerMessagePtr message);
		void OnVelocityMessage(VelocityNotifyMessagePtr message);
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

		float m_Speed;
		float m_SteerForce;
		Vec2 m_MaxSteerAngleAtSpeed; 
		Vec2 m_MinSteerAngleAtSpeed; 
		
		float m_MaxSteerVelocity;
		float m_CurrentAngle;
		float m_DesiredAngle;
		float m_VehicleSpeed;
		int m_DynamicInputPower;
	};
}
#endif
