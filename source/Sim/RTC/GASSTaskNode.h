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
#pragma once

#include "Sim/GASSCommon.h"

namespace tbb
{
	class task;
	class spin_mutex;
}

namespace GASS
{
	class TBBManager;
	class TaskNode;
	typedef GASS_SHARED_PTR<TaskNode> TaskNode2Ptr;
	class ITaskNodeListener
	{
	public:
		virtual ~ITaskNodeListener(){}
		virtual void Update(double delta_time, TaskNode* caller) = 0;

	};

	typedef GASS_WEAK_PTR<ITaskNodeListener> TaskNodeListenerWeakPtr;
	typedef GASS_SHARED_PTR<ITaskNodeListener> TaskNodeListenerPtr;

	class TaskNodeUpdateCallback
	{
	public:
		TaskNodeUpdateCallback(GASS_FUNCTION<void (double)> func, TaskNodeListenerPtr object)
		{
			m_Callback = func;
			m_Object = object;
		}
		GASS_FUNCTION<void (double)> m_Callback;
		TaskNodeListenerWeakPtr m_Object;
	};

	class GASSExport TaskNode
	{
	public:
		typedef std::vector<TaskNodeListenerWeakPtr> Listeners;
		typedef std::vector<TaskNodeUpdateCallback> CallbackVector;

		typedef std::vector<TaskNode2Ptr> TaskNode2Vector;
		enum UpdateMode
		{
			PARALLEL,
			SEQUENCE
		};
		TaskNode(int id);
		virtual ~TaskNode();
		void Update(double delta_time,tbb::task *parent);
		
		void Register(TaskNodeListenerPtr listener);
		void Unregister(TaskNodeListenerPtr listener);
		void RegisterPostUpdate(TaskNodeListenerPtr listener);
		void UnregisterPostUpdate(TaskNodeListenerPtr listener);
		void SetPaused(bool value) { m_Paused= value;}
		bool GetPaused() const {return m_Paused;}
		void ResetTime() { m_CurrentTime = 0;}
		double GetTime() const { return m_CurrentTime;}
		void AddChildNode(TaskNode2Ptr child);
		void SetUpdateFrequency(double value) {m_UpdateFrequency = value;}
		double GetUpdateFrequency() const {return m_UpdateFrequency;}
		void SetListenerUpdateMode(UpdateMode mode) { m_ListenerMode =mode;}
		UpdateMode GetListenerUpdateMode() const { return m_ListenerMode;}
		void SetChildrenUpdateMode(UpdateMode mode) { m_ChildrenMode =mode;}
		UpdateMode GetChildrenUpdateMode() const { return m_ChildrenMode ;}
		int GetID() const {return m_ID;}
		void SetMaxSimulationSteps(int value) {m_MaxSimulationSteps = value;}
		int GetMaxSimulationSteps() const {return m_MaxSimulationSteps;}
		TaskNode* GetChildByID(int id) const;
	private:
		//public for now, don't call!
		void UpdateChildren(double delta_time,tbb::task *parent);
		void UpdateListeners(double delta_time,tbb::task *parent);
		void UpdatePostListeners(double delta_time,tbb::task *parent);
		void _DoUnreg(TaskNodeListenerPtr listener);

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
		double m_CurrentTime;

		//helper
		double m_TimeToProcess;
	};
}

