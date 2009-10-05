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

namespace GASS
{
	class ITerrainComponent;
	class IGeometryComponent;
	class ODEBody;
	class ODEPhysicsSceneManager;
	class ODEGeometry : public Reflection<ODEGeometry,BaseSceneComponent>
	{
	public:
		enum PhysicsGeometryType
		{
			PGT_MESH,
			PGT_BOX,
			PGT_TERRAIN,
			PGT_CYLINDER,
			PGT_SPHERE
		};

		ODEGeometry();
		virtual ~ODEGeometry();
		static void RegisterReflection();
		virtual void OnCreate();

		virtual void Disable();
		virtual void Enable();
		virtual void SetScale(const Vec3 &value);
		void SetFriction(float value){m_Friction = value;}
		float GetFriction() const {return m_Friction;}
	protected:
		
		void SetPosition(const Vec3 &pos);
		void SetRotation(const Quaternion &rot);

		void OnTransformationChanged(TransformationNotifyMessagePtr message);
		//void OnRotationChanged(MessagePtr message);
		void OnLoad(LoadPhysicsComponentsMessagePtr message);
		void OnCollisionSettings(CollisionSettingsMessagePtr message);
		void CreateODEGeomFromGeom(IGeometryComponent* geom, 
											dSpaceID space,
											dGeomID &id_vector,
											dGeomID &transform_id_vector, 
											ODEBody* body);
		void CreateODEMassFromGeom(IGeometryComponent* geom,ODEBody* body);
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
		float GetTerrainHeight(unsigned int x,unsigned int z);

	protected:
		
		dGeomID m_GeomID;
		dGeomID m_TransformGeomID;
		dGeomID m_SecondGeomID;
		dGeomID m_SecondTransformGeomID;
		dSpaceID m_ODESpaceID;
		dSpaceID m_ODESecondarySpaceID;
		ODEBody* m_Body;
		Vec3 m_BBSize; // bounding box start size
		float m_BSSize; // bounding sphere start size
		
		PhysicsGeometryType m_GeometryType;
		std::string m_GeometryTemplate;
		std::string m_AddToBody;
		Vec3 m_Offset;
		Vec3 m_CollisionGeomScale;
		float m_Friction;
		float m_Slip;
		ODEPhysicsSceneManager* m_SceneManager;

		//Terrain data
		float m_SampleWidth;
		float m_SampleHeight;
		AABox m_TerrainBounds;
		ITerrainComponent* m_TerrainGeom;
	};
}
