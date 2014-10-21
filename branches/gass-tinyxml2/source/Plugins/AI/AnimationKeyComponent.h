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
	class AnimationKeyComponent :  public Reflection<AnimationKeyComponent , BaseSceneComponent>
	{
	public:
		AnimationKeyComponent(void);
		virtual ~AnimationKeyComponent(void);
		static void RegisterReflection();
		virtual void OnInitialize();
		Key GetKey()const {return m_Key;}
		
	protected:
		void UpdateAnimation();
		
		Float GetTime() const {return m_Key.m_Time;}
		void SetTime(Float value){m_Key.m_Time = value;UpdateAnimation();}
		Vec3 GetPosition() const{ return m_Key.m_Pos;}
		void SetPosition(const Vec3 &pos) {m_Key.m_Pos = pos;UpdateAnimation();}
		Quaternion GetRotation() const {return m_Key.m_Rot;}
		void SetRotation(const Quaternion &rot) {m_Key.m_Rot = rot;UpdateAnimation();}
		Vec3 GetScale() const {return m_Key.m_Scale;}
		void SetScale(const Vec3 &scale) {m_Key.m_Scale= scale;UpdateAnimation();}
		bool GetUsePosition() const {return m_Key.m_UsePos;}
		void SetUsePosition(bool value) {m_Key.m_UsePos= value;UpdateAnimation();}
		bool GetUseRotation() const {return m_Key.m_UseRot;}
		void SetUseRotation(bool value) {m_Key.m_UseRot= value;UpdateAnimation();}

		bool m_Initialized;
		Key m_Key;
	};
	typedef SPTR<AnimationKeyComponent> AnimationKeyComponentPtr;
}
