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

#ifndef PHYS_X_PHYSICS_MANAGER
#define PHYS_X_PHYSICS_MANAGER

#include <map>
#include "PxPhysics.h"
#include "Sim/GASS.h"

class NxPhysicsSDK;

namespace GASS
{
	class IMeshComponent;
	struct MeshData;

	struct NxCollisionMesh
	{
		MeshData* Mesh;
		NxTriangleMesh* NxMesh;
	};

	class PhysXPhysicsSystem : public Reflection<PhysXPhysicsSystem, SimSystem>
	{
	public:
		typedef std::map<std::string,NxCollisionMesh> CollisionMeshMap;
	public:
		PhysXPhysicsSystem();
		virtual ~PhysXPhysicsSystem();
		NxPhysicsSDK* GetNxSDK() {return m_PhysicsSDK;}
		//NxTriangleMesh* GetTriMesh(CollisionMesh *cm);
		static void RegisterReflection();
		virtual void OnCreate();

		NxCollisionMesh CreateCollisionMesh(IMeshComponent* mesh);
		bool HasCollisionMesh(const std::string &name);

	protected:
		void OnInit(MessagePtr message);
		void OnShutdown(MessagePtr message);

	private:
		// Physics
		NxPhysicsSDK* m_PhysicsSDK;
		CollisionMeshMap m_ColMeshMap;
		//NxScene* m_Scene;
	};

	typedef boost::shared_ptr<PhysXPhysicsSystem> PhysXPhysicsSystemPtr;
}

#endif