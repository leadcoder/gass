/****************************************************************************
* This file is part of GASS.                                                *
* See https://github.com/leadcoder/gass                                     *
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
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


#include "Sim/RTC/GASSTaskNode.h"
#include "Core/Utils/GASSException.h"
#include "Core/Utils/GASSStringUtils.h"
#include "Core/Serialize/tinyxml2.h"
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
#include <tbb/spin_mutex.h>
#include <sstream>

namespace GASS
{
	TaskNode::TaskNode(int id) : m_ListenerMode(SEQUENCE),
		m_ChildrenMode(SEQUENCE),
		m_TimeToProcess(0),
		m_UpdateFrequency(0),
		m_Paused(false),
		m_ID(id),
		m_MaxSimulationSteps(-1),
		m_Mutex(new tbb::spin_mutex),
		m_CurrentTime(0)
	{

	}

	TaskNode::~TaskNode()
	{
		delete m_Mutex;
	}


	bool TaskNodeSortPredicate(const TaskNodePtr &lhs, const TaskNodePtr &rhs)
	{
		return lhs->GetID() < rhs->GetID();
	}

	void TaskNode::AddChildNode(TaskNodePtr child) 
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

	void TaskNode::Register(TaskNodeListenerPtr listener)
	{
		tbb::spin_mutex::scoped_lock lock(*m_Mutex);
		m_Listeners.push_back(listener);
	}

	void TaskNode::Unregister(TaskNodeListenerPtr listener)
	{
		tbb::spin_mutex::scoped_lock lock(*m_Mutex);
		m_RequestUnregListeners.push_back(listener);
	}

	void TaskNode::_DoUnreg(TaskNodeListenerPtr listener)
	{
		TaskNode::Listeners::iterator iter = m_Listeners.begin();
		while(iter != m_Listeners.end())
		{
			TaskNodeListenerPtr list_iter = (*iter).lock();
			if(list_iter == listener)
			{
				iter = m_Listeners.erase(iter);
			}
			else
				++iter;
		}
	}
	
	struct TaskNodeListenerExecutor
	{
		TaskNodeListenerExecutor(TaskNode* caller,const TaskNode::Listeners& listeners, double delta_time)
			:m_Caller(caller),m_Listeners(listeners),m_DeltaTime(delta_time)
		{}
		TaskNodeListenerExecutor(TaskNodeListenerExecutor& e,tbb::split)
			:m_Caller(e.m_Caller), m_DeltaTime(e.m_DeltaTime), m_Listeners(e.m_Listeners)
		{}
		TaskNodeListenerExecutor & operator=( const TaskNodeListenerExecutor & ) { return *this; }
	
		void operator()(const tbb::blocked_range<size_t>& r) const {
			for (size_t i=r.begin();i!=r.end();++i)
			{
				TaskNodeListenerPtr listener = (m_Listeners[i]).lock();
				if(listener)
					listener->Update(m_DeltaTime,m_Caller);
			}
		}
		const TaskNode::Listeners& m_Listeners;
		TaskNode *m_Caller;
		double m_DeltaTime;
	};

	struct TaskNodeChildrenExecutor
	{
		TaskNodeChildrenExecutor(const TaskNode::TaskNodeVector& children, double delta_time)
			:m_Children(children),m_DeltaTime(delta_time)
		{}
		TaskNodeChildrenExecutor(TaskNodeChildrenExecutor& e,tbb::split)
			:m_Children(e.m_Children) , m_DeltaTime(e.m_DeltaTime)
		{}
		TaskNodeChildrenExecutor & operator=( const TaskNodeChildrenExecutor & ) { return *this; }

		void operator()(const tbb::blocked_range<size_t>& r) const {
			for (size_t i=r.begin();i!=r.end();++i)
			{
				m_Children[i]->Update(m_DeltaTime,NULL);
			}
		}
		const TaskNode::TaskNodeVector& m_Children;
		double m_DeltaTime;
	};


	void TaskNode::RegisterPostUpdate(TaskNodeListenerPtr listener)
	{
		m_PostListeners.push_back(listener);
	}

	void TaskNode::UnregisterPostUpdate(TaskNodeListenerPtr listener)
	{
		TaskNode::Listeners::iterator iter = m_PostListeners.begin();
		while(iter != m_PostListeners.end())
		{
			TaskNodeListenerPtr list_iter = (*iter).lock();
			if(list_iter == listener)
			{
				iter = m_PostListeners.erase(iter);
			}
			else
				++iter;
		}
	}

	void TaskNode::Update(double delta_time,tbb::task *parent)
	{
		//first remove listeners that want to unregister
		for(size_t i = 0; i < m_RequestUnregListeners.size(); i++)
		{
			_DoUnreg(m_RequestUnregListeners[i].lock());
		}
		m_RequestUnregListeners.clear();

		if(!m_Paused)
		{
			if(m_UpdateFrequency > 0)
			{
				double update_interval = 1.0/m_UpdateFrequency;
				//do some time slicing
				m_TimeToProcess += delta_time;
				long long num_steps = static_cast<long long> (m_TimeToProcess / update_interval);
				long long clamp_num_steps = num_steps;
				//Take max 10 simulation step each frame
				if(m_MaxSimulationSteps > 0 && num_steps > m_MaxSimulationSteps)
					clamp_num_steps = m_MaxSimulationSteps;

				//std::cout << "steps:" << clamp_num_steps << "\n";
				for (int i = 0; i < clamp_num_steps; ++i)
				{
					UpdateListeners(update_interval,parent);
					UpdateChildren(update_interval,parent);
					UpdatePostListeners(update_interval,parent);
					m_CurrentTime += update_interval;
				}
				m_TimeToProcess -= update_interval * num_steps;
			}
			else
			{
				UpdateListeners(delta_time,parent);
				UpdateChildren(delta_time,parent);
				UpdatePostListeners(delta_time,parent);
				m_CurrentTime += delta_time;
			}
		}
	}

	void TaskNode::UpdateListeners(double delta_time,tbb::task* /*parent*/)
	{
		TaskNode::Listeners::iterator iter = m_Listeners.begin();
		switch(m_ListenerMode)
		{
		case SEQUENCE:
			//make copy of listeners to support unregister operation?
			while(iter != m_Listeners.end())
			{
				TaskNodeListenerPtr listener = (*iter).lock();
				if(listener)
				{
					listener->Update(delta_time,this);
					++iter;
				}
				else
					iter = m_Listeners.erase(iter);
			}
			break;
		case PARALLEL:
			TaskNodeListenerExecutor exec(this,m_Listeners,delta_time);
			tbb::parallel_for(tbb::blocked_range<size_t>(0,m_Listeners.size()),exec);
			while(iter != m_Listeners.end())
			{
				TaskNodeListenerPtr listener = (*iter).lock();
				if(listener)
				{
					++iter;
				}
				else // remove dead listener
					iter = m_Listeners.erase(iter);
			}
		}
	}

	void TaskNode::UpdatePostListeners(double delta_time,tbb::task* /*parent*/)
	{
		TaskNode::Listeners::iterator iter = m_PostListeners.begin();
			while(iter != m_PostListeners.end())
			{
				TaskNodeListenerPtr listener = (*iter).lock();
				if(listener)
				{
					listener->Update(delta_time,this);
					++iter;
				}
				else
					iter = m_PostListeners.erase(iter);
			}
			
	}

	TaskNode*  TaskNode::GetChildByID(int id) const
	{
		for(size_t i=0; i < m_Children.size();i++)
		{
			if(m_Children[i]->GetID() == id)
				return m_Children[i].get();
		}
		std::stringstream ss;
		ss << "Failed to get TaskNode by id:" << id;
		GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,ss.str(), "TaskNode2::GetChildByID");
		//return NULL;
	}

	void TaskNode::UpdateChildren(double delta_time,tbb::task *parent)
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
			TaskNodeChildrenExecutor exec(m_Children,delta_time);
			tbb::parallel_for(tbb::blocked_range<size_t>(0,m_Children.size()),exec);
			break;
		}
	}
}

