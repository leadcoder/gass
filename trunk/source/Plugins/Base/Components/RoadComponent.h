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
#include "Sim/Scheduling/ITaskListener.h"
#include "Plugins/Base/CoreMessages.h"

namespace GASS
{
	class RoadComponent : public Reflection<RoadComponent,BaseSceneComponent>
	{
	public:
		RoadComponent();
		virtual ~RoadComponent();
		static void RegisterReflection();
		virtual void OnCreate();
	protected:
		void SetFlatTerrain(bool value);
		bool GetFlatTerrain() const;
		void SetFlatWidth(float value) {m_FlatWidth = value;}
		float GetFlatWidth() const {return m_FlatWidth;}
		void SetFlatFade(float value) {m_FlatFade = value;}
		float GetFlatFade() const {return m_FlatFade;}

		void OnUnload(UnloadComponentsMessagePtr message);
		void OnLoad(LoadCoreComponentsMessagePtr message);
		bool m_Initialized;
		float m_FlatFade;
		float m_FlatWidth;
		
	};

	typedef boost::shared_ptr<RoadComponent> RoadComponentPtr;
	typedef boost::weak_ptr<RoadComponent> RoadComponentWeakPtr;
	
}

