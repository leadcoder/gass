
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
	protected:
		
		ADD_ATTRIBUTE(Float,GoalRadius)
		void OnLoad(LocationLoadedMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		void OnTransformationChanged(TransformationNotifyMessagePtr message);
		void OnTriggerEnter(TriggerEnterMessagePtr);
		void OnTriggerExit(TriggerExitMessagePtr);
		SceneObjectPtr GetCurrentTarget() const {return SceneObjectPtr(m_CurrentTarget,boost::detail::sp_nothrow_tag());}

		SceneObjectPtr GetRandomLocationObject(const SceneObjectID &id) const;
		Vec3 GetRandomLocation(SceneObjectPtr so) const;
		Vec2 GetRandomSpeed() const{return m_RandomSpeed;}
		void SetRandomSpeed(const Vec2 &value);
		//TargetBehavior GetBehaviorFromID(const SceneObjectID &id) const;
		void GoToTarget(SceneObjectPtr &obj, double delay);
		void RandomRespawn(double delay);
		void GoToRandomTarget(double delay);

		SceneObjectID m_TargetLocationID;
		SceneObjectID m_SpawnLocationID;
		SceneObjectID m_TriggerID;
		SceneObjectWeakPtr m_CurrentTarget;
		
		Vec3 m_CurrentTargetLocation;
		Vec2 m_RandomSpeed;
		bool m_Initlized;
		
	};
	typedef boost::shared_ptr<PedestrianBehaviorComponent> PedestrianBehaviorComponentPtr;
}
