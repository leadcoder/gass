#pragma once
#include <ode/ode.h>


namespace GASS
{
	class PhysicsMesh;
	typedef SPTR<PhysicsMesh> PhysicsMeshPtr;

	struct ODECollisionMeshInfo
	{
		PhysicsMeshPtr Mesh;
		dTriMeshDataID ID;
	};
}
