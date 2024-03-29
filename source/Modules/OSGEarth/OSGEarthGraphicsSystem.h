#pragma once

#include "Sim/GASS.h"
#include "Modules/OSG/OSGGraphicsSystem.h"

namespace GASS
{
	/**
		Collision system implementation that use osg for intersection test
	*/
	class OSGEarthGraphicsSystem : public Reflection<OSGEarthGraphicsSystem, OSGGraphicsSystem>
	{
	public:
		static void RegisterReflection();
		OSGEarthGraphicsSystem(SimSystemManagerWeakPtr manager);
		~OSGEarthGraphicsSystem() override;
		void OnSystemInit() override;
		std::string GetSystemName() const override {return "OSGEarthGraphicsSystem";}
		SceneObjectPtr CreateDefaultCamera() const override;
	private:
		void RegisterSceneManager() override;
	};
	using OSGEarthGraphicsSystemPtr = std::shared_ptr<OSGEarthGraphicsSystem>;
}

