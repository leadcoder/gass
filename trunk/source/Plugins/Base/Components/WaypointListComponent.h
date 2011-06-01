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
	class SplineAnimation;
	class WaypointListComponent : public Reflection<WaypointListComponent,BaseSceneComponent>
	{
		friend class WaypointComponent;
	public:
		WaypointListComponent();
		virtual ~WaypointListComponent();
		static void RegisterReflection();
		virtual void OnCreate();
	protected:
		float GetRadius()const;
		void SetRadius(float radius);
		int GetSplineSteps()const;
		void SetSplineSteps(int steps);
		
		bool GetEnableSpline()const;
		void SetEnableSpline(bool value);
		bool GetAutoUpdateTangents()const;
		void SetAutoUpdateTangents(bool value);
		
		void RecursiveIncreaseResolution(const Vec3& line_start,  const Vec3& line_end, SplineAnimation &spline, Float min_dist) const;
		
		void OnUnload(UnloadComponentsMessagePtr message);
		void OnLoad(LoadCoreComponentsMessagePtr message);
		void OnMoved(TransformationNotifyMessagePtr message);
		void OnUpdate(UpdateWaypointListMessagePtr message);

		//Helpers
		std::vector<Vec3> GetWaypoints() const;
		void UpdatePath();

		float m_Radius;
		int m_SplineSteps;
		bool m_EnableSpline;
		bool m_Initialized;
		bool m_AutoUpdateTangents;
	};

	typedef boost::shared_ptr<WaypointListComponent> WaypointListComponentPtr;
	typedef boost::weak_ptr<WaypointListComponent> WaypointListComponentWeakPtr;
	
}

