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
#include <ode/ode.h>
#include "Core/Math/GASSAABox.h"
#include "ODEPhysicsSceneManager.h"
#include "ODEBaseGeometryComponent.h"

namespace GASS
{
	class IGeometryComponent;
	class ODEBodyComponent;
	class ODEPhysicsSceneManager;
	using ODEPhysicsSceneManagerWeakPtr = std::weak_ptr<ODEPhysicsSceneManager>;
	using GeometryComponentPtr = std::shared_ptr<IGeometryComponent>;

	class ODECylinderGeometryComponent : public Reflection<ODECylinderGeometryComponent,ODEBaseGeometryComponent>
	{
	friend class ODEPhysicsSceneManager;
	public:
		ODECylinderGeometryComponent();
		~ODECylinderGeometryComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
	protected:
		dGeomID CreateODEGeom() override;
		void SetRadius(Float radius);
		Float GetRadius() const;
		void SetLength(Float length);
		Float GetLength() const;

		void UpdateBodyMass() override;
		void SetSizeFromMesh(bool value) override;
		//debug functions
		//void CreateDebugBox(const Vec3 &size,const Vec3 &offset);
		void UpdateDebug() override;
	protected:
		Float m_Radius{1}; 
		Float m_Length{3};
	};
}
