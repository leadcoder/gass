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
#include "Core/Common.h"
#include "Sim/Systems/GASSSimSystem.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "tinyxml.h"
#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"
namespace GASS
{
	SimSystem::SimSystem(void) : m_UpdateBucket(0), m_Name("SimSystem")
	{
	
	}

	SimSystem::~SimSystem(void) 
	{
	
	}

	void SimSystem::OnCreate()
	{

	}

	void SimSystem::RegisterReflection()
	{
		RegisterProperty<std::string>( "Name", &GASS::SimSystem::GetName, &GASS::SimSystem::SetName);
		RegisterProperty<int>( "UpdateBucket", &GASS::SimSystem::GetUpdateBucket, &GASS::SimSystem::SetUpdateBucket);
	}

	SimSystemManagerPtr SimSystem::GetSimSystemManager() const
	{
		return boost::shared_dynamic_cast<SimSystemManager>(SystemManagerPtr(m_Owner));
	}

	void SimSystem::Register(SystemListenerPtr listener)
	{
		m_Listeners.push_back(listener);
	}

	void SimSystem::Unregister(SystemListenerPtr listener)
	{
		std::vector<SystemListenerPtr>::iterator iter = m_Listeners.begin();
		while(iter != m_Listeners.end())
		{
			if(listener == *iter)
				iter = m_Listeners.erase(iter);
			else
				iter++;
		}
	}

	struct SystemListenerExecutor
	{
		SystemListenerExecutor(const std::vector<SystemListenerPtr>& sl_vector, double delta_time)
			:m_SLVector(sl_vector),m_DeltaTime(delta_time)
		{}
		SystemListenerExecutor(SystemListenerExecutor& e,tbb::split)
			:m_SLVector(e.m_SLVector)
		{}

		void operator()(const tbb::blocked_range<size_t>& r) const {
			for (size_t i=r.begin();i!=r.end();++i)
			{
				m_SLVector[i]->SystemTick(m_DeltaTime);
			}
		}
		const std::vector<SystemListenerPtr>& m_SLVector;
		double m_DeltaTime;
	};

	void SimSystem::Update(double delta)
	{
		std::vector<SystemListenerPtr>::iterator iter = m_Listeners.begin();
		//remove dead listeners
		while(iter != m_Listeners.end())
		{
			if(!*iter)
				iter = m_Listeners.erase(iter);
			else
			{
				iter++;
			}
		}
		if(m_Listeners.size() > 0)
		{
			(*m_Listeners.begin())->SystemTick(delta);
		}
		else
		{
			SystemListenerExecutor exec(m_Listeners,delta);
			tbb::parallel_for(tbb::blocked_range<size_t>(0,m_Listeners.size()),exec);
		}
	}

	void SimSystem::LoadXML(TiXmlElement *xml_elem)
	{
		LoadProperties(xml_elem);
	}

	void SimSystem::SaveXML(TiXmlElement *xml_elem)
	{
		TiXmlElement * this_elem;
		this_elem = new TiXmlElement( GetRTTI()->GetClassName().c_str());  
		xml_elem->LinkEndChild( this_elem );  
		SaveProperties(this_elem);
	}

	int SimSystem::GetUpdateBucket() const
	{
		return m_UpdateBucket;
	}

	void SimSystem::SetUpdateBucket(int priority)
	{
		m_UpdateBucket = priority;
	}
}




