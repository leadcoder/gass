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

#pragma once 
#include "Sim/GASS.h"
#include "Core/Math/GASSSplineAnimation.h"
#include "Plugins/Game/GameMessages.h"
#include "DetourCrowd/DetourCrowd.h"
#include "Plugins/Base/CoreMessages.h"
#include "AISceneManager.h"
struct dtCrowdAgent;

namespace GASS
{
	class IWaypointListComponent;

	class AITrainComponent :  public Reflection<AITrainComponent , BaseSceneComponent>
	{
	public:
		AITrainComponent(void);
		virtual ~AITrainComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void SceneManagerTick(double delta_time);
	protected:
		ADD_PROPERTY(Float,AccelerationDistance);
		ADD_PROPERTY(Float,StopDistance);
		ADD_PROPERTY(Float,StopTime);
		ADD_PROPERTY(Float,DoorDelay);
		ADD_PROPERTY(SceneObjectID,TrainObject);
		
		void CreateSpline();

		void SetTrackDistance(Float dist);
		Float GetTrackDistance() const {return m_TrackDist;}
		std::string GetWaypointList() const;
		void SetWaypointList(const std::string &value);
		IWaypointListComponent* FindWaypointList(const std::string &name) const;
		Float GetDesiredVelocity() const;
		void SetDesiredVelocity(Float value);
		void SetVelocityTargetID(const SceneObjectID &id);
		SceneObjectID GetVelocityTargetID() const;
		void OnSpeedMessage(DesiredSpeedMessagePtr message);
		void OnLoad(LocationLoadedMessagePtr message);
		void OnTransformationChanged(TransformationNotifyMessagePtr message);
		void RecPost(MessagePtr message,SceneObjectPtr obj);
		bool m_Initialized;
		double m_CurrentTime;
		std::string m_WaypointListName;
		SplineAnimation m_SplineAnimation;
		Float m_TotDist;
		Float m_Velocity;
		Float m_DesiredVelocity;
		Float m_TrackDist;
		SceneObjectWeakPtr m_Train;


		//local
		double m_TimeStop;
		bool m_Stopped;
		bool m_DoorOpen;
		


		Vec3 m_Pos;
		Quaternion m_Rot;
		
		
	};
	typedef SPTR<AITrainComponent> AITrainComponentPtr;
}
