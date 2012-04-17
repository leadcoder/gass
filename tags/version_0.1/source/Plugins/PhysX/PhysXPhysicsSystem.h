/****************************************************************************
*                                                                           *
* HiFiEngine                                                                *
* Copyright (C)2003 - 2005 Johan Hedstrom                                   *
* Email: hifiengine@gmail.com                                               *
* Web page: http://n00b.dyndns.org/HiFiEngine                               *
*                                                                           *
* HiFiEngine is only used with knowledge from the author. This software     *
* is not allowed to redistribute without permission from the author.        *
* For further license information, please turn to the product home page or  *
* contact author. Abuse against the HiFiEngine license is prohibited by law.*
*                                                                           *
*****************************************************************************/ 

#ifndef PHYS_X_PHYSICS_MANAGER
#define PHYS_X_PHYSICS_MANAGER

#include <map>
#include "NxPhysics.h"
#include "Core/MessageSystem/IMessage.h"
#include "Sim/Scenario/Scene/BaseSceneManager.h"
#include "Sim/Systems/SimSystem.h"

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