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
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Interface/GASSIPhysicsGeometryComponent.h"
#include <ode/ode.h>
#include "Core/Math/GASSAABox.h"
#include "ODEPhysicsSceneManager.h"
#include "IODEGeometryComponent.h"


namespace GASS
{
	class IHeightmapTerrainComponent;
	class IGeometryComponent;
	class ODEBodyComponent;
	class ODEPhysicsSceneManager;
	using ODEPhysicsSceneManagerWeakPtr = std::weak_ptr<ODEPhysicsSceneManager>;
	using GeometryComponentPtr = std::shared_ptr<IGeometryComponent>;
	using HeightmapTerrainComponentPtr = std::shared_ptr<IHeightmapTerrainComponent>;


	class ODETerrainGeometryComponent : public Reflection<ODETerrainGeometryComponent,Component> , public IPhysicsGeometryComponent, public IODEGeometryComponent
	{
	friend class ODEPhysicsSceneManager;
	public:
		ODETerrainGeometryComponent();
		~ODETerrainGeometryComponent() override;
		static void RegisterReflection();
		void OnInitialize() override;
		void OnDelete() override;
	protected:
		
		void OnCollisionSettings(CollisionSettingsRequestPtr message);
		void OnGeometryChanged(GeometryChangedEventPtr message);
		void SetFriction(float value){m_Friction = value;}
		float GetFriction() const override {return m_Friction;}
		unsigned long GetCollisionBits() const;
		void SetCollisionBits(unsigned long value);
		unsigned long GetCollisionCategory() const;
		void SetCollisionCategory(unsigned long value);
		void Disable();
		void Enable();
		void Reset();
		dSpaceID GetSpace();
		dGeomID CreateTerrain();
		HeightmapTerrainComponentPtr GetTerrainComponent() const;
		static dReal TerrainHeightCallback(void* data,int x,int z);	
		Float GetTerrainHeight(unsigned int x,unsigned int z);
	protected:
		Float m_SampleWidth{0};
		//int m_Samples;
		Float m_SampleHeight{0};
		AABox m_TerrainBounds;
		IHeightmapTerrainComponent* m_TerrainGeom{nullptr};
		unsigned long m_CollisionCategory{1};
		unsigned long m_CollisionBits{1};
		dGeomID m_GeomID{nullptr};
		dSpaceID m_SpaceID{nullptr};
		std::string m_GeometryTemplate;
		float m_Friction{1};
		bool m_Debug{false};
		ODEPhysicsSceneManagerWeakPtr m_SceneManager;
	};
}
