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



#include "Sim/GASSSimSystemManager.h"
#include "Sim/Scheduling/GASSTBBRuntimeController.h"
#include "Sim/Scheduling/GASSTBBUpdateTask.h"
#include "Sim/GASSSimEngine.h"


namespace GASS
{
	TBBRuntimeController::TBBRuntimeController()
	{

	}

	TBBRuntimeController::~TBBRuntimeController()
	{	

	}

	void TBBRuntimeController::Init(int num_threads)
	{
		//int nthread = tbb::task_scheduler_init::automatic;
		int  default_num_t = tbb::task_scheduler_init::default_num_threads();
		if(num_threads == -1)
			num_threads = default_num_t;
		
		m_Scheduler = new tbb::task_scheduler_init(num_threads);
		m_TasksRoot = new( tbb::task::allocate_root() ) tbb::empty_task;
		LogManager::getSingleton().stream() << "TBBRuntimeController initialized with "  << num_threads  << " threads";
		//tbb::tick_count ts = tbb::tick_count::now();
	}

	void TBBRuntimeController::Register(ITaskListener* listener)
	{
		if(listener->GetTaskGroup() == MAIN_TASK_GROUP)
		{
			m_PrimaryUpdateVector.push_back(listener);
		}
		else
		{
			tbb::spin_mutex::scoped_lock lock(m_Mutex);
			m_TaskGroups[listener->GetTaskGroup()].push_back(listener);
		}
	}

	void TBBRuntimeController::Unregister(ITaskListener* listener)
	{
		TaskGroup group =listener->GetTaskGroup();
		if(group == MAIN_TASK_GROUP)
		{
			TaskListenerVector::iterator iter = m_PrimaryUpdateVector.begin();
			while(iter != m_PrimaryUpdateVector.end())
			{
				if((*iter) == listener)
				{
					iter = m_PrimaryUpdateVector.erase(iter);
				}
				else
					++iter;
			}
			return;
		}
		else
		{
			tbb::spin_mutex::scoped_lock lock(m_Mutex);
			TaskListenerVector::iterator iter = m_TaskGroups[group].begin();
			while(iter != m_TaskGroups[group].end())
			{
				if((*iter) == listener)
				{
					iter = m_TaskGroups[group].erase(iter);
				}
				else
					++iter;
			}
		}


	}


	void TBBRuntimeController::Update(double delta_time)
	{
		m_TasksRoot->set_ref_count(1);

		TaskGroupMap groups;
		//mutex
		{
			//lock
			tbb::spin_mutex::scoped_lock lock(m_Mutex);
			groups = m_TaskGroups;
		}

		if(groups.size() > 0)
		{
			tbb::task_list task_list;

			TaskGroupMap::iterator iter = groups.begin();

			for(; iter != groups.end();++iter)
			{
				//	tbb::task* test = new( tbb::task::allocate_root() ) tbb::empty_task;
				TBBUpdateTask* update_task = new( m_TasksRoot->allocate_additional_child_of( *m_TasksRoot ) ) TBBUpdateTask(delta_time,iter->second);
				// affinity will increase the chances that each SystemTask will be assigned
				// to a unique thread, regardless of PerformanceHint
				//pSystemTask->set_affinity( m_affinityIDs[i % uAffinityCount] );

				//Memory leak??? should we delete update_task?
				task_list.push_back( *update_task);
			}


			m_TasksRoot->spawn( task_list);
		}

		//Update primary thread here
		for(int i = 0 ; i < m_PrimaryUpdateVector.size();i++)
		{
			m_PrimaryUpdateVector[i]->Update(delta_time);
		}

		//wait for all tasks
		if(groups.size() > 0)
			m_TasksRoot->wait_for_all();

		//Sync!
		//SimEngine::Get().GetSimSystemManager()->Update(delta_time);
	}


	void TBBRuntimeController::Log()
	{
		LogManager::getSingleton().stream() << "Num registred tasks in primary vector:" << m_PrimaryUpdateVector.size();
		LogManager::getSingleton().stream() << "Num registred groups:" << m_TaskGroups.size();

		TaskGroupMap::iterator iter = m_TaskGroups.begin();
		for(; iter != m_TaskGroups.end();++iter)
		{
			LogManager::getSingleton().stream() << "Num registred in"  <<  iter->first << ":" << iter->second.size();
		}
	}
}
