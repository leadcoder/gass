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
#include "Sim/GASSComponent.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"

namespace GASS
{
	class WaypointComponent;
	using WaypointComponentWeakPtr = std::weak_ptr<WaypointComponent>;

	/**
		Component that hold waypoint data for WaypointListComponent
	*/
	class WaypointComponent : public Reflection<WaypointComponent,Component>
	{
	public:
		WaypointComponent();
		~WaypointComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;
		void SetTangent(const Vec3 &tangent);
		Vec3 GetTangent();
		bool GetCustomTangent() const {return m_CustomTangent;}
		void SetCustomTangent(bool value);
		void Rotate(const Quaternion &rot);
		bool IsActive() const { return m_Active; }
	protected:
		//@deprecated
		void SetTangentWeight(Float value);
		Float GetTangentWeight() const;
		SceneObjectPtr GetOrCreateTangent();
		void OnPostInitializedEvent(PostInitializedEventPtr message);
		void OnTransformation(TransformationChangedEventPtr event);
		void OnTangentTransformation(TransformationChangedEventPtr event);
		void UpdateTangentLine();
		void NotifyUpdate();

		Vec3 m_Tangent;
		Float m_TangentWeight{1.0};
		bool m_Initialized{false};
		bool m_CustomTangent{false};
		bool m_Active{true};
		bool m_TrackTransformation{true};
		SceneObjectWeakPtr m_TangentObject;
	};

	using WaypointComponentPtr = std::shared_ptr<WaypointComponent>;
	
}

