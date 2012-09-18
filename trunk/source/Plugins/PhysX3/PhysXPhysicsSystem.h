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

#ifndef PHYS_X_PHYSICS_SYSTEM
#define PHYS_X_PHYSICS_SYSTEM

#include "PhysXCommon.h"


namespace physx
{
	class PxPhysics;
}
namespace GASS
{
	class IMeshComponent;
	struct MeshData;

	class PhysXPhysicsSystem : public Reflection<PhysXPhysicsSystem, SimSystem>
	{
	public:
//		typedef std::map<std::string,NxCollisionMesh> CollisionMeshMap;
	public:
		PhysXPhysicsSystem();
		virtual ~PhysXPhysicsSystem();
		static void RegisterReflection();
		virtual void OnCreate();
		physx::PxPhysics* GetPxSDK() const {return m_PhysicsSDK;}
		physx::PxMaterial* GetDefaultMaterial() const {return m_DefaultMaterial;}
		physx::PxDefaultAllocator* GetAllocator() {return &m_DefaultAllocator;}
	protected:
		void OnInit(InitSystemMessagePtr message);
		void OnShutdown(MessagePtr message);
	private:
		// Physics
		physx::PxPhysics* m_PhysicsSDK;
		physx::PxFoundation* m_Foundation;
		physx::PxMaterial* m_DefaultMaterial;
		physx::PxDefaultAllocator m_DefaultAllocator;
		//CollisionMeshMap m_ColMeshMap;
		//NxScene* m_Scene;
	};

	typedef boost::shared_ptr<PhysXPhysicsSystem> PhysXPhysicsSystemPtr;
}

#endif