
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
#include "Plugins/Game/GameMessages.h"
#include "DetourCrowd/DetourCrowd.h"
#include "Plugins/Base/CoreMessages.h"
#include "Plugins/AI/AIMessages.h"
struct dtCrowdAgent;

namespace GASS
{
	class PedestrianBehaviorComponent :  public Reflection<PedestrianBehaviorComponent , BaseSceneComponent>
	{
	public:
		PedestrianBehaviorComponent(void);
		virtual ~PedestrianBehaviorComponent(void);
		static void RegisterReflection();
		void OnInitialize();
		void SetTargetID(const SceneObjectID &id);
		SceneObjectID GetTargetID() const;
		void SetSpawnLocationID(const SceneObjectID &id);
		SceneObjectID GetSpawnLocationID() const;
		virtual void SceneManagerTick(double delta_time);
		std::string GetState() {return m_State;}
	protected:
		void SetHealth(Float health);
		Float GetHealth() const;
	
		ADD_ATTRIBUTE(Float,GoalRadius)
		ADD_ATTRIBUTE(SceneObjectID ,ExitLocationID)
		ADD_ATTRIBUTE(Float,FleeThreshold)
		ADD_ATTRIBUTE(Float,FleeSpeed)
		

		void OnLoad(LocationLoadedMessagePtr message);
		void OnTransformationChanged(TransformationNotifyMessagePtr message);
		void OnTriggerEnter(TriggerEnterMessagePtr);
		void OnTriggerExit(TriggerExitMessagePtr);
		void OnHealthChanged(HealthChangedMessagePtr message);
		SceneObjectPtr GetCurrentTarget() const {return SceneObjectPtr(m_CurrentTarget,NO_THROW);}

		SceneObjectPtr GetRandomLocationObject(const SceneObjectID &id) const;
		Vec3 GetRandomLocation(SceneObjectPtr so) const;
		Vec2 GetRandomSpeed() const{return m_RandomSpeed;}
		void SetRandomSpeed(const Vec2 &value);
		//TargetBehavior GetBehaviorFromID(const SceneObjectID &id) const;
		void GoToTarget(SceneObjectPtr &obj, double delay);
		void RandomRespawn(double delay);
		void GoToRandomTarget(double delay);

		SceneObjectID m_TargetLocationID;
		SceneObjectID m_InitialTargetLocationID;
		SceneObjectID m_SpawnLocationID;
		SceneObjectWeakPtr m_CurrentTarget;
		
		Vec3 m_CurrentTargetLocation;
		Vec2 m_RandomSpeed;
		bool m_Initialized;
		Vec3 m_Position;
		std::string m_State;
		std::string m_DebugState;
		Float m_Health;
		
	};
	typedef SPTR<PedestrianBehaviorComponent> PedestrianBehaviorComponentPtr;
}
