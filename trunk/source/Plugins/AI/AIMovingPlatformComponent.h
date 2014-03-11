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
		void OnLocationLoaded(LocationLoadedEventPtr message);

		ADD_PROPERTY(Float,Acceleration);
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
