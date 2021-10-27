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

#ifndef VELOCITY_TO_SOUND_COMPONENT_H
#define VELOCITY_TO_SOUND_COMPONENT_H


#include "Sim/GASSCommon.h"
#include "Sim/GASSComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"

namespace GASS
{

	class SceneObject;
	using SceneObjectPtr = std::shared_ptr<SceneObject>;
	using SceneObjectWeakPtr = std::weak_ptr<SceneObject>;

	class VelocityToSoundComponent :  public Reflection<VelocityToSoundComponent,Component>
	{
	public:
		VelocityToSoundComponent();
		~VelocityToSoundComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void SceneManagerTick(double delta_time) override;
	private:
		void OnHingeReport(PhysicsHingeJointReportEventPtr message);
		Float m_Volume{1.0};
		Float m_Pitch{1.0};
		Float m_TargetPitch{1.0};
		Float m_MaxVelRequest{0};
		Vec2 m_MinMaxVolume;
		Vec2 m_MinMaxPitch;
		Float m_VelocityLimit{16};
	};
}
#endif
