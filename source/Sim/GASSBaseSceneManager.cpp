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

#include "Sim/GASSBaseSceneManager.h"
#include "Sim/GASSSimSystemManager.h"
#include "Core/Common.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "tinyxml2.h"
#include <tbb/blocked_range.h>
#include <tbb/parallel_for.h>

namespace GASS
{
	BaseSceneManager::BaseSceneManager(void)
	{
		
	}

	BaseSceneManager::~BaseSceneManager(void)
	{

	}

	void BaseSceneManager::RegisterReflection()
	{
		RegisterProperty<std::string>( "Name", &GASS::BaseSceneManager::GetName, &GASS::BaseSceneManager::SetName);
	}

	struct SceneListenerExecutor
	{
		SceneListenerExecutor(const std::vector<SceneManagerListenerWeakPtr>& sl_vector, double delta_time)
			:m_SLVector(sl_vector),m_DeltaTime(delta_time)
		{}
		SceneListenerExecutor(SceneListenerExecutor& e,tbb::split)
			:m_SLVector(e.m_SLVector), m_DeltaTime(e.m_DeltaTime)
		{}
		SceneListenerExecutor & operator=( const SceneListenerExecutor & ) { return *this; }

		void operator()(const tbb::blocked_range<size_t>& r) const {
			for (size_t i=r.begin();i!=r.end();++i)
			{
				SceneManagerListenerPtr listener = m_SLVector[i].lock();
				listener->SceneManagerTick(m_DeltaTime);
			}
		}
		const std::vector<SceneManagerListenerWeakPtr>& m_SLVector;
		double m_DeltaTime;
	};

	void BaseSceneManager::SystemTick(double delta_time) 
	{
		std::vector<SceneManagerListenerWeakPtr>::iterator iter = m_Listeners.begin();
		/*while(iter != m_Listeners.end())
		{
			SceneManagerListenerPtr listener = (*iter).lock();
			if(listener)
			{
				listener->SceneManagerTick(delta_time);
				iter++;
			}
			else 
				iter = m_Listeners.erase(iter);
		}*/


		while(iter != m_Listeners.end())
		{
			SceneManagerListenerPtr listener = (*iter).lock();
			if(listener)
			{
				++iter;
			}
			else 
				iter = m_Listeners.erase(iter);
		}

		SceneListenerExecutor exec(m_Listeners,delta_time);
		tbb::parallel_for(tbb::blocked_range<size_t>(0,m_Listeners.size()),exec);
	}

	void BaseSceneManager::Register(SceneManagerListenerPtr listener)
	{
		//tbb::spin_mutex::scoped_lock lock(m_Mutex);
		m_Listeners.push_back(listener);
	}

	void BaseSceneManager::Unregister(SceneManagerListenerPtr listener)
	{
		//tbb::spin_mutex::scoped_lock lock(m_Mutex);
		std::vector<SceneManagerListenerWeakPtr>::iterator iter = m_Listeners.begin();
		while(iter != m_Listeners.end())
		{
			
			if((*iter).lock() == listener)
				iter = m_Listeners.erase(iter);
			else
				++iter;
		}
	}

	void BaseSceneManager::LoadXML(tinyxml2::XMLElement *xml_elem)
	{
		_LoadProperties(xml_elem);
	}


	void BaseSceneManager::SaveXML(tinyxml2::XMLElement *xml_elem)
	{
		tinyxml2::XMLElement * this_elem;
		this_elem = xml_elem->GetDocument()->NewElement(GetRTTI()->GetClassName().c_str());  
		xml_elem->LinkEndChild( this_elem );  
		_SaveProperties(this_elem);
	}
	
}




