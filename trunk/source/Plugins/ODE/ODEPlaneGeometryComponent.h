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

#include "Sim/GASSBaseSceneComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include <ode/ode.h>
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Math/GASSAABox.h"
#include "Core/Math/GASSQuaternion.h"
#include "ODEPhysicsSceneManager.h"
//#include "ODEGeometry.h"
#include "ODEBaseGeometryComponent.h"

namespace GASS
{
	class IGeometryComponent;
	class ODEBodyComponent;
	class ODEPhysicsSceneManager;
	typedef boost::weak_ptr<ODEPhysicsSceneManager> ODEPhysicsSceneManagerWeakPtr;
	typedef boost::shared_ptr<IGeometryComponent> GeometryComponentPtr;

	class ODEPlaneGeometryComponent : public Reflection<ODEPlaneGeometryComponent,ODEBaseGeometryComponent>
	{
	friend class ODEPhysicsSceneManager;
	public:
		ODEPlaneGeometryComponent();
		virtual ~ODEPlaneGeometryComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
	protected:
		dGeomID CreateODEGeom();
		void UpdateODEGeom();
		void SetNormal(const Vec3 &normal);
		Vec3 GetNormal() const;
		void SetPlaneOffset(Float offset);
		Float GetPlaneOffset() const;


		//override and do nothting to avoid crashen, plane dont support positions and rotations!!!!!
		void SetPosition(const Vec3 &pos);
		void SetRotation(const Quaternion &rot);
	
		void UpdateBodyMass() {};
	protected:
		Vec3 m_Normal; 
		Float m_PlaneOffset;
	};
}
