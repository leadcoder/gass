
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
		void OnUnload(UnloadComponentsMessagePtr message);
		void OnTransformationChanged(TransformationNotifyMessagePtr message);
		bool m_Initlized;
		KeyframeAnimation m_Animation;
		double m_CurrentTime;
		Quaternion m_Rotation;
		Vec3 m_Position;
	};
	typedef boost::shared_ptr<KeyframeAnimationComponent> KeyframeAnimationComponentPtr;
}
