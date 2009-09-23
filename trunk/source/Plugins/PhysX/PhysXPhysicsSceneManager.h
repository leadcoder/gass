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

#include <map>
#include "Core/MessageSystem/Message.h"
#include "Sim/Scenario/Scene/BaseSceneManager.h"
#include "Sim/Scheduling/TaskGroups.h"
#include "Sim/Scheduling/ITaskListener.h"

class NxTriangleMesh;
class NxScene;

namespace GASS
{
	class IMeshComponent;
	struct MeshData;

	struct PhysXCollisionMesh
	{
		MeshData* Mesh;
		NxTriangleMesh* NxMesh;
	};

	class PhysXPhysicsSceneManager  : public Reflection<PhysXPhysicsSceneManager, BaseSceneManager> , public ITaskListener
	{
	public:
		typedef std::map<std::string,PhysXCollisionMesh> CollisionMeshMap;
	public:
		PhysXPhysicsSceneManager();
		virtual ~PhysXPhysicsSceneManager();
		static void RegisterReflection();
		virtual void OnCreate();
		TaskGroup GetTaskGroup() const;
		NxScene* GetNxScene() {return m_NxScene;}
	protected:
		void Update(double delta);
		void OnLoad(MessagePtr message);
		void OnUnload(MessagePtr message);
		void OnLoadSceneObject(MessagePtr message);
		void SetGravity(float gravity);
		float GetGravity() const;
		void SetTaskGroup(TaskGroup value);
	private:
		float m_Gravity;
		bool m_Paused;
		TaskGroup m_TaskGroup;
		CollisionMeshMap m_ColMeshMap;
		bool m_Init;
		double m_SimulationUpdateInterval;
		double m_TimeToProcess;
		int m_MaxSimSteps;
		NxScene *m_NxScene;
	};
}

