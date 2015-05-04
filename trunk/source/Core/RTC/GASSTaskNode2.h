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

#include "Core/Common.h"
#include <boost/function.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace tbb
{
	class task;
	class spin_mutex;
}

namespace GASS
{
	class RunTimeController2;
	class TaskNode2;
	typedef SPTR<TaskNode2> TaskNode2Ptr;
	class ITaskNode2Listener
	{
	public:
		virtual ~ITaskNode2Listener(){}
		virtual void Update(double delta_time, TaskNode2* caller) = 0;

	};

	typedef WPTR<ITaskNode2Listener> TaskNode2ListenerWeakPtr;
	typedef SPTR<ITaskNode2Listener> TaskNode2ListenerPtr;

	class TaskNode2UpdateCallback
	{
	public:
		TaskNode2UpdateCallback(boost::function<void (double)> func, TaskNode2ListenerPtr object)
		{
			m_Callback = func;
			m_Object = object;
		}
		boost::function<void (double)> m_Callback;
		TaskNode2ListenerWeakPtr m_Object;
	};

	class GASSCoreExport TaskNode2
	{
	public:
		typedef std::vector<TaskNode2ListenerWeakPtr> Listeners;
		typedef std::vector<TaskNode2UpdateCallback> CallbackVector;

		typedef std::vector<TaskNode2Ptr> TaskNode2Vector;
		enum UpdateMode
		{
			PARALLEL,
			SEQUENCE
		};
		TaskNode2(int id);
		virtual ~TaskNode2();
		void Update(double delta_time,tbb::task *parent);
		
		void Register(TaskNode2ListenerPtr listener);
		void Unregister(TaskNode2ListenerPtr listener);
		void RegisterPostUpdate(TaskNode2ListenerPtr listener);
		void UnregisterPostUpdate(TaskNode2ListenerPtr listener);
		void SetPaused(bool value) { m_Paused= value;}
		bool GetPaused() const {return m_Paused;}
		void AddChildNode(TaskNode2Ptr child);
		void SetUpdateFrequency(double value) {m_UpdateFrequency = value;}
		double GetUpdateFrequency() const {return m_UpdateFrequency;}
		void SetListenerUpdateMode(UpdateMode mode) { m_ListenerMode =mode;}
		UpdateMode GetListenerUpdateMode() const { return m_ListenerMode;}
		void SetChildrenUpdateMode(UpdateMode mode) { m_ChildrenMode =mode;}
		UpdateMode GetChildrenUpdateMode() const { return m_ChildrenMode ;}
		int GetID() const {return m_ID;}
		void SetMaxSimulationSteps(int value) {m_MaxSimulationSteps = value;}
		int GetMaxSimulationSteps(int value) const {return m_MaxSimulationSteps;}
		TaskNode2* GetChildByID(int id) const;
	private:
		//public for now, don't call!
		void UpdateChildren(double delta_time,tbb::task *parent);
		void UpdateListeners(double delta_time,tbb::task *parent);
		void UpdatePostListeners(double delta_time,tbb::task *parent);
		void _DoUnreg(TaskNode2ListenerPtr listener);

		TaskNode2Vector m_Children;
		
		int m_ID;
		UpdateMode m_ChildrenMode;
		UpdateMode m_ListenerMode;
		Listeners m_Listeners;
		Listeners m_PostListeners;
		Listeners m_RequestUnregListeners;

		CallbackVector m_CallbackVector;
		
		bool m_Paused;
		double m_UpdateFrequency;
		tbb::spin_mutex *m_Mutex;
		int m_MaxSimulationSteps;

		//helper
		double m_TimeToProcess;
	};
}

