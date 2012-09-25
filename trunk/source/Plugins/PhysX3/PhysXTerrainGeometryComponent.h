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

#include "PhysXPhysicsSceneManager.h"
#include "PhysXBaseGeometryComponent.h"
#include "PhysXCommon.h"

namespace GASS
{
	class ITerrainComponent;
	class IGeometryComponent;
	class PhysXPhysicsSceneManager;
	typedef boost::weak_ptr<PhysXPhysicsSceneManager> PhysXPhysicsSceneManagerWeakPtr;
	typedef boost::shared_ptr<IGeometryComponent> GeometryComponentPtr;
	typedef boost::shared_ptr<ITerrainComponent> TerrainComponentPtr;

	class PhysXTerrainGeometryComponent : public Reflection<PhysXTerrainGeometryComponent,BaseSceneComponent> , public IPhysicsGeometryComponent
	{
	friend class PhysXPhysicsSceneManager;
	public:
		PhysXTerrainGeometryComponent();
		virtual ~PhysXTerrainGeometryComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
	protected:
		void OnLoad(LoadComponentsMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		void OnCollisionSettings(CollisionSettingsMessagePtr message);
		void OnGeometryChanged(GeometryChangedMessagePtr message);
		void OnPhysicsDebug(PhysicsDebugMessagePtr message);
		void SetFriction(float value){m_Friction = value;}
		float GetFriction() const {return m_Friction;}
		unsigned long GetCollisionBits() const;
		void SetCollisionBits(unsigned long value);
		unsigned long GetCollisionCategory() const;
		void SetCollisionCategory(unsigned long value);
		void Disable();
		void Enable();
		void Reset();
		physx::PxShape* CreateTerrain();
		TerrainComponentPtr GetTerrainComponent() const;
	protected:
		Float m_SampleWidth;
		Float m_SampleHeight;
		AABox m_TerrainBounds;
		ITerrainComponent* m_TerrainGeom;
		unsigned long m_CollisionCategory;
		unsigned long m_CollisionBits;
		std::string m_GeometryTemplate;
		float m_Friction;
		bool m_Debug;
		PhysXPhysicsSceneManagerWeakPtr m_SceneManager;
		physx::PxShape* m_Shape;
	};
}
