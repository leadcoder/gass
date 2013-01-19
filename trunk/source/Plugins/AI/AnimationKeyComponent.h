
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
