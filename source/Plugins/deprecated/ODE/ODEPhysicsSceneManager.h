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
#include <ode/ode.h>
#include "Sim/GASSBaseSceneManager.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneMessages.h"

namespace GASS
{
	class IMeshComponent;
	class PhysicsMesh;
	using MeshComponentPtr = std::shared_ptr<IMeshComponent>;
	using PhysicsMeshPtr = std::shared_ptr<PhysicsMesh>;
	
	struct ODEPhysicsCollisionMesh
	{
		PhysicsMeshPtr Mesh;
		dTriMeshDataID ID;
	};

	class ODEPhysicsSceneManager  : public Reflection<ODEPhysicsSceneManager, BaseSceneManager> 
	{
	public:
		using CollisionMeshMap = std::map<std::string, ODEPhysicsCollisionMesh>;
	public:
		ODEPhysicsSceneManager(SceneWeakPtr scene);
		~ODEPhysicsSceneManager() override;
		static void RegisterReflection();
		
		void OnPostConstruction() override;
		void OnSceneCreated() override;
		void OnSceneShutdown() override;
		bool GetSerialize() const override {return false;}

		//IPhysicsSceneManager
		virtual void SetActive(bool value) { m_Active = value; }
		virtual bool GetActive() const { return m_Active; }

		dWorldID GetWorld()const {return m_World;}
		void OnUpdate(double delta) override;
	
		dSpaceID GetPhysicsSpace() const {return m_Space;}
		dSpaceID GetCollisionSpace() const {return m_CollisionSpace;}
		ODEPhysicsCollisionMesh CreateCollisionMesh(const std::string &col_mesh_id,MeshComponentPtr mesh);
		bool HasCollisionMesh(const std::string &name);
		void SetGravity(float gravity);
		float GetGravity() const;
		static void CreateODERotationMatrix(const Mat4 &m, dReal *ode_mat);
		static void CreateGASSRotationMatrix(const dReal *ode_mat, Mat4 &m);
	protected:
		static void NearCallback (void *data, dGeomID o1, dGeomID o2);
		void ProcessCollision(dGeomID o1, dGeomID o2);
	private:
		dWorldID m_World;
		dSpaceID m_Space;
		dSpaceID m_CollisionSpace;
		dJointGroupID m_ContactGroup;
		float m_Gravity;
		bool m_Active;
		CollisionMeshMap m_ColMeshMap;
		bool m_Init;
		double m_SimulationUpdateInterval;
		double m_TimeToProcess;
		int m_MaxSimSteps;
		static bool m_ZUp;
	};
	using ODEPhysicsSceneManagerPtr = std::shared_ptr<ODEPhysicsSceneManager>;
	using ODEPhysicsSceneManagerWeakPtr = std::weak_ptr<ODEPhysicsSceneManager>;
	
	
}

