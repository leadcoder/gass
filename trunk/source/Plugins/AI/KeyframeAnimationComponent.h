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
#include "Core/Math/GASSKeyframeAnimation.h"
#include "Plugins/Game/GameMessages.h"
#include "DetourCrowd/DetourCrowd.h"
#include "Plugins/Base/CoreMessages.h"
#include "AISceneManager.h"
struct dtCrowdAgent;

namespace GASS
{
	class KeyframeAnimationComponent :  public Reflection<KeyframeAnimationComponent , BaseSceneComponent>
	{
	public:
		KeyframeAnimationComponent(void);
		virtual ~KeyframeAnimationComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void SceneManagerTick(double delta_time);
		void UpdateAnimation();
	protected:
		void OnLoad(LocationLoadedMessagePtr message);
		void OnTransformationChanged(TransformationNotifyMessagePtr message);
		bool m_Initialized;
		KeyframeAnimation m_Animation;
		double m_CurrentTime;
		Quaternion m_Rotation;
		Vec3 m_Position;
	};
	typedef SPTR<KeyframeAnimationComponent> KeyframeAnimationComponentPtr;
}
