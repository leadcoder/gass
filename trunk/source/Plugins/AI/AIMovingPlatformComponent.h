
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

	class AIMovingPlatformComponent :  public Reflection<AIMovingPlatformComponent , BaseSceneComponent>
	{
	public:
		AIMovingPlatformComponent(void);
		virtual ~AIMovingPlatformComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void SceneManagerTick(double delta_time);
	protected:
		void OnSpeedMessage(DesiredSpeedMessagePtr message);
		void OnLocationLoaded(LocationLoadedMessagePtr message);

		ADD_ATTRIBUTE(Float,Acceleration);
		std::string GetWaypointList() const;
		void SetWaypointList(const std::string &value);
		IWaypointListComponent* FindWaypointList(const std::string &name) const;
		Float GetDesiredVelocity() const;
		void SetDesiredVelocity(Float value);
		void SetVelocityTargetID(const SceneObjectID &id);
		SceneObjectID GetVelocityTargetID() const;
		
		bool m_Initialized;
		double m_CurrentTime;
		std::string m_WaypointListName;
		SplineAnimation m_SplineAnimation;
		Float m_TotDist;
		Float m_Velocity;
		Float m_DesiredVelocity;
	};
	typedef SPTR<AIMovingPlatformComponent> AIMovingPlatformComponentPtr;
}
