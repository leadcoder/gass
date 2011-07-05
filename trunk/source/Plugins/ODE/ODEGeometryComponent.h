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
#include "Sim/Components/Physics/IPhysicsGeometryComponent.h"

namespace GASS
{
	class ITerrainComponent;
	class IGeometryComponent;
	class ODEBodyComponent;
	class ODEPhysicsSceneManager;
	typedef boost::weak_ptr<ODEPhysicsSceneManager> ODEPhysicsSceneManagerWeakPtr;
	typedef boost::shared_ptr<IGeometryComponent> GeometryComponentPtr;


	class ODEGeometryComponent : public Reflection<ODEGeometryComponent,BaseSceneComponent> , public IPhysicsGeometryComponent
	{
	friend class ODEPhysicsSceneManager;
	public:
		enum PhysicsGeometryType
		{
			PGT_MESH,
			PGT_BOX,
			PGT_TERRAIN,
			PGT_CYLINDER,
			PGT_SPHERE,
			PGT_PLANE
		};

		ODEGeometryComponent();
		virtual ~ODEGeometryComponent();
		static void RegisterReflection();
		virtual void OnCreate();

		
	protected:
		void Disable();
		void Enable();
		void SetScale(const Vec3 &value);
		void SetFriction(float value){m_Friction = value;}
		float GetFriction() const {return m_Friction;}
		void Reset();
		void OnGeometryChanged(GeometryChangedMessagePtr message);
		void SetSizeFromGeom(dGeomID id, GeometryComponentPtr geom);
		void SetPosition(const Vec3 &pos);
		void SetRotation(const Quaternion &rot);
		void OnTransformationChanged(TransformationNotifyMessagePtr message);
		void OnLoad(LoadPhysicsComponentsMessagePtr message);
		void OnCollisionSettings(CollisionSettingsMessagePtr message);
		void CreateODEGeomFromGeom(IGeometryComponent* geom, 
											dSpaceID space,
											dGeomID &id_vector,
											dGeomID &transform_id_vector, 
											ODEBodyComponent* body);
		void CreateODEMassFromGeom(IGeometryComponent* geom,ODEBodyComponent* body);
		dSpaceID GetStaticSpace();
		dSpaceID GetSecondaryStaticSpace();
		void SetScale(const Vec3 &value, dGeomID id);
		void SetOffset(const Vec3 &value){m_Offset = value;}
		Vec3 GetOffset() const {return m_Offset;}
		void SetSlip(float value){m_Friction = value;}
		float GetSlip() const {return m_Friction;}
		void SetGeometryType(const std::string &geom_type);
		std::string GetGeometryType() const;
		dGeomID CreateTerrain(IGeometryComponent* geom, dSpaceID space);

		static dReal TerrainHeightCallback(void* data,int x,int z);	
		Float GetTerrainHeight(unsigned int x,unsigned int z);
		
		long int GetCollisionBits() const;
		void SetCollisionBits(long int value);

		long int GetCollisionCategory() const;
		void SetCollisionCategory(long int value);

		void SetBaseScale(const Vec3 &value) {m_CollisionGeomScale = value;}
		Vec3 GetBaseScale() const {return m_CollisionGeomScale;}
		
		//debug functions
		void CreateDebugBox(const Vec3 &size,const Vec3 &offset);
		void CreateDebugSphere(float size,const Vec3 &offset);
		SceneObjectPtr GetDebugObject();
		void UpdateDebug(bool enable);
		void OnPhysicsDebug(PhysicsDebugMessagePtr message);
		void OnGeometryScale(GeometryScaleMessagePtr message);
	protected:
		dGeomID m_GeomID;
		dGeomID m_TransformGeomID;
		dGeomID m_SecondGeomID;
		dGeomID m_SecondTransformGeomID;
		dSpaceID m_ODESpaceID;
		dSpaceID m_ODESecondarySpaceID;
		ODEBodyComponent* m_Body;
		Vec3 m_BBoxSize; // bounding box start size
		float m_BSphereSize; // bounding sphere start size
		
		PhysicsGeometryType m_PhysicsGeometryType;
		std::string m_GeometryTemplate;
		std::string m_AddToBody;
		Vec3 m_Offset;
		Vec3 m_BBOffset;
		Vec3 m_CollisionGeomScale;
		float m_Friction;
		float m_Slip;
		ODEPhysicsSceneManagerWeakPtr m_SceneManager;

		//Terrain data
		Float m_SampleWidth;
		Float m_SampleHeight;
		AABox m_TerrainBounds;
		ITerrainComponent* m_TerrainGeom;
		long int m_CollisionCategory;
		long int m_CollisionBits;
	};
}
