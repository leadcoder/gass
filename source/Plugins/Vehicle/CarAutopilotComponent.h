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

#ifndef CAR_AUTOPILOT_H
#define CAR_AUTOPILOT_H


#include "Sim/GASSCommon.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPlatformMessages.h"
#include "Sim/Messages/GASSSensorMessages.h"
#include "Core/Utils/GASSPIDControl.h"

namespace GASS
{
	class CarAutopilotComponent :  public Reflection<CarAutopilotComponent,BaseSceneComponent>
	{
	public:
		CarAutopilotComponent();
		~CarAutopilotComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;
		void SceneManagerTick(double delta) override;
	private:
		ADD_PROPERTY(bool,Enable);
		ADD_PROPERTY(Float,DesiredSpeed);
		ADD_PROPERTY(Float,DesiredPosRadius);
		ADD_PROPERTY(PIDControl,TurnPID);
		ADD_PROPERTY(PIDControl,TrottlePID);
		ADD_PROPERTY(Float,BrakeDistanceFactor);
		ADD_PROPERTY(bool,InvertBackWardSteering);
		ADD_PROPERTY(bool,Support3PointTurn);
		ADD_PROPERTY(Float,MaxReverseDistance);
		ADD_PROPERTY(bool, CollisionAvoidance);
		
		std::string GetSteerInput() const{return m_SteerInput;}
		void SetSteerInput(const std::string &input) {m_SteerInput = input;}
		std::string GetThrottleInput() const{return m_ThrottleInput;}
		void SetThrottleInput(const std::string &input) {m_ThrottleInput = input;}
		void _UpdateDrive(double  delta_time);
		void OnPhysicsMessage(PhysicsVelocityEventPtr message);
		void OnTransMessage(TransformationChangedEventPtr message);
		void OnGotoPosition(GotoPositionRequestPtr message);
		void OnSetDesiredSpeed(DesiredSpeedMessagePtr message);
		void OnFaceDirectionRequest(FaceDirectionRequestPtr message);
		void OnRadarEvent(VehicleRadarEventPtr message);
		void OnSensorEvent(SensorMessagePtr message);
		
		Vec3 m_AngularVelocity;
		Vec3 m_CurrentPos;
		Vec3 m_DesiredPos;
		
		std::string m_ThrottleInput;
		std::string m_SteerInput;
		Vec3 m_VehicleSpeed;
		Mat4 m_Transformation;
		bool m_WPReached;
		PlatformType m_PlatformType;

		Vec3 m_FaceDirection;
		bool m_HasDir;
		bool m_HasCollision;
		Vec3 m_CollisionPoint;
		Float m_CollisionDist;
		
		DetectionVector m_ProximityData;
	};
}
#endif
