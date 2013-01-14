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

#include <ode/ode.h>
#include <map>
#include "Core/MessageSystem/GASSIMessage.h"
#include "Sim/GASSBaseSceneManager.h"
#include "Sim/Interface/GASSICollisionSceneManager.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Messages/GASSPhysicsSceneObjectMessages.h"
#include "Sim/Messages/GASSCoreSceneMessages.h"
#include "Sim/Messages/GASSPhysicsSceneMessages.h"


namespace GASS
{
	class IMeshComponent;
	struct MeshData;
	typedef boost::shared_ptr<IMeshComponent> MeshComponentPtr;
	struct ODECollisionMesh
	{
		MeshData* Mesh;
		dTriMeshDataID ID;
	};

	class ODEPhysicsSceneManager  : public Reflection<ODEPhysicsSceneManager, BaseSceneManager> 
	{
	public:
		typedef std::map<std::string,ODECollisionMesh> CollisionMeshMap;
	public:
		ODEPhysicsSceneManager();
		virtual ~ODEPhysicsSceneManager();
		static void RegisterReflection();
		
		virtual void OnCreate();
		virtual void OnInit();
		virtual void OnShutdown();
		virtual bool GetSerialize() const {return false;}

		dWorldID GetWorld()const {return m_World;}
		void SystemTick(double delta);
		bool IsActive()const {return !m_Paused;}

		dSpaceID GetPhysicsSpace(){return m_Space;}
		dSpaceID GetCollisionSpace(){return m_CollisionSpace;}
		ODECollisionMesh CreateCollisionMesh(const std::string &col_mesh_id,MeshComponentPtr mesh);
		bool HasCollisionMesh(const std::string &name);
		void OnActivateMessage(ActivatePhysicsMessagePtr message);
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
		bool m_Paused;
		CollisionMeshMap m_ColMeshMap;
		bool m_Init;
		double m_SimulationUpdateInterval;
		double m_TimeToProcess;
		int m_MaxSimSteps;
		static bool m_ZUp;
	};
	typedef boost::shared_ptr<ODEPhysicsSceneManager> ODEPhysicsSceneManagerPtr;
	typedef boost::weak_ptr<ODEPhysicsSceneManager> ODEPhysicsSceneManagerWeakPtr;
	
	
}

