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
#include "Core/MessageSystem/IMessage.h"
#include "Core/Math/Vector.h"
#include "Sim/Common.h"
#include "Sim/Components/Graphics/ICameraComponent.h"
#include "Sim/Components/Graphics/Geometry/ILineComponent.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Plugins/Base/CoreMessages.h"


namespace GASS
{
	class WaypointComponent;
	typedef boost::weak_ptr<WaypointComponent> WaypointComponentWeakPtr;

	class WaypointComponent : public Reflection<WaypointComponent,BaseSceneComponent>
	{
	public:
		WaypointComponent();
		virtual ~WaypointComponent();
		static void RegisterReflection();
		virtual void OnCreate();

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
		void OnUnload(UnloadComponentsMessagePtr message);
		void OnLoad(LoadCoreComponentsMessagePtr message);
		void OnRotate(WorldRotationMessagePtr message);
		void UpdateTangentLine();
		
		Vec3 m_Tangent;
		Float m_TangentWeight;
		bool m_Initialized;
		bool m_CustomTangent;
	};

	typedef boost::shared_ptr<WaypointComponent> WaypointComponentPtr;
	
}
