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

#ifndef TRACK_COMPONENT_H
#define TRACK_COMPONENT_H


#include "Sim/GASSCommon.h"
#include "Sim/GASSComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/GASSSceneObjectRef.h"

namespace GASS
{

	class SceneObject;
	using SceneObjectPtr = std::shared_ptr<SceneObject>;
	using SceneObjectWeakPtr = std::weak_ptr<SceneObject>;

	class TrackComponent :  public Reflection<TrackComponent,Component>
	{
	public:
		TrackComponent();
		~TrackComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
	private:
		void OnDriveWheelPhysicsMessage(PhysicsVelocityEventPtr message);

		Vec2 GetAnimationSpeedFactor()const {return m_AnimationSpeedFactor;}
		float GetParticleEmissionFactor() const{return m_ParticleEmissionFactor;}
		void SetParticleEmissionFactor(float value) {m_ParticleEmissionFactor = value;}
		float GetSoundVolumeFactor() const{return m_SoundVolumeFactor;}
		void SetSoundVolumeFactor(float value) {m_SoundVolumeFactor = value;}
		void SetAnimationSpeedFactor(const Vec2 &value){m_AnimationSpeedFactor=value;}
		
		SceneObjectRef m_DriveWheel;
		bool m_Initialized{false};
		Vec2 m_AnimationSpeedFactor;
		Vec2 m_AnimationValue;
		float m_ParticleEmissionFactor{0.6f};
		float m_SoundVolumeFactor{1.0f};
	};
}
#endif
