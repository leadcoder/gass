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
#include "Sim/Scenario/Scene/Messages/PhysicsSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/GraphicsSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/CoreSceneObjectMessages.h"
#include "Sim/Components/Physics/IPhysicsGeometryComponent.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Math/AABox.h"
#include "Core/Math/Quaternion.h"
#include "HavokPhysicsSceneManager.h"

// Need some shapes
//
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics/Collide/Shape/HeightField/SampledHeightField/hkpSampledHeightFieldShape.h>
#include <Physics/Collide/Shape/HeightField/SampledHeightField/hkpSampledHeightFieldBaseCinfo.h>
#include <Common/Base/Types/Geometry/Aabb/hkAabb.h>
#include <Common/Base/Algorithm/PseudoRandom/hkPseudoRandomGenerator.h>

#include <Physics/Collide/Query/CastUtil/hkpWorldRayCastInput.h>
#include <Physics/Collide/Query/CastUtil/hkpWorldRayCastOutput.h>
#include <Physics/Collide/Query/Collector/RayCollector/hkpClosestRayHitCollector.h>

namespace GASS
{
	class CoarseSampledHeightFieldShape;
	class ITerrainComponent;
	class IGeometryComponent;
	typedef boost::weak_ptr<HavokPhysicsSceneManager> HavokPhysicsSceneManagerWeakPtr;
	typedef boost::shared_ptr<IGeometryComponent> GeometryComponentPtr;
	typedef boost::shared_ptr<ITerrainComponent> TerrainComponentPtr;


	class HavokTerrainGeometryComponent : public Reflection<HavokTerrainGeometryComponent,BaseSceneComponent> , public IPhysicsGeometryComponent
	{
		friend class ODEPhysicsSceneManager;
	public:
		HavokTerrainGeometryComponent();
		virtual ~HavokTerrainGeometryComponent();
		static void RegisterReflection();
		virtual void OnCreate();
		Float GetTerrainHeight(unsigned int x,unsigned int z);
	protected:
		void OnLoad(LoadPhysicsComponentsMessagePtr message);
		void OnCollisionSettings(CollisionSettingsMessagePtr message);
		void OnGeometryChanged(GeometryChangedMessagePtr message);
		void OnPhysicsDebug(PhysicsDebugMessagePtr message);
		void SetFriction(float value){m_Friction = value;}
		float GetFriction() const {return m_Friction;}
		long int GetCollisionBits() const;
		void SetCollisionBits(long int value);
		long int GetCollisionCategory() const;
		void SetCollisionCategory(long int value);
		void Disable();
		void Enable();
		void Reset();
		TerrainComponentPtr GetTerrainComponent() const;
		
		
	protected:
		Float m_SampleWidth;
		Float m_SampleHeight;
		AABox m_TerrainBounds;
		ITerrainComponent* m_TerrainGeom;
		long int m_CollisionCategory;
		long int m_CollisionBits;
		std::string m_GeometryTemplate;
		float m_Friction;
		bool m_Debug;
		HavokPhysicsSceneManagerWeakPtr m_SceneManager;
		CoarseSampledHeightFieldShape* m_Shape;
	};
}
