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

#include "PhysXPhysicsSceneManager.h"
#include "PhysXBaseGeometryComponent.h"
#include "PhysXCommon.h"

namespace GASS
{
	class IGeometryComponent;
	class PhysXPhysicsSceneManager;
	using PhysXPhysicsSceneManagerWeakPtr = std::weak_ptr<PhysXPhysicsSceneManager>;
	using GeometryComponentPtr = std::shared_ptr<IGeometryComponent>;
	
	class PhysXConvexGeometryComponent : public Reflection<PhysXConvexGeometryComponent,Component>
	{
	friend class PhysXPhysicsSceneManager;
	public:
		PhysXConvexGeometryComponent();
		~PhysXConvexGeometryComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;
		PhysXConvexMesh GetConvexMesh() const {return m_ConvexMesh;}
	protected:
		void OnGeometryChanged(GeometryChangedEventPtr message);
	protected:
		bool m_SimulationCollision{true};
		physx::PxShape *m_Shape{nullptr};
		PhysXConvexMesh m_ConvexMesh;
		PhysXPhysicsSceneManagerWeakPtr m_SceneManager;
	};
	using PhysXConvexGeometryComponentPtr = std::shared_ptr<PhysXConvexGeometryComponent>;
}
