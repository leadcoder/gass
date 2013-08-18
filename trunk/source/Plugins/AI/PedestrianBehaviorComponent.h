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
