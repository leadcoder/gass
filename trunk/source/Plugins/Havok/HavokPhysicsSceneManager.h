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

#pragma once

#include <map>
#include "Core/MessageSystem/IMessage.h"
#include "Sim/Scenario/Scene/BaseSceneManager.h"
#include "Sim/Scenario/Scene/Messages/PhysicsSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/GraphicsSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/CoreSceneObjectMessages.h"
#include "Sim/Scenario/Scene/Messages/CoreScenarioSceneMessages.h"
#include "Sim/Scenario/Scene/Messages/PhysicsScenarioSceneMessages.h"
#include "Sim/Scheduling/TaskGroups.h"
#include "Sim/Scheduling/ITaskListener.h"

// Math and base include
#include <Common/Base/hkBase.h>
#include <Common/Base/System/hkBaseSystem.h>
#include <Common/Base/System/Error/hkDefaultError.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/Monitor/hkMonitorStream.h>
#include <Common/Base/Memory/System/hkMemorySystem.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>

/////////////////////////////////////////////////////////////////////////////////////////////// Dynamics includes
#include <Physics/Collide/hkpCollide.h>	
#include <Physics/Collide/Agent/ConvexAgent/SphereBox/hkpSphereBoxAgent.h>	
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>					
#include <Physics/Collide/Shape/Convex/Sphere/hkpSphereShape.h>				
#include <Physics/Collide/Dispatch/hkpAgentRegisterUtil.h>					

#include <Physics/Collide/Query/CastUtil/hkpWorldRayCastInput.h>			
#include <Physics/Collide/Query/CastUtil/hkpWorldRayCastOutput.h>			

#include <Physics/Dynamics/World/hkpWorld.h>								
#include <Physics/Dynamics/Entity/hkpRigidBody.h>							
#include <Physics/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>	

#include <Common/Base/Thread/Job/ThreadPool/Cpu/hkCpuJobThreadPool.h>
#include <Common/Base/Thread/Job/ThreadPool/Spu/hkSpuJobThreadPool.h>
#include <Common/Base/Thread/JobQueue/hkJobQueue.h>

/////////////////////////////////////////////////////////////////////////////////////////////// Visual Debugger includes
#include <Common/Visualize/hkVisualDebugger.h>
#include <Physics/Utilities/VisualDebugger/hkpPhysicsContext.h>				






namespace GASS
{
	class IMeshComponent;
	struct MeshData;

	
	class HavokPhysicsSceneManager  : public Reflection<HavokPhysicsSceneManager, BaseSceneManager> , public ITaskListener
	{
	public:
		HavokPhysicsSceneManager();
		virtual ~HavokPhysicsSceneManager();
		static void RegisterReflection();
		virtual void OnCreate();
		
		//ITaskListener interface
		void Update(double delta);
		TaskGroup GetTaskGroup() const;

		bool IsActive()const {return !m_Paused;}
		hkpWorld* GetWorld() const {return m_pPhysicsWorld;}
	protected:
		void OnLoad(LoadSceneManagersMessagePtr message);
		void OnUnload(UnloadSceneManagersMessagePtr message);
		void OnLoadSceneObject(SceneObjectCreatedNotifyMessagePtr message);
		void OnActivateMessage(ActivatePhysicsMessagePtr message);
		void SetGravity(float gravity);
		float GetGravity() const;
		void SetTaskGroup(TaskGroup value);
	private:
		float m_Gravity;
		bool m_Paused;
		TaskGroup m_TaskGroup;
		bool m_Init;
		double m_SimulationUpdateInterval;
		double m_TimeToProcess;
		int m_MaxSimSteps;
		bool m_visualDebuggerActive;

		//Variables for memory needs of HAVOK
		hkMemoryRouter* m_pMemoryRouter;
		hkHardwareInfo m_hardwareInfo;
		int m_iTotalNumThreadsUsed;
		hkCpuJobThreadPoolCinfo m_threadPoolCinfo;
		hkJobThreadPool* m_pThreadPool;
		hkJobQueueCinfo m_jobQueuInfo;
		hkJobQueue* m_pJobQueue;

	//World control variables
		hkpWorldCinfo m_worldInfo;
		hkpWorld* m_pPhysicsWorld;

	//Visual debugger
		bool m_bVisualDebuggerActive;
		hkArray<hkProcessContext*> m_arrayPhysicsContext;
		hkVisualDebugger* m_pVisualDebugger;
		hkpPhysicsContext* m_pPhysicsContext;
	};
	typedef boost::shared_ptr<HavokPhysicsSceneManager> HavokPhysicsSceneManagerPtr;
	typedef boost::weak_ptr<HavokPhysicsSceneManager> HavokPhysicsSceneManagerWeakPtr;
}

