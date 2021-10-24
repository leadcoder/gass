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

#ifndef STEER_COMPONENT_H
#define STEER_COMPONENT_H


#include "Sim/GASSCommon.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Messages/GASSInputMessages.h"

namespace GASS
{

	class SceneObject;
	using SceneObjectPtr = std::shared_ptr<SceneObject>;
	using SceneObjectWeakPtr = std::weak_ptr<SceneObject>;

	class SteerComponent :  public Reflection<SteerComponent,BaseSceneComponent>
	{
	public:
		SteerComponent();
		~SteerComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
	private:
		void OnJointUpdate(ODEPhysicsHingeJointEventPtr message);
		void OnInput(InputRelayEventPtr message);
		void SetSteerForce(float value) {m_SteerForce = value;}
		float GetSteerForce() const {return m_SteerForce;}
		void SetMaxSteerVelocity(float value) {m_MaxSteerVelocity = value;}
		float GetMaxSteerVelocity() const {return m_MaxSteerVelocity;}
		void SetMaxSteerAngle(float value) {m_MaxSteerAngle = value;}
		float GetMaxSteerAngle() const {return m_MaxSteerAngle;}
		void SetSpeedMultiplier(float value) {m_Speed = value;}
		float GetSpeedMultiplier() const {return m_Speed;}

		float m_Speed{1};
		float m_SteerForce{100};
		float m_MaxSteerVelocity{1};
		float m_MaxSteerAngle{45};
		float m_CurrentAngle{0};
		float m_DesiredAngle{0};
	};
}
#endif
