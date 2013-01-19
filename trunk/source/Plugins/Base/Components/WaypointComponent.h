/****************************************************************************
*                                                                           *
* GASS                                                                *
* Copyright (C)2003 - 2005 Johan Hedstrom                                   *
* Email: GASS@gmail.com                                               *
* Web page: http://n00b.dyndns.org/GASS                               *
*                                                                           *
* GASS is only used with knowledge from the author. This software     *
* is not allowed to redistribute without permission from the author.        *
* For further license information, please turn to the product home page or  *
* contact author. Abuse against the GASS license is prohibited by law.*
*                                                                           *
*****************************************************************************/ 

#pragma once
#include "Core/Common.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Math/GASSVector.h"
#include "Sim/GASSCommon.h"
#include "Sim/Interface/GASSICameraComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSSceneObject.h"
#include "Plugins/Base/CoreMessages.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"


namespace GASS
{
	class WaypointComponent;
	typedef WPTR<WaypointComponent> WaypointComponentWeakPtr;

	class WaypointComponent : public Reflection<WaypointComponent,BaseSceneComponent>
	{
	public:
		WaypointComponent();
		virtual ~WaypointComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();
		Float GetTangentWeight()const;
		void SetTangentLength(Float value);
		void SetTangent(const Vec3 &tangent);
		Vec3 GetTangent() const;
		bool GetCustomTangent() const {return m_CustomTangent;}
	protected:
		void SetTangentWeight(Float value);
		void NotifyUpdate();
		void OnChangeName(GASS::SceneObjectNameMessagePtr message);
		void OnMoved(MessagePtr message);
		void OnTangentMoved(MessagePtr message);
		void OnPosition(PositionMessagePtr message);
		void OnRotate(WorldRotationMessagePtr message);
		void UpdateTangentLine();
		
		Vec3 m_Tangent;
		Float m_TangentWeight;
		bool m_Initialized;
		bool m_CustomTangent;
	};

	typedef SPTR<WaypointComponent> WaypointComponentPtr;
	
}

