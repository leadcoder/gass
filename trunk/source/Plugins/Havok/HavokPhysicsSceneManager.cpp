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

//you need this. it creates pointer to characters which Havok Engine uses
//if you put this line into HavokUtilities.h you will get re-definition errors
//because all cpp files which includes HavokUtilities.h will try to create these character pointers in their .obj copies
//and this will create Linking redefinition problems
#include <Common/Base/keycode.cxx>

//You do not want to include the animation headers at this point
//if you comment this out it will give unresolved symbol errors for animation libraries
#ifdef HK_FEATURE_PRODUCT_ANIMATION
#undef HK_FEATURE_PRODUCT_ANIMATION
#endif
//I need to do that because of some memory linkings at compile time
//if you comment this out it will give unresolved symbol errors
#ifndef HK_EXCLUDE_LIBRARY_hkgpConvexDecomposition
#define HK_EXCLUDE_LIBRARY_hkgpConvexDecomposition
#endif

//this is for linking hkBase.lib
//if you comment this out it will give unresolved symbol errors for hkBase.lib
#include <Common/Base/Config/hkProductFeatures.cxx> 

//you want to include this for being able to define the following struct and function
#include <Physics/Collide/hkpCollide.h>	
//these lines are needed for getting rid of linker problems for hkpCollide.lib
//I could not find a better solution
//source for why I do this --> http://software.intel.com/en-us/forums/showthread.php?t=73886
struct hkTestEntry* hkUnitTestDatabase = HK_NULL; 
hkBool HK_CALL hkTestReport(hkBool32 cond, const char* desc, const char* file, int line) {return false;}

#include "Sim/GASS.h"
#include "Plugins/Havok/HavokPhysicsSceneManager.h"
#include "Plugins/Havok/HavokPhysicsSystem.h"


#include "Sim/Components/Physics/GASSIPhysicsGeometryComponent.h"

namespace GASS
{

	HavokPhysicsSceneManager::HavokPhysicsSceneManager() :	m_Paused(false),
		m_Gravity(-9.81f),
		m_SimulationUpdateInterval(1.0/60.0), //Locked to 60hz, if this value is changed the 
		//behavior of simulation is effected and values for 
		//bodies and joints must be re-trimmed
		m_TimeToProcess(0),
		m_MaxSimSteps(4)
	{

	}

	HavokPhysicsSceneManager::~HavokPhysicsSceneManager()
	{
	}

	void HavokPhysicsSceneManager::RegisterReflection()
	{
		SceneManagerFactory::GetPtr()->Register("PhysicsSceneManager",new GASS::Creator<HavokPhysicsSceneManager, ISceneManager>);
		RegisterProperty<float>("Gravity", &GASS::HavokPhysicsSceneManager::GetGravity, &GASS::HavokPhysicsSceneManager::SetGravity);
	}

	void HavokPhysicsSceneManager::SetGravity(float gravity)
	{
		m_Gravity = gravity;
	}

	float HavokPhysicsSceneManager::GetGravity() const
	{
		return m_Gravity;
	}

	void HavokPhysicsSceneManager::OnCreate()
	{
		GetScene()->RegisterForMessage(REG_TMESS(HavokPhysicsSceneManager::OnLoad,LoadSceneManagersMessage,0));
		GetScene()->RegisterForMessage(REG_TMESS(HavokPhysicsSceneManager::OnUnload,UnloadSceneManagersMessage,0));
		GetScene()->RegisterForMessage(REG_TMESS(HavokPhysicsSceneManager::OnLoadSceneObject,SceneObjectCreatedNotifyMessage,Scene::PHYSICS_COMPONENT_LOAD_PRIORITY));
		GetScene()->RegisterForMessage(REG_TMESS(HavokPhysicsSceneManager::OnActivateMessage,ActivatePhysicsMessage,0));
	}


	void HavokPhysicsSceneManager::OnActivateMessage(ActivatePhysicsMessagePtr message)
	{
		if(message->Activate())
			m_Paused = false;
		else 
			m_Paused = true;
	}

	void HavokPhysicsSceneManager::OnLoadSceneObject(SceneObjectCreatedNotifyMessagePtr message)
	{

	}

	void HavokPhysicsSceneManager::SystemTick(double delta_time)
	{

		if (!m_Paused)
		{

			//do some time slicing
			m_TimeToProcess += delta_time;
			int num_steps = (int) (m_TimeToProcess / m_SimulationUpdateInterval);
			int clamp_num_steps = num_steps;

			//Take max 4 simulation step each frame
			if(num_steps > m_MaxSimSteps) clamp_num_steps = m_MaxSimSteps;

			for (int i = 0; i < clamp_num_steps; ++i)
			{
			
				m_pPhysicsWorld->stepMultithreaded(m_pJobQueue, m_pThreadPool, m_SimulationUpdateInterval);
				//stepVisualDebugger(deltaTime);
				hkMonitorStream::getInstance().reset();
				m_pThreadPool->clearTimerData();		
			}
			//std::cout << "Steps:" <<  clamp_num_steps << std::endl;
			m_TimeToProcess -= m_SimulationUpdateInterval * num_steps;
		}
	}


	static void errorReport(const char* msg, void* errorReportObject)
	{
		printf( "%s", msg );
	}


	void HavokPhysicsSceneManager::OnLoad(LoadSceneManagersMessagePtr message)
	{
		ScenePtr scene = message->GetScene();

		HavokPhysicsSystemPtr system =  SimEngine::GetPtr()->GetSimSystemManager()->GetFirstSystem<HavokPhysicsSystem>();
		if(system == NULL)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to find HavokPhysicsSystem", "HavokPhysicsSceneManager::OnLoad");
		SystemListenerPtr listener = shared_from_this();

		system->Register(listener);

		Vec3 gravity_vec(0,m_Gravity,0);

		{//initialize Havok Memory
			// Allocate 0.5MB of physics solver buffer.
			//hkMemoryRouter* m_pMemoryRouter;
			m_pMemoryRouter = hkMemoryInitUtil::initDefault(hkMallocAllocator::m_defaultMallocAllocator, hkMemorySystem::FrameInfo(500000));
			hkBaseSystem::init( m_pMemoryRouter, errorReport );
		}
		{// Initialize the multi-threading classes, hkJobQueue, and hkJobThreadPool
			// Most of the comments are copied and pasted from ConsoleExampleMt.cpp of HavokDemos folder (2010 version)
			// They can be used for all Havok multithreading tasks. In this exmaple we only show how to use
			// them for physics, but you can reference other multithreading demos in the demo framework
			// to see how to multithread other products. The model of usage is the same as for physics.
			// The hkThreadpool has a specified number of threads that can run Havok jobs.  These can work
			// alongside the main thread to perform any Havok multi-threadable computations.
			// The model for running Havok tasks in Spus and in auxilary threads is identical.  It is encapsulated in the
			// class hkJobThreadPool.  On PlayStation(R)3 we initialize the SPU version of this class, which is simply a SPURS taskset.
			// On other multi-threaded platforms we initialize the CPU version of this class, hkCpuJobThreadPool, which creates a pool of threads
			// that run in exactly the same way.  On the PlayStation(R)3 we could also create a hkCpuJobThreadPool.  However, it is only
			// necessary (and advisable) to use one Havok PPU thread for maximum efficiency. In this case we simply use this main thread
			// for this purpose, and so do not create a hkCpuJobThreadPool.
			// Get the number of physical threads available on the system
			//hkHardwareInfo m_hardwareInfo;
			hkGetHardwareInfo(m_hardwareInfo);
			m_iTotalNumThreadsUsed = m_hardwareInfo.m_numThreads;

			// We use one less than this for our thread pool, because we must also use this thread for our simulation
			//hkCpuJobThreadPoolCinfo m_threadPoolCinfo;
			m_threadPoolCinfo.m_numThreads = m_iTotalNumThreadsUsed - 1;

			//RDS_PREVDEFINITIONS this line is from previous HavokWrapper
			//m_threadPoolCinfo.m_allocateRuntimeMemoryBlocks = true;
			// This line enables timers collection, by allocating 200 Kb per thread.  If you leave this at its default (0),
			// timer collection will not be enabled.
			m_threadPoolCinfo.m_timerBufferPerThreadAllocation = 200000;
			m_pThreadPool = new hkCpuJobThreadPool( m_threadPoolCinfo );

			// We also need to create a Job queue. This job queue will be used by all Havok modules to run multithreaded work.
			// Here we only use it for physics.
			m_jobQueuInfo.m_jobQueueHwSetup.m_numCpuThreads = m_iTotalNumThreadsUsed;
			m_pJobQueue = new hkJobQueue(m_jobQueuInfo);

			//
			// Enable monitors for this thread.
			//

			// Monitors have been enabled for thread pool threads already (see above comment).
			hkMonitorStream::getInstance().resize(200000);
		}

		{// <PHYSICS-ONLY>: Create the physics world.			
			// At this point you would initialize any other Havok modules you are using.
			// The world cinfo contains global simulation parameters, including gravity, solver settings etc.

			// Set the simulation type of the world to multi-threaded.
			m_worldInfo.m_simulationType = hkpWorldCinfo::SIMULATION_TYPE_MULTITHREADED;

			// Flag objects that fall "out of the world" to be automatically removed - just necessary for this physics scene
			// In other words, objects that fall "out of the world" will be automatically removed
			//m_worldInfo.m_broadPhaseBorderBehaviour = hkpWorldCinfo::BROADPHASE_BORDER_REMOVE_ENTITY;
			m_worldInfo.m_broadPhaseBorderBehaviour = hkpWorldCinfo::BROADPHASE_BORDER_DO_NOTHING;

			//RDS_HARDCODED values here --> just for me to see if I can make this part better
			//standard gravity settings, collision tolerance and world size 
			m_worldInfo.m_gravity.set(0,-9.8f,0);
			//I do not know what the next line does. For this demo it doesnt change anything if/if not having this line enabled
			//m_worldInfo.setupSolverInfo(hkpWorldCinfo::SOLVER_TYPE_8ITERS_MEDIUM);
			m_worldInfo.m_collisionTolerance = 0.1f;
			//This will effect the removal of objects when they fall out of the world.
			//If you have BROADPHASE_BORDER_REMOVE_ENTITY then your entities will be removed from Havok according to this number you set
			m_worldInfo.setBroadPhaseWorldSize(15000.0f);

			//initialize world with created info
			m_pPhysicsWorld = new hkpWorld(m_worldInfo);

			// RDS_PREVDEFINITIONS this line is from another HavokWrapper. Helps when using VisualDebugger
			// Disable deactivation, so that you can view timers in the VDB. This should not be done in your game.
			m_pPhysicsWorld->m_wantDeactivation = false;


			// When the simulation type is SIMULATION_TYPE_MULTITHREADED, in the debug build, the sdk performs checks
			// to make sure only one thread is modifying the world at once to prevent multithreaded bugs. Each thread
			// must call markForRead / markForWrite before it modifies the world to enable these checks.
			m_pPhysicsWorld->markForWrite();

			// Register all collision agents, even though only box - box will be used in this particular example.
			// It's important to register collision agents before adding any entities to the world.
			hkpAgentRegisterUtil::registerAllAgents( m_pPhysicsWorld->getCollisionDispatcher() );

			// We need to register all modules we will be running multi-threaded with the job queue
			m_pPhysicsWorld->registerWithJobQueue( m_pJobQueue );

			// Now we have finished modifying the world, release our write marker.
			m_pPhysicsWorld->unmarkForWrite();
		}

		{//RDS Begin --> you can use such a way to enable VisualDebugger
			//so that you  do not need commenting out some lines all the time you change it
#ifdef HAVOK_VISUAL_DEBUGGER_ENABLED
			registerVisualDebugger();
#endif	
		}//RDS End

		m_Init = true;
	}

	void HavokPhysicsSceneManager::OnUnload(UnloadSceneManagersMessagePtr message)
	{
		
	}
}
