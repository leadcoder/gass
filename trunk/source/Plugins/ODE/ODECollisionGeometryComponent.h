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

#include "Sim/Components/GASSBaseSceneComponent.h"
#include "Sim/Scene/GASSCoreSceneObjectMessages.h"
#include "Sim/Scene/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Scene/GASSPhysicsSceneObjectMessages.h"
#include <ode/ode.h>
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Math/GASSAABox.h"
#include "Core/Math/GASSQuaternion.h"
#include "ODEPhysicsSceneManager.h"
#include "Sim/Components/Physics/GASSIPhysicsGeometryComponent.h"

namespace GASS
{
	class IGeometryComponent;
	class ITerrainComponent;
	class ODECollisionSystem;
	typedef boost::shared_ptr<ODECollisionSystem> ODECollisionSystemPtr;
	typedef boost::shared_ptr<IGeometryComponent> GeometryComponentPtr;
	typedef boost::shared_ptr<ITerrainComponent> TerrainComponentPtr;

	class ODECollisionGeometryComponent : public Reflection<ODECollisionGeometryComponent,BaseSceneComponent>
	{
		
	public:
		friend class ODECollisionSystem;
		enum CollisionGeomType
		{
			CGT_MESH,
			CGT_TERRAIN,
			CGT_BOX,
			CGT_NONE
		};

		ODECollisionGeometryComponent();
		virtual ~ODECollisionGeometryComponent();
		static void RegisterReflection();
		virtual void OnInitialize();
	protected:
		//Message functions
		void OnGeometryChanged(GeometryChangedMessagePtr message);
		void OnUnload(UnloadComponentsMessagePtr message);
		void OnCollisionSettings(CollisionSettingsMessagePtr message);
		void OnTransformationChanged(TransformationNotifyMessagePtr message);
		void OnGeometryScale(GeometryScaleMessagePtr message);

		//helpers
		void CreateGeometry();
		void SetPosition(const Vec3 &pos);
		void SetRotation(const Quaternion &rot);
		void SetScale(const Vec3 &scale);
		bool IsInitialized() const;
		ODECollisionSystemPtr GetCollisionSystem() const;
		dGeomID CreateMeshGeometry();
		dGeomID CreateTerrainGeometry();
		dGeomID CreateBoxGeometry();
	
		void Reset();
		void Disable();
		void Enable();
		unsigned long GetCollisionBits() const;
		void SetCollisionBits(unsigned long value);
		unsigned long GetCollisionCategory() const;
		void SetCollisionCategory(unsigned long value);
		void SetType(CollisionGeomType type) {m_Type= type;}
		CollisionGeomType GetType() const {return m_Type;}

		//Height map based Terrain helpers
		TerrainComponentPtr GetTerrainComponent() const;
		static dReal TerrainHeightCallback(void* data,int x,int z);	
		Float GetTerrainHeight(unsigned int x,unsigned int z);
	protected:


		dGeomID m_GeomID;
		unsigned long m_CollisionCategory;
		unsigned long m_CollisionBits;
		CollisionGeomType m_Type;
		Vec3 m_Offset;

		//Static Terrrain data, only support one terrain loaded at the same time
		struct TerrainData
		{
			Float m_SampleWidth;
			Float m_SampleHeight;
			AABox m_TerrainBounds;
			ITerrainComponent* m_TerrainGeom;
		};
		static TerrainData m_TerrainData;

	};

	typedef boost::shared_ptr<ODECollisionGeometryComponent> ODECollisionGeometryComponentPtr;
}
