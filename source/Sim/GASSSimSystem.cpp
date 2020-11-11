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
#include "Core/Common.h"
#include "Sim/GASSSimSystem.h"
#include "Core/Serialize/tinyxml2.h"

namespace GASS
{
	SimSystem::SimSystem(SimSystemManagerWeakPtr manager) : m_UpdateGroup(UGID_NO_UPDATE),
		m_Owner(manager)
	{
		
	}

	SimSystem::~SimSystem(void) 
	{
	
	}

	void SimSystem::RegisterReflection()
	{
		RegisterMember("UpdateGroup", &GASS::SimSystem::m_UpdateGroup);
	}

	SimSystemManagerPtr SimSystem::GetSimSystemManager() const
	{
		return SimSystemManagerPtr(m_Owner);
	}

	void SimSystem::RegisterListener(SystemListenerPtr listener)
	{
		m_Listeners.push_back(listener);
	}

	void SimSystem::UnregisterListener(SystemListenerPtr listener)
	{
		std::vector<SystemListenerWeakPtr>::iterator iter = m_Listeners.begin();
		while(iter != m_Listeners.end())
		{
			SystemListenerPtr c_list = (*iter).lock();
			if(listener == c_list)
				iter = m_Listeners.erase(iter);
			else
				++iter;
		}
	}

	void SimSystem::OnSystemUpdate(double delta_time)
	{
	//	_UpdateListeners(delta_time);
	}


	void SimSystem::_Update(double delta_time)
	{
		std::vector<SystemListenerWeakPtr>::iterator iter = m_Listeners.begin();
		//remove dead listeners
		while (iter != m_Listeners.end())
		{
			SystemListenerPtr listener = (*iter).lock();

			if (!listener)
				iter = m_Listeners.erase(iter);
			else
			{
				++iter;
			}
		}

		_PreUpdate(delta_time);
		OnSystemUpdate(delta_time);
		_PostUpdate(delta_time);
	}

	void SimSystem::_PreUpdate(double delta_time)
	{
		for (size_t i = 0; i < m_Listeners.size(); ++i)
		{
			SystemListenerPtr listener = m_Listeners[i].lock();
			if(listener)
				listener->OnPreSystemUpdate(delta_time);
		}
	}
	
	void SimSystem::_PostUpdate(double delta_time)
	{
		for (size_t i = 0; i < m_Listeners.size(); ++i)
		{
			SystemListenerPtr listener = m_Listeners[i].lock();
			if (listener)
				listener->OnPostSystemUpdate(delta_time);
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




