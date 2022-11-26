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
#include "Sim/GASSComponent.h"
#include "Sim/Interface/GASSICollisionComponent.h"

#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include <ode/ode.h>
#include "Core/Math/GASSAABox.h"
#include "Core/Math/GASSQuaternion.h"
#include "ODECollisionInfo.h"

namespace GASS
{
	class IGeometryComponent;
	class IHeightmapTerrainComponent;
	class ODECollisionSceneManager;
	using ODECollisionSceneManagerPtr = std::shared_ptr<ODECollisionSceneManager>;
	using GeometryComponentPtr = std::shared_ptr<IGeometryComponent>;
	using HeightmapTerrainComponentPtr = std::shared_ptr<IHeightmapTerrainComponent>;

	class ODECollisionGeometryComponent : public Reflection<ODECollisionGeometryComponent,Component>, ICollisionComponent
	{
		friend class ODECollisionSceneManager;
	public:
		friend class ODECollisionSystem;
		enum CollisionGeomType
		{
			CGT_MESH,
			CGT_TERRAIN,
			CGT_BOX,
			CGT_PLANE,
			CGT_NONE
		};

		ODECollisionGeometryComponent();
		~ODECollisionGeometryComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;

		//ICollisionComponent
		void SetActive(bool value) override;
		bool GetActive() const override;
	protected:

		static void CreateODERotationMatrix(const Mat4 &m, dReal *ode_mat);
		static void CreateGASSRotationMatrix(const dReal *ode_mat, Mat4 &m);

		//Message functions
		void OnGeometryChanged(GeometryChangedEventPtr message);
		void OnCollisionSettings(CollisionSettingsRequestPtr message);
		void OnTransformationChanged(TransformationChangedEventPtr message);
		void OnGeometryFlagsChanged(GeometryFlagsChangedEventPtr message);

		//Get set section
		
		std::string GetTypeByName() const;
		void SetTypeByName(const std::string &type);

		//helpers
		
		void CreateGeometry();
		void SetPosition(const Vec3 &pos);
		void SetRotation(const Quaternion &rot);
		void SetScale(const Vec3 &scale);
		bool IsInitialized() const;
		ODECollisionSceneManagerPtr GetCollisionSceneManager() const;
		dGeomID CreateMeshGeometry();
		dGeomID CreateTerrainGeometry();
		dGeomID CreateBoxGeometry();
		dGeomID CreatePlaneGeometry() const;
		void Reset();
		void Disable();
		void Enable();

		unsigned long GetFlags() const;
		void SetFlags(unsigned long value);
		
		void SetType(CollisionGeomType type) {m_Type= type;}
		CollisionGeomType GetType() const {return m_Type;}

		//Height map based Terrain helpers
		static dReal TerrainHeightCallback(void* data,int x,int z);	
		Float GetTerrainHeight(unsigned int x,unsigned int z);
	protected:
		dGeomID m_GeomID{nullptr};
		CollisionGeomType m_Type{CGT_NONE};
		Vec3 m_Offset;
		//Static Terrain data, only support one terrain loaded at the same time
		struct TerrainData
		{
			Float m_SampleWidth;
			Float m_SampleHeight;
			AABox m_TerrainBounds;
			IHeightmapTerrainComponent* m_TerrainGeom;
			int m_Samples;
		};
		TerrainData* m_TerrainData{nullptr};
		ODECollisionSceneManagerPtr m_CollisionSceneManager;
		ODECollisionMeshInfo m_ColMeshInfo;
	};
	using ODECollisionGeometryComponentPtr = std::shared_ptr<ODECollisionGeometryComponent>;
}
