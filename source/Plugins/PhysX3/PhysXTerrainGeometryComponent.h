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

#include "PhysXCommon.h"
#include "PhysXPhysicsSceneManager.h"
#include "PhysXBaseGeometryComponent.h"


namespace GASS
{
	class IHeightmapTerrainComponent;
	class IGeometryComponent;
	class PhysXPhysicsSceneManager;
	typedef GASS_WEAK_PTR<PhysXPhysicsSceneManager> PhysXPhysicsSceneManagerWeakPtr;
	typedef GASS_SHARED_PTR<IGeometryComponent> GeometryComponentPtr;
	typedef GASS_SHARED_PTR<IHeightmapTerrainComponent> HeightmapTerrainComponentPtr;

	class PhysXTerrainGeometryComponent : public Reflection<PhysXTerrainGeometryComponent,BaseSceneComponent> , public IPhysicsGeometryComponent
	{
	friend class PhysXPhysicsSceneManager;
	public:
		PhysXTerrainGeometryComponent();
		virtual ~PhysXTerrainGeometryComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
		virtual void OnDelete();
	protected:
		
		void OnCollisionSettings(CollisionSettingsRequestPtr message);
		void OnGeometryChanged(GeometryChangedEventPtr message);
		void Disable();
		void Enable();
		void Reset();
		physx::PxShape* CreateTerrain();
		HeightmapTerrainComponentPtr GetTerrainComponent() const;
	protected:
		AABox m_TerrainBounds;
		IHeightmapTerrainComponent* m_TerrainGeom;
		std::string m_GeometryTemplate;
		bool m_Debug;
		PhysXPhysicsSceneManagerWeakPtr m_SceneManager;
		physx::PxShape* m_Shape;
	};
}
