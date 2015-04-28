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


#include "Core/RTC/GASSTaskNode2.h"
#include "Core/RTC/GASSRunTimeController2.h"
#include "Core/Utils/GASSException.h"
#include "Core/Utils/GASSStringUtils.h"
#include <tinyxml2.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/spin_mutex.h>

namespace GASS
{
	TaskNode2::TaskNode2(RunTimeController2* rtc, int id) : m_ListenerMode(SEQUENCE),
		m_ChildrenMode(SEQUENCE),
		m_TimeToProcess(0),
		m_UpdateFrequency(0),
		m_RTC(rtc),
		m_Paused(false),
		m_ID(id),
		m_MaxSimulationSteps(-1),
		m_Mutex(new tbb::spin_mutex)
	{

	}

	TaskNode2::~TaskNode2()
	{
		delete m_Mutex;
	}


	bool TaskNodeSortPredicate(const TaskNode2Ptr &lhs, const TaskNode2Ptr &rhs)
	{
		return lhs->GetID() < rhs->GetID();
	}

	void TaskNode2::AddChildNode(TaskNode2Ptr child) 
	{
		tbb::spin_mutex::scoped_lock lock(*m_Mutex);
		m_Children.push_back(child);

		//SORT by ID
		std::sort(m_Children.begin(), m_Children.end(), TaskNodeSortPredicate);
	}

	/*bool TaskNodeListenerSortPredicate(const TaskNodeListener2Ptr &lhs, const TaskNodeListener2Ptr &rhs)
	{
		return lhs->GetPriority() < rhs->GetPriority();
	}
	*/

	void TaskNode2::Register(TaskNodeListener2Ptr listener)
	{
		tbb::spin_mutex::scoped_lock lock(*m_Mutex);

		m_Listeners.push_back(listener);
		//std::sort(m_Listeners.begin(), m_Listeners.end(), TaskNodeListenerSortPredicate);
	}

	void TaskNode2::Unregister(TaskNodeListener2Ptr listener)
	{
		tbb::spin_mutex::scoped_lock lock(*m_Mutex);
		m_RequestUnregListeners.push_back(listener);
	}

	void TaskNode2::_DoUnreg(TaskNodeListener2Ptr listener)
	{
		TaskNode2::Listeners::iterator iter = m_Listeners.begin();
		while(iter != m_Listeners.end())
		{
			TaskNode2ListenerPtr list_iter = TaskNode2ListenerPtr(*iter,NO_THROW);
			if(list_iter == listener)
			{
				iter = m_Listeners.erase(iter);
			}
			else
				iter++;
		}
	}
	
	struct TaskNode2ListenerExecutor
	{
		TaskNode2ListenerExecutor(TaskNode2* caller,const TaskNode2::Listeners& listeners, double delta_time)
			:m_Caller(caller),m_Listeners(listeners),m_DeltaTime(delta_time)
		{}
		TaskNode2ListenerExecutor(TaskNode2ListenerExecutor& e,tbb::split)
			:m_Listeners(e.m_Listeners)
		{}

		void operator()(const tbb::blocked_range<size_t>& r) const {
			for (size_t i=r.begin();i!=r.end();++i)
			{
				TaskNode2ListenerPtr listener = TaskNode2ListenerPtr(m_Listeners[i],NO_THROW);
				if(listener)
					listener->Update(m_DeltaTime,m_Caller);
			}
		}
		const TaskNode2::Listeners& m_Listeners;
		TaskNode2 *m_Caller;
		double m_DeltaTime;
	};

	struct TaskNode2ChildrenExecutor
	{
		TaskNode2ChildrenExecutor(const TaskNode2::TaskNode2Vector& children, double delta_time)
			:m_Children(children),m_DeltaTime(delta_time)
		{}
		TaskNode2ChildrenExecutor(TaskNode2ChildrenExecutor& e,tbb::split)
			:m_Children(e.m_Children)
		{}

		void operator()(const tbb::blocked_range<size_t>& r) const {
			for (size_t i=r.begin();i!=r.end();++i)
			{
				m_Children[i]->Update(m_DeltaTime,NULL);
			}
		}
		const TaskNode2::TaskNode2Vector& m_Children;
		double m_DeltaTime;
	};

	void TaskNode2::Update(double delta_time,tbb::task *parent)
	{
		//first remove listeners that want to unregister
		for(size_t i = 0; i < m_RequestUnregListeners.size(); i++)
		{
			_DoUnreg(TaskNode2ListenerPtr(m_RequestUnregListeners[i],NO_THROW));
		}
		m_RequestUnregListeners.clear();

		if(!m_Paused)
		{
			if(m_UpdateFrequency > 0)
			{
				double update_interval = 1.0/m_UpdateFrequency;
				//do some time slicing
				m_TimeToProcess += delta_time;
				long long num_steps = (long long) (m_TimeToProcess / update_interval);
				long long clamp_num_steps = num_steps;
				//Take max 10 simulation step each frame
				if(m_MaxSimulationSteps > 0 && num_steps > m_MaxSimulationSteps)
					clamp_num_steps = m_MaxSimulationSteps;

				for (int i = 0; i < clamp_num_steps; ++i)
				{
					UpdateListeners(update_interval,parent);
					UpdateChildren(update_interval,parent);
				}
				m_TimeToProcess -= update_interval * num_steps;
			}
			else
			{
				UpdateListeners(delta_time,parent);
				UpdateChildren(delta_time,parent);
			}
		}
	}

	void TaskNode2::UpdateListeners(double delta_time,tbb::task *parent)
	{
		TaskNode2::Listeners::iterator iter = m_Listeners.begin();
		switch(m_ListenerMode)
		{
		case SEQUENCE:
			//make copy of listeners to support unregister operation?
			while(iter != m_Listeners.end())
			{
				TaskNode2ListenerPtr listener = TaskNode2ListenerPtr(*iter,NO_THROW);
				if(listener)
				{
					listener->Update(delta_time,this);
					iter++;
				}
				else
					iter = m_Listeners.erase(iter);
			}
			break;
		case PARALLEL:
			TaskNode2ListenerExecutor exec(this,m_Listeners,delta_time);
			tbb::parallel_for(tbb::blocked_range<size_t>(0,m_Listeners.size()),exec);
			while(iter != m_Listeners.end())
			{
				TaskNode2ListenerPtr listener = TaskNode2ListenerPtr(*iter,NO_THROW);
				if(listener)
				{
					iter++;
				}
				else // remove dead listener
					iter = m_Listeners.erase(iter);
			}
		}
	}

	void TaskNode2::UpdateChildren(double delta_time,tbb::task *parent)
	{
		switch(m_ChildrenMode)
		{
		case SEQUENCE:
			for(size_t i=0; i < m_Children.size();i++)
			{
				//parent->wait_for_all();
				m_Children[i]->Update(delta_time,parent);
				//parent->increment_ref_count();
			}
			break;
		case PARALLEL:
			TaskNode2ChildrenExecutor exec(m_Children,delta_time);
			tbb::parallel_for(tbb::blocked_range<size_t>(0,m_Children.size()),exec);
			break;
		}
	}
}

