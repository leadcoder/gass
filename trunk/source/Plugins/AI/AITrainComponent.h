
/****************************************************************************
*                                                                           *
* HiFiEngine                                                                *
* Copyright (C)2003 - 2005 Johan Hedstrom                                   *
* Email: hifiengine@gmail.com                                               *
* Web page: http://n00b.dyndns.org/HiFiEngine                               *
*                                                                           *
* HiFiEngine is only used with knowledge from the author. This software     *
* is not allowed to redistribute without permission from the author.        *
* For further license information, please turn to the product home page or  *
* contact author. Abuse against the HiFiEngine license is prohibited by law.*
*                                                                           *
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
		ADD_ATTRIBUTE(Float,AccelerationDistance);
		ADD_ATTRIBUTE(Float,StopDistance);
		ADD_ATTRIBUTE(Float,StopTime);
		ADD_ATTRIBUTE(Float,DoorDelay);
		ADD_ATTRIBUTE(SceneObjectID,TrainObject);
		
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
	typedef boost::shared_ptr<AITrainComponent> AITrainComponentPtr;
}
