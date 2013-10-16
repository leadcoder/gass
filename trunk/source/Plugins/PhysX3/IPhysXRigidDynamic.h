#pragma once
#include "Sim/GASSCommon.h"

namespace physx
{
	class PxRigidDynamic; 
}

namespace GASS
{

	/**
		Interface that can be used by all physx components that want to expose actors,
		PhysXBodyComponent is a  obvious candidate another is the PhysXVehicleComponent.
		This interface is for example used by the PhysXHingeComponent to connect actors
	*/
	class IPhysXRigidDynamic
	{
	public:
		IPhysXRigidDynamic(){}
		virtual ~IPhysXRigidDynamic(){}
		virtual physx::PxRigidDynamic* GetPxRigidDynamic() const  = 0;
	protected:
	private:
	};
	typedef SPTR<IPhysXRigidDynamic> PhysXBodyPtr;
}