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


#include "Sim/GASSTaskNode.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSRunTimeController.h"
#include "Core/Utils/GASSException.h"
#include "Core/Utils/GASSStringUtils.h"
#include <tinyxml2.h>
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>

namespace GASS
{
	TaskNode::TaskNode() : m_NodeMode(SEQUENCE), m_OnlyUpdateOnRequest(false), m_RespondToPause(true),
		m_TimeToProcess(0),
		m_UpdateFrequency(0)

	{

	}

	TaskNode::~TaskNode()
	{

	}

	void TaskNode::LoadXML(tinyxml2::XMLElement *xml_elem)
	{
		if(xml_elem->Attribute("name"))
		{
			m_Name = xml_elem->Attribute("name");
		}
		else
		{
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to get task node name", "TaskNode::LoadXML");
		}

		if(xml_elem->Attribute("mode"))
		{
			std::string type = StringUtils::ToUpper(xml_elem->Attribute("mode"));
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
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to get task mode", "TaskNode::LoadXML");
		}


		if(xml_elem->Attribute("externalUpdate"))
		{
			int value = m_OnlyUpdateOnRequest;
			xml_elem->QueryIntAttribute("externalUpdate",&value);
			m_OnlyUpdateOnRequest = value;
		}


		if(xml_elem->Attribute("respondToPause"))
		{
			int value = m_RespondToPause;
			xml_elem->QueryIntAttribute("respondToPause",&value);
			m_RespondToPause = value;
		}

		if(xml_elem->Attribute("updateFrequency"))
		{
			int value = m_UpdateFrequency;
			xml_elem->QueryIntAttribute("updateFrequency",&value);
			m_UpdateFrequency = value;
		}

		tinyxml2::XMLElement *xml_child_elem = xml_elem->FirstChildElement("TaskNode");
		while(xml_child_elem)
		{
			TaskNodePtr task_node(new TaskNode());
			task_node->LoadXML(xml_child_elem);
			m_Children.push_back(task_node);
			xml_child_elem = xml_child_elem->NextSiblingElement("TaskNode");
		}
	}

	void TaskNode::Register(TaskNodeListenerPtr listener)
	{
		m_Listeners.push_back(listener);
	}

	void TaskNode::Unregister(TaskNodeListenerPtr listener)
	{

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
				TaskNodeListenerPtr listener = TaskNodeListenerPtr(m_Listeners[i],NO_THROW);
				if(listener)
					listener->Update(m_DeltaTime);
			}
		}
		const TaskNode::Listeners& m_Listeners;
		double m_DeltaTime;
	};


	struct TaskNodeChildrenExecutor
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
				m_Children[i]->Update(m_DeltaTime,NULL);
			}
		}
		const TaskNode::TaskNodeVector& m_Children;
		double m_DeltaTime;
	};


	void TaskNode::Update(double delta_time,tbb::task *parent)
	{

		//update on request
		/*if(m_OnlyUpdateOnRequest)
		{
			//Check if we should update this frame?
			if(SimEngine::Get().GetRuntimeController()->HasUpdateRequest())
			{
				double request_time = SimEngine::Get().GetRuntimeController()->GetUpdateRequestTimeStep();

				if(m_UpdateFrequency > 0)
				{
					double update_interval = 1.0/double (m_UpdateFrequency);
					//do some time slicing
					m_TimeToProcess += request_time;
					int num_steps = (int) (m_TimeToProcess / update_interval);
					int clamp_num_steps = num_steps;
					//Take max 10 simulation step each frame
					if(num_steps > 10)
						clamp_num_steps = 10;

					for (int i = 0; i < clamp_num_steps; ++i)
					{
						UpdateListeners(update_interval,parent);
						UpdateChildren(update_interval,parent);

						SimEngine::Get().SyncMessages(update_interval);
					}
					m_TimeToProcess -= update_interval * num_steps;
				}
				else
				{
					UpdateListeners(request_time,parent);
					UpdateChildren(request_time,parent);
				}
			}
		}
		else*/
		{
			//only check if we should reflect paused flag and if so if we the simulation is paused.
			//if(!(m_RespondToPause && SimEngine::Get().GetRuntimeController()->GetSimulationPaused()))
			{
				UpdateListeners(delta_time,parent);
				UpdateChildren(delta_time,parent);
			}
		}
	}

	void TaskNode::UpdateListeners(double delta_time,tbb::task *parent)
	{
		TaskNode::Listeners::iterator iter = m_Listeners.begin();
		switch(m_NodeMode)
		{
		case SEQUENCE:

			while(iter != m_Listeners.end())
			{
				TaskNodeListenerPtr listener = TaskNodeListenerPtr(*iter,NO_THROW);
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
			TaskNodeListenerExecutor exec(m_Listeners,delta_time);
			tbb::parallel_for(tbb::blocked_range<size_t>(0,m_Listeners.size()),exec);

			/*while(iter != m_Listeners.end())
			{
				tbb::task_list task_list;

				TaskNodeListenerPtr listener = TaskNodeListenerPtr(*iter,NO_THROW);
				if(listener)
				{
					task_list.push_back(*new(parent->allocate_child()) UpdateListenerTask(delta_time,listener));
					parent->increment_ref_count();
					iter++;
				}
				else
					iter = m_Listeners.erase(iter);
				parent->spawn(task_list);
			}
			break;*/
		}
	}

	void TaskNode::UpdateChildren(double delta_time,tbb::task *parent)
	{
		switch(m_NodeMode)
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
			/*tbb::task_list task_list;
			Dummy& c = *new( parent->allocate_continuation() ) Dummy();
			for(size_t i=0; i < m_Children.size();i++)
			{

				task_list.push_back(*new(parent->allocate_child()) UpdateTaskNode(delta_time,m_Children[i].get()));
				parent->increment_ref_count();
			}
			if(m_Children.size() > 0)
			{
				parent->spawn(task_list);
			}*/
			break;
		}
	}

}

