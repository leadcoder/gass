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
	using PhysXPhysicsSceneManagerWeakPtr = std::weak_ptr<PhysXPhysicsSceneManager>;
	using GeometryComponentPtr = std::shared_ptr<IGeometryComponent>;
	using HeightmapTerrainComponentPtr = std::shared_ptr<IHeightmapTerrainComponent>;

	class PhysXTerrainGeometryComponent : public Reflection<PhysXTerrainGeometryComponent,Component> , public IPhysicsGeometryComponent
	{
	friend class PhysXPhysicsSceneManager;
	public:
		PhysXTerrainGeometryComponent();
		~PhysXTerrainGeometryComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;
	protected:
		void OnCollisionSettings(CollisionSettingsRequestPtr message);
		void OnGeometryChanged(GeometryChangedEventPtr message);
		void OnTransformationChanged(TransformationChangedEventPtr message);

		void Disable();
		void Enable();
		void Reset();
		void SetPosition(const Vec3& pos);
		void SetRotation(const Quaternion& rot);
		HeightmapTerrainComponentPtr GetTerrainComponent() const;
	protected:
		void CreateTerrain();
		void Release();

		AABox m_TerrainBounds;
		IHeightmapTerrainComponent* m_TerrainGeom{nullptr};
		std::string m_GeometryTemplate;
		bool m_Debug{false};
		PhysXPhysicsSceneManagerWeakPtr m_SceneManager;
		physx::PxShape* m_Shape{nullptr};
		physx::PxRigidStatic* m_Actor{nullptr};
	};
}
