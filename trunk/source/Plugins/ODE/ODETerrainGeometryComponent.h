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
#include "Sim/Components/Physics/IPhysicsGeometryComponent.h"
#include <ode/ode.h>
#include "Core/MessageSystem/IMessage.h"
#include "Core/Math/AABox.h"
#include "Core/Math/Quaternion.h"
#include "ODEPhysicsSceneManager.h"
#include "ODEBaseGeometryComponent.h"


namespace GASS
{
	class ITerrainComponent;
	class IGeometryComponent;
	class ODEBodyComponent;
	class ODEPhysicsSceneManager;
	typedef boost::weak_ptr<ODEPhysicsSceneManager> ODEPhysicsSceneManagerWeakPtr;
	typedef boost::shared_ptr<IGeometryComponent> GeometryComponentPtr;
	typedef boost::shared_ptr<ITerrainComponent> TerrainComponentPtr;
	

	class ODETerrainGeometryComponent : public Reflection<ODETerrainGeometryComponent,BaseSceneComponent> , public IPhysicsGeometryComponent
	{
	friend class ODEPhysicsSceneManager;
	public:
		ODETerrainGeometryComponent();
		virtual ~ODETerrainGeometryComponent();
		static void RegisterReflection();
		virtual void OnCreate();
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
		dGeomID CreateODEGeom();
		void Disable();
		void Enable();
		void Reset();
		dSpaceID GetSpace();
		dGeomID CreateTerrain();
		TerrainComponentPtr GetTerrainComponent() const;
		static dReal TerrainHeightCallback(void* data,int x,int z);	
		Float GetTerrainHeight(unsigned int x,unsigned int z);
	protected:
		Float m_SampleWidth;
		
		Float m_SampleHeight;
		AABox m_TerrainBounds;
		ITerrainComponent* m_TerrainGeom;
		long int m_CollisionCategory;
		long int m_CollisionBits;
		dGeomID m_GeomID;
		dSpaceID m_SpaceID;
		std::string m_GeometryTemplate;
		float m_Friction;
		bool m_Debug;
		ODEPhysicsSceneManagerWeakPtr m_SceneManager;
	};
}
