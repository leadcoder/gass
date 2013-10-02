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

#ifndef TANK_AUTOPILOT_H
#define TANK_AUTOPILOT_H


#include "Sim/GASSCommon.h"
#include "Sim/Interface/GASSITerrainComponent.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/Interface/GASSIControlSettingsSystem.h"
#include "Plugins/Game/GameMessages.h"
#include "Utils/PIDControl.h"

namespace GASS
{
	class TankAutopilotComponent :  public Reflection<TankAutopilotComponent,BaseSceneComponent>
	{
	public:
		TankAutopilotComponent();
		virtual ~TankAutopilotComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();
		virtual void SceneManagerTick(double delta);
	private:
		ADD_PROPERTY(bool,Enable);
		ADD_PROPERTY(float,DesiredSpeed);
		ADD_PROPERTY(float,DesiredPosRadius);
		ADD_PROPERTY(PIDControl,TurnPID);
		ADD_PROPERTY(PIDControl,TrottlePID);

		//PIDControl m_TurnPID;
		//PIDControl m_TrottlePID;
		
		std::string GetSteerInput() const{return m_SteerInput;}
		void SetSteerInput(const std::string &input) {m_SteerInput = input;}
		std::string GetThrottleInput() const{return m_ThrottleInput;}
		void SetThrottleInput(const std::string &input) {m_ThrottleInput = input;}
		

		void DriveTo(const Vec3 &pos,const Vec3 &last_pos, float desired_speed, float time);
		void OnPhysicsMessage(VelocityNotifyMessagePtr message);
		void OnInput(InputControllerMessagePtr message);
		void OnTransMessage(TransformationNotifyMessagePtr message);
		void OnGotoPosition(GotoPositionMessagePtr message);
		void OnSetDesiredSpeed(DesiredSpeedMessagePtr message);
		
		Vec3 m_AngularVelocity;
		Vec3 m_CurrentPos;
		Vec3 m_LastPos;
		Vec3 m_DesiredPos;
		
		std::string m_ThrottleInput;
		std::string m_SteerInput;
		Vec3 m_VehicleSpeed;
		Mat4 m_Transformation;
	};
}
#endif
