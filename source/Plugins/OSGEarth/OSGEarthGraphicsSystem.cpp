#include "OSGEarthGraphicsSystem.h"
#include "OSGEarthCommonIncludes.h"

namespace GASS
{
	OSGEarthGraphicsSystem::OSGEarthGraphicsSystem(SimSystemManagerWeakPtr manager) : Reflection(manager)
	{
		m_UpdateGroup = UGID_PRE_SIM;
	}

	OSGEarthGraphicsSystem::~OSGEarthGraphicsSystem()
	{

	}

	void OSGEarthGraphicsSystem::RegisterReflection()
	{
		SystemFactory::GetPtr()->Register<OSGEarthGraphicsSystem>("OSGEarthGraphicsSystem");
	}

	void OSGEarthGraphicsSystem::OnSystemInit()
	{
		osgEarth::initialize();
		//SceneManagerFactory::GetPtr()->Register<OSGCollisionSceneManager>("OSGCollisionSceneManager");
	}
}
