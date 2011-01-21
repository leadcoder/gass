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

#ifndef TRACK_COMPONENT_H
#define TRACK_COMPONENT_H

#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Scenario/Scene/SceneObjectMessages.h"
#include "Sim/Common.h"
#include "Core/MessageSystem/IMessage.h"
#include "Plugins/Game/GameMessages.h"

namespace GASS
{

	class SceneObject;
	typedef boost::shared_ptr<SceneObject> SceneObjectPtr;
	typedef boost::weak_ptr<SceneObject> SceneObjectWeakPtr;

	class TrackComponent :  public Reflection<TrackComponent,BaseSceneComponent>
	{
	public:
		TrackComponent();
		virtual ~TrackComponent();
		static void RegisterReflection();
		virtual void OnCreate();
	private:
		std::string GetDriveWheel() const;
		void SetDriveWheel(const std::string &wheel);
		Vec2 GetAnimationSpeedFactor()const {return m_AnimationSpeedFactor;}
		float GetParticleEmissionFactor() const{return m_ParticleEmissionFactor;}
		void SetParticleEmissionFactor(float value) {m_ParticleEmissionFactor = value;}
		void SetAnimationSpeedFactor(const Vec2 &value){m_AnimationSpeedFactor=value;}
		void OnLoad(LoadGameComponentsMessagePtr message);
		void OnDriveWheelPhysicsMessage(VelocityNotifyMessagePtr message);

		SceneObjectWeakPtr m_DriveWheel;
		std::string m_DriveWheelName;
		bool m_Initialized;
		Vec2 m_AnimationSpeedFactor;
		Vec2 m_AnimationValue;
		float m_ParticleEmissionFactor;
	};
}
#endif
