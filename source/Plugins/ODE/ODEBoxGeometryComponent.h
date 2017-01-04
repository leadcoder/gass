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
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include <ode/ode.h>
#include "Core/Math/GASSAABox.h"
#include "ODEPhysicsSceneManager.h"
#include "ODEBaseGeometryComponent.h"

namespace GASS
{
	class ITerrainComponent;
	class IGeometryComponent;
	class ODEBodyComponent;
	class ODEPhysicsSceneManager;
	typedef GASS_WEAK_PTR<ODEPhysicsSceneManager> ODEPhysicsSceneManagerWeakPtr;
	typedef GASS_SHARED_PTR<IGeometryComponent> GeometryComponentPtr;

	class ODEBoxGeometryComponent : public Reflection<ODEBoxGeometryComponent,ODEBaseGeometryComponent>
	{
	friend class ODEPhysicsSceneManager;
	public:
		ODEBoxGeometryComponent();
		virtual ~ODEBoxGeometryComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
	protected:
		dGeomID CreateODEGeom();
		void SetSize(const Vec3 &size);
		Vec3 GetSize() const;
		void UpdateBodyMass();
		void SetSizeFromMesh(bool value);
		void OnGeometryScale(GeometryScaleRequestPtr message);
		//debug functions
		void CreateDebugBox(const Vec3 &size,const Vec3 &offset);
		void UpdateDebug();
	protected:
		Vec3 m_Size;
		Vec3 m_Scale;
	};
}
