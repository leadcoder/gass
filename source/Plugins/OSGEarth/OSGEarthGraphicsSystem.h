#pragma once

#include "Sim/GASS.h"

namespace GASS
{
	/**
		Collision system implementation that use osg for intersection test
	*/
	class OSGEarthGraphicsSystem : public Reflection<OSGEarthGraphicsSystem, SimSystem>
	{
	public:
		static void RegisterReflection();
		OSGEarthGraphicsSystem(SimSystemManagerWeakPtr manager);
		~OSGEarthGraphicsSystem() override;
		void OnSystemInit() override;
		std::string GetSystemName() const override {return "OSGEarthGraphicsSystem";}
	private:
	};
	typedef GASS_SHARED_PTR<OSGEarthGraphicsSystem> OSGCollisionSystemPtr;
}

