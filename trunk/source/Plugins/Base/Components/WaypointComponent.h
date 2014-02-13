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


	/**
		Component that hold waypoint data for WaypointListComponent
	*/

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
		void Rotate(const Quaternion &rot);
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

