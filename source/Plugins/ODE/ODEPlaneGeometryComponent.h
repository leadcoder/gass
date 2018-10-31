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
#include "Sim/GASSCommon.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include <ode/ode.h>
#include "Core/Math/GASSAABox.h"
#include "Core/Math/GASSQuaternion.h"
#include "ODEPhysicsSceneManager.h"
#include "ODEBaseGeometryComponent.h"

namespace GASS
{
	class IGeometryComponent;
	class ODEBodyComponent;
	class ODEPhysicsSceneManager;
	typedef GASS_WEAK_PTR<ODEPhysicsSceneManager> ODEPhysicsSceneManagerWeakPtr;
	typedef GASS_SHARED_PTR<IGeometryComponent> GeometryComponentPtr;

	class ODEPlaneGeometryComponent : public Reflection<ODEPlaneGeometryComponent,ODEBaseGeometryComponent>
	{
	friend class ODEPhysicsSceneManager;
	public:
		ODEPlaneGeometryComponent();
		~ODEPlaneGeometryComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
	protected:
		dGeomID CreateODEGeom() override;
		void UpdateODEGeom() override;
		void SetNormal(const Vec3 &normal);
		Vec3 GetNormal() const;
		void SetPlaneOffset(Float offset);
		Float GetPlaneOffset() const;


		//override and do nothting to avoid crashen, plane dont support positions and rotations!!!!!
		void SetPosition(const Vec3 &pos) override;
		void SetRotation(const Quaternion &rot) override;
	
		void UpdateBodyMass() override {};
	protected:
		Vec3 m_Normal; 
		Float m_PlaneOffset;
	};
}
