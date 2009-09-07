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
#include "Core/MessageSystem/Message.h"
#include "Sim/Scenario/Scene/BaseSceneManager.h"
#include "Sim/Scheduling/TaskGroups.h"
#include "Sim/Scheduling/ITaskListener.h"

namespace Ogre
{
	class VertexData;
}
namespace GASS
{
	class IMeshComponent;
	struct MeshData;

	struct ODECollisionMesh
	{
		MeshData* Mesh;
		dTriMeshDataID ID;
	};

	class ODEPhysicsSceneManager  : public Reflection<ODEPhysicsSceneManager, BaseSceneManager> , public ITaskListener
	{
	public:
		typedef std::map<std::string,ODECollisionMesh> CollisionMeshMap;
	public:
		ODEPhysicsSceneManager();
		virtual ~ODEPhysicsSceneManager();
		static void RegisterReflection();
		virtual void OnCreate();
		dSpaceID GetPhysicsSpace(){return m_Space;}
		dSpaceID GetCollisionSpace(){return m_CollisionSpace;}
		ODECollisionMesh CreateCollisionMesh(IMeshComponent* mesh);
		bool HasCollisionMesh(const std::string &name);
		static void CreateODERotationMatrix(const Mat4 &m, dReal *ode_mat);
		static void CreateGASSRotationMatrix(const dReal *ode_mat, Mat4 &m);
		dWorldID GetWorld()const {return m_World;}

		//ITaskListener interface
		void Update(double delta);
		TaskGroup GetTaskGroup() const;

	protected:
		void OnLoad(MessagePtr message);
		void OnUnload(MessagePtr message);
		void OnLoadSceneObject(MessagePtr message);
		void SetGravity(float gravity);
		float GetGravity() const;
		void SetTaskGroup(TaskGroup value);
		static void NearCallback (void *data, dGeomID o1, dGeomID o2);
		void ProcessCollision(dGeomID o1, dGeomID o2);
	private:
		dWorldID m_World;
		dSpaceID m_Space;
		dSpaceID m_StaticSpace;
		dSpaceID m_CollisionSpace;
		dJointGroupID m_ContactGroup;
		float m_Gravity;
		bool m_Paused;
		TaskGroup m_TaskGroup;
		CollisionMeshMap m_ColMeshMap;
		bool m_Init;
		double m_SimulationUpdateInterval;
		double m_TimeToProcess;
		int m_MaxSimSteps;
	};
}

