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

#pragma once
#include "Core/Common.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Math/GASSVector.h"
#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"

namespace GASS
{
	class WaypointComponent;
	typedef GASS_WEAK_PTR<WaypointComponent> WaypointComponentWeakPtr;

	/**
		Component that hold waypoint data for WaypointListComponent
	*/
	class WaypointComponent : public Reflection<WaypointComponent,BaseSceneComponent>
	{
	public:
		WaypointComponent();
		~WaypointComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;
		//void SetTangentLength(Float value);
		void SetTangent(const Vec3 &tangent);
		Vec3 GetTangent() const;
		bool GetCustomTangent() const {return m_CustomTangent;}
		void SetCustomTangent(bool value);
		void Rotate(const Quaternion &rot);
		bool IsActive() const { return m_Active; }
	protected:
		//@deprecated
		void SetTangentWeight(Float value);
		Float GetTangentWeight()const;

		void OnPostInitializedEvent(PostInitializedEventPtr message);
		void OnTransformation(TransformationChangedEventPtr event);
		void OnTangentTransformation(TransformationChangedEventPtr event);
		void _UpdateTangentLine();
		void _NotifyUpdate();

		Vec3 m_Tangent;
		Float m_TangentWeight;
		bool m_Initialized;
		bool m_CustomTangent;
		bool m_Active;
		bool m_TrackTransformation;
	};

	typedef GASS_SHARED_PTR<WaypointComponent> WaypointComponentPtr;
	
}

