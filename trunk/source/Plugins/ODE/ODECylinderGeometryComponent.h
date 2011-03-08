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

#include "Sim/Components/BaseSceneComponent.h"
#include "Sim/Scenario/Scene/SceneObjectMessages.h"
#include <ode/ode.h>
#include "Core/MessageSystem/IMessage.h"
#include "Core/Math/AABox.h"
#include "Core/Math/Quaternion.h"
#include "ODEPhysicsSceneManager.h"
//#include "ODEGeometry.h"
#include "ODEBaseGeometryComponent.h"

namespace GASS
{
	class ITerrainComponent;
	class IGeometryComponent;
	class ODEBodyComponent;
	class ODEPhysicsSceneManager;
	typedef boost::weak_ptr<ODEPhysicsSceneManager> ODEPhysicsSceneManagerWeakPtr;
	typedef boost::shared_ptr<IGeometryComponent> GeometryComponentPtr;

	class ODECylinderGeometryComponent : public Reflection<ODECylinderGeometryComponent,ODEBaseGeometryComponent>
	{
	friend class ODEPhysicsSceneManager;
	public:
		ODECylinderGeometryComponent();
		virtual ~ODECylinderGeometryComponent();
		static void RegisterReflection();
		virtual void OnCreate();
	protected:
		dGeomID CreateODEGeom();
		void SetRadius(Float radius);
		Float GetRadius() const;
		void SetLength(Float length);
		Float GetLength() const;

		void UpdateBodyMass();
		void SetSizeFromMesh(bool value);
		//debug functions
		void CreateDebugBox(const Vec3 &size,const Vec3 &offset);
		void UpdateDebug();
	protected:
		Float m_Radius; 
		Float m_Length;
	};
}
