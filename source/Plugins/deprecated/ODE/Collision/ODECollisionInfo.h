#pragma once
#include <ode/ode.h>


namespace GASS
{
	class PhysicsMesh;
	typedef GASS_SHARED_PTR<PhysicsMesh> PhysicsMeshPtr;

	struct ODECollisionMeshInfo
	{
		PhysicsMeshPtr Mesh;
		dTriMeshDataID ID;
	};
}
