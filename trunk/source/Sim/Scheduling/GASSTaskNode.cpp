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

#include "Sim/Scheduling/GASSTaskNode.h"
#include "Core/Utils/GASSException.h"
#include "Core/Utils/GASSMisc.h"
#include <tinyxml.h>
#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"

namespace GASS
{
	TaskNode::TaskNode() : m_ListenerMode(SEQUENCE)
		,m_ChildrenMode(SEQUENCE)
		,m_NodeMode(SEQUENCE)
	{

	}

	TaskNode::~TaskNode()
	{

	}

	void TaskNode::LoadXML(TiXmlElement *xml_elem)
	{
		if(xml_elem->Attribute("name"))
		{
			m_Name = xml_elem->Attribute("name");
		}
		else
		{
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to get task node name", "TaskNode::LoadXML");
		}

		if(xml_elem->Attribute("listener_mode"))
		{
			std::string type = Misc::ToUpper(xml_elem->Attribute("listener_mode"));
			if(type == "PARALLEL")
			{
				m_ListenerMode = PARALLEL;
			}
			else if(type == "SEQUENCE")
			{
				m_ListenerMode = SEQUENCE;
			}
			else
			{
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Undefined task node type:" + type, "TaskNode::LoadXML");
			}
		}
		else
		{
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Undefined update mode", "TaskNode::LoadXML");
		}

		if(xml_elem->Attribute("children_mode"))
		{
			std::string type = Misc::ToUpper(xml_elem->Attribute("children_mode"));
			if(type == "PARALLEL")
			{
				m_ChildrenMode = PARALLEL;
			}
			else if(type == "SEQUENCE")
			{
				m_ChildrenMode = SEQUENCE;
			}
			else
			{
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Undefined update mode:" + type, "TaskNode::LoadXML");
			}
		}
		else
		{
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to get task node type", "TaskNode::LoadXML");
		}


		if(xml_elem->Attribute("mode"))
		{
			std::string type = Misc::ToUpper(xml_elem->Attribute("mode"));
			if(type == "PARALLEL")
			{
				m_NodeMode = PARALLEL;
			}
			else if(type == "SEQUENCE")
			{
				m_NodeMode = SEQUENCE;
			}
			else
			{
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Undefined update mode:" + type, "TaskNode::LoadXML");
			}
		}
		else
		{
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to get task node type", "TaskNode::LoadXML");
		}


		/*if(xml_elem->Attribute("custom_freqency"))
		{

		}*/
		TiXmlElement *xml_child_elem = xml_elem->FirstChildElement("TaskNode");
		while(xml_child_elem)
		{
			TaskNodePtr task_node(new TaskNode());
			task_node->LoadXML(xml_child_elem);
			m_Children.push_back(task_node);
			xml_child_elem = xml_child_elem->NextSiblingElement("TaskNode");
		}
	}

	TaskNode* TaskNode::GetNodeByName(const std::string &name) 
	{
		if(m_Name == name)
			return this;

		for(size_t i=0; i < m_Children.size();i++)
		{
			TaskNode* node = m_Children[i]->GetNodeByName(name);
			if(node)
				return node;
		}
		return NULL;
	}

	struct TaskNodeListenerExecutor
	{
		TaskNodeListenerExecutor(const TaskNode::Listeners& listeners, double delta_time)
			:m_Listeners(listeners),m_DeltaTime(delta_time)
		{}
		TaskNodeListenerExecutor(TaskNodeListenerExecutor& e,tbb::split)
			:m_Listeners(e.m_Listeners)
		{}

		void operator()(const tbb::blocked_range<size_t>& r) const {
			for (size_t i=r.begin();i!=r.end();++i)
			{
				TaskNodeListenerPtr listener = TaskNodeListenerPtr(m_Listeners[i],boost::detail::sp_nothrow_tag());
				if(listener)
					listener->Update(m_DeltaTime);
			}
		}
		const TaskNode::Listeners& m_Listeners;
		double m_DeltaTime;
	};


	/*struct TaskNodeChildrenExecutor
	{
		TaskNodeChildrenExecutor(const TaskNode::TaskNodeVector& children, double delta_time)
			:m_Children(children),m_DeltaTime(delta_time)
		{}
		TaskNodeChildrenExecutor(TaskNodeChildrenExecutor& e,tbb::split)
			:m_Children(e.m_Children)
		{}

		void operator()(const tbb::blocked_range<size_t>& r) const {
			for (size_t i=r.begin();i!=r.end();++i)
			{
				m_Children[i]->Update(m_DeltaTime);
			}
		}
		const TaskNode::TaskNodeVector& m_Children;
		double m_DeltaTime;
	};*/

	class GASSExport UpdateListenersTask : public tbb::task
	{
	public:
		UpdateListenersTask(double delta_time, TaskNode* node) : m_DeltaTime(delta_time),m_TaskNode(node)
		{

		}
		tbb::task*  execute()
		{
			m_TaskNode->UpdateListeners(m_DeltaTime,this);
			return NULL;
		}
	private:
		TaskNode* m_TaskNode;
		double m_DeltaTime;
	};

	class GASSExport UpdateChildrenTask : public tbb::task
	{
	public:
		UpdateChildrenTask (double delta_time, TaskNode* node) : m_DeltaTime(delta_time),m_TaskNode(node)
		{

		}
		tbb::task*  execute()
		{
			m_TaskNode->UpdateChildren(m_DeltaTime,this->parent());
			return NULL;
		}
	private:
		TaskNode* m_TaskNode;
		double m_DeltaTime;
	};

	class GASSExport UpdateTaskNode : public tbb::task
	{
	public:
		UpdateTaskNode (double delta_time, TaskNode* node) : m_DeltaTime(delta_time),m_TaskNode(node)
		{

		}
		tbb::task*  execute()
		{
			m_TaskNode->Update(m_DeltaTime,this->parent());
			return NULL;
		}
	private:
		TaskNode* m_TaskNode;
		double m_DeltaTime;
	};


	class GASSExport UpdateListenerTask : public tbb::task
	{
	public:
		UpdateListenerTask(double delta_time, TaskNodeListenerPtr listener) : m_DeltaTime(delta_time),m_Listener(listener)
		{

		}
		tbb::task*  execute()
		{
			m_Listener->Update(m_DeltaTime);
			return NULL;
		}
	private:
		TaskNodeListenerPtr m_Listener;
		double m_DeltaTime;
	};


	void TaskNode::Update(double delta_time,tbb::task *parent) 
	{
		//remove dead listeners
		switch(m_NodeMode)
		{
		case SEQUENCE:
			UpdateListeners(delta_time,parent);
			UpdateChildren(delta_time,parent);
		case PARALLEL:
			tbb::task_list task_list;
			task_list.push_back(*new(parent->allocate_child()) UpdateListenersTask(delta_time,this));
			task_list.push_back(*new(parent->allocate_child()) UpdateChildrenTask(delta_time,this));
			parent->spawn(task_list);
			break;
		}
	}

	void TaskNode::UpdateListeners(double delta_time,tbb::task *parent) 
	{
		TaskNode::Listeners::iterator iter = m_Listeners.begin();
		switch(m_ListenerMode)
		{
		case SEQUENCE:

			while(iter != m_Listeners.end())
			{
				TaskNodeListenerPtr listener = TaskNodeListenerPtr(*iter,boost::detail::sp_nothrow_tag());
				if(listener)
				{
					listener->Update(delta_time);
					iter++;
				}
				else 
					iter = m_Listeners.erase(iter);
			}
			break;
		case PARALLEL:
			//TaskNodeListenerExecutor exec(m_Listeners,delta_time);
			//tbb::parallel_for(tbb::blocked_range<size_t>(0,m_Listeners.size()),exec);
			
			while(iter != m_Listeners.end())
			{
				tbb::task_list task_list;
			
				TaskNodeListenerPtr listener = TaskNodeListenerPtr(*iter,boost::detail::sp_nothrow_tag());
				if(listener)
				{
					task_list.push_back(*new(parent->allocate_child()) UpdateListenerTask(delta_time,listener));
					iter++;
				}
				else 
					iter = m_Listeners.erase(iter);

				parent->spawn(task_list);
			}
			break;
		}
	}

	void TaskNode::UpdateChildren(double delta_time,tbb::task *parent)
	{
		switch(m_ChildrenMode)
		{
		case SEQUENCE:
			for(size_t i=0; i < m_Children.size();i++)
			{
				m_Children[i]->Update(delta_time,parent);
			}
			break;
		case PARALLEL:
			//TaskNodeChildrenExecutor exec(m_Children,delta_time);
			//tbb::parallel_for(tbb::blocked_range<size_t>(0,m_Children.size()),exec);
			tbb::task_list task_list;
			for(size_t i=0; i < m_Children.size();i++)
			{
				task_list.push_back(*new(parent->allocate_child()) UpdateTaskNode(delta_time,this));
			}
			parent->spawn(task_list);
			break;
		}
	}
}

