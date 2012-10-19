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
#include "Sim/Interface/GASSIWaypointListComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Plugins/Base/CoreMessages.h"

namespace GASS
{
	class SplineAnimation;
	class WaypointListComponent : public Reflection<WaypointListComponent,BaseSceneComponent>, public IWaypointListComponent
	{
		friend class WaypointComponent;
	public:
		WaypointListComponent();
		virtual ~WaypointListComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual std::vector<Vec3> GetWaypoints(bool relative_position = true) const;
		float GetRadius()const;
	protected:
		ADD_ATTRIBUTE(bool,ShowPathLine);
		
		void SetRadius(float radius);
		int GetSplineSteps()const;
		void SetSplineSteps(int steps);
		
		bool GetEnableSpline()const;
		void SetEnableSpline(bool value);

		void SetShowWaypoints(bool value);
		bool GetShowWaypoints() const;

		bool GetAutoUpdateTangents()const;
		void SetAutoUpdateTangents(bool value);

		void SetExport(const std::string &filename);
		std::string GetExport() const;

		
		void RecursiveIncreaseResolution(const Vec3& line_start,  const Vec3& line_end, SplineAnimation &spline, Float min_dist) const;
		
		void OnMoved(TransformationNotifyMessagePtr message);
		void OnUpdate(UpdateWaypointListMessagePtr message);

		//Helpers
		
		void UpdatePath();

		float m_Radius;
		int m_SplineSteps;
		bool m_EnableSpline;
		bool m_Initialized;
		bool m_AutoUpdateTangents;
		bool m_ShowWaypoints;
		Vec4 m_LineColor;
	};

	typedef boost::shared_ptr<WaypointListComponent> WaypointListComponentPtr;
	typedef boost::weak_ptr<WaypointListComponent> WaypointListComponentWeakPtr;
	
}

