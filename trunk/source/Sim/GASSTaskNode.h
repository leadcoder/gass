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
#ifndef TASK_NODE_H
#define TASK_NODE_H
#include "Sim/GASSCommon.h"

namespace tinyxml2
{
	class XMLElement;
}

namespace tbb
{
	class task;
}
namespace GASS
{
	class TaskNode;
	typedef SPTR<TaskNode> TaskNodePtr;
	class ITaskNodeListener
	{
	public:
		virtual ~ITaskNodeListener(){}
		virtual void Update(double delta_time) = 0;
	};

	typedef WPTR<ITaskNodeListener> TaskNodeListenerWeakPtr;
	typedef SPTR<ITaskNodeListener> TaskNodeListenerPtr;
	class GASSExport TaskNode
	{
	public:
		typedef std::vector<TaskNodeListenerWeakPtr> Listeners;
		typedef std::vector<TaskNodePtr> TaskNodeVector;
		enum UpdateMode
		{
			PARALLEL,
			SEQUENCE
		};
		TaskNode();
		void LoadXML();
		virtual ~TaskNode();
		void LoadXML(tinyxml2::XMLElement *xml_elem);
		TaskNode* GetNodeByName(const std::string &name); 
		void Update(double delta_time,tbb::task *parent);
		
		//public for now, dont call!
		void UpdateChildren(double delta_time,tbb::task *parent);
		void UpdateListeners(double delta_time,tbb::task *parent);
		
		void Register(TaskNodeListenerPtr listener);
		void Unregister(TaskNodeListenerPtr listener);
		void SetOnlyUpdateOnRequest(bool value) { m_OnlyUpdateOnRequest = value;}
		bool GetOnlyUpdateOnRequest() const {return m_OnlyUpdateOnRequest;}
	private:
		TaskNodeVector m_Children;
		std::string m_Name;
		UpdateMode m_NodeMode;
		Listeners m_Listeners;
		bool m_OnlyUpdateOnRequest;
		bool m_RespondToPause;
		int m_UpdateFrequency;

		//helper
		double m_TimeToProcess;
	};
}


#endif
