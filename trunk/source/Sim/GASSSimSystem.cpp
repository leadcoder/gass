/****************************************************************************
* This file is part of GASS.                                                *
* See http://code.google.com/p/gass/                                        *
*                                                                           *
* Copyright (c) 2008-2015 GASS team. See Contributors.txt for details.      *
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
#include "Core/Common.h"
#include "Sim/GASSSimSystem.h"
#include "Sim/GASSSimSystemManager.h"
#include "tinyxml2.h"
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>
namespace GASS
{
	SimSystem::SimSystem(void) : m_UpdateGroup(UGID_NO_UPDATE)
	{
		
	}

	SimSystem::~SimSystem(void) 
	{
	
	}

	void SimSystem::RegisterReflection()
	{
		RegisterProperty<UpdateGroupIDBinder>("UpdateGroup", &GASS::SimSystem::GetUpdateGroup, &GASS::SimSystem::SetUpdateGroup);
	}

	SimSystemManagerPtr SimSystem::GetSimSystemManager() const
	{
		return SimSystemManagerPtr(m_Owner);
	}

	void SimSystem::Register(SystemListenerPtr listener)
	{
		m_Listeners.push_back(listener);
	}

	void SimSystem::RegisterForUpdate()
	{
		UpdateGroupID ugid = GetUpdateGroup().GetValue();
		if(ugid != UGID_NO_UPDATE)
		{
			TaskNode2* node = SimEngine::Get().GetRunTimeController()->GetRootNode()->GetChildByID(ugid);
			node->Register(shared_from_this());
		}
	}

	void SimSystem::Unregister(SystemListenerPtr listener)
	{
		std::vector<SystemListenerWeakPtr>::iterator iter = m_Listeners.begin();
		while(iter != m_Listeners.end())
		{
			SystemListenerPtr c_list(*iter,NO_THROW);
			if(listener == c_list)
				iter = m_Listeners.erase(iter);
			else
				iter++;
		}
	}

	struct SystemListenerExecutor
	{
		SystemListenerExecutor(const std::vector<SystemListenerWeakPtr>& sl_vector, double delta_time)
			:m_SLVector(sl_vector),m_DeltaTime(delta_time)
		{}
		SystemListenerExecutor(SystemListenerExecutor& e,tbb::split)
			:m_SLVector(e.m_SLVector)
		{}

		void operator()(const tbb::blocked_range<size_t>& r) const {
			for (size_t i=r.begin();i!=r.end();++i)
			{
				SystemListenerPtr listener(m_SLVector[i],NO_THROW);
				listener->SystemTick(m_DeltaTime);
			}
		}
		const std::vector<SystemListenerWeakPtr>& m_SLVector;
		double m_DeltaTime;
	};

	void SimSystem::Update(double delta_time, TaskNode2* caller)
	{
		std::vector<SystemListenerWeakPtr>::iterator iter = m_Listeners.begin();
		//remove dead listeners
		while(iter != m_Listeners.end())
		{
			SystemListenerPtr listener(*iter,NO_THROW);
			
			if(!listener)
				iter = m_Listeners.erase(iter);
			else
			{
				iter++;
			}
		}
		if(m_Listeners.size() == 1)
		{
			SystemListenerPtr listener(*m_Listeners.begin(),NO_THROW);
			listener->SystemTick(delta_time);
		}
		else
		{
			SystemListenerExecutor exec(m_Listeners,delta_time);
			tbb::parallel_for(tbb::blocked_range<size_t>(0,m_Listeners.size()),exec);
		}
	}

	void SimSystem::LoadXML(tinyxml2::XMLElement *xml_elem)
	{
		_LoadProperties(xml_elem);
	}

	void SimSystem::SaveXML(tinyxml2::XMLElement *xml_elem)
	{
		tinyxml2::XMLElement * this_elem;
		this_elem = xml_elem->GetDocument()->NewElement( GetRTTI()->GetClassName().c_str());  
		xml_elem->LinkEndChild( this_elem );  
		_SaveProperties(this_elem);
	}
}




