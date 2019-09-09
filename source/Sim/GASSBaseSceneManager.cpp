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
#include "Core/Serialize/tinyxml2.h"

namespace GASS
{
	BaseSceneManager::BaseSceneManager(SceneWeakPtr scene) : m_Scene(scene), m_PreSystemUpdate(false)
	{
		
	}

	void BaseSceneManager::RegisterReflection()
	{
		RegisterGetSet( "Name", &GASS::BaseSceneManager::GetName, &GASS::BaseSceneManager::SetName);
	}

	void BaseSceneManager::_UpdatePostListeners(double delta_time)
	{
		std::vector<SceneManagerListenerWeakPtr>::iterator iter = m_PostListeners.begin();

		while (iter != m_PostListeners.end())
		{
			SceneManagerListenerPtr listener = (*iter).lock();
			if (listener)
			{
				++iter;
			}
			else
				iter = m_PostListeners.erase(iter);
		}

		for (size_t i = 0; i < m_PostListeners.size(); i++)
		{
			SceneManagerListenerPtr listener = m_PostListeners[i].lock();
			if (listener)
				listener->SceneManagerTick(delta_time);
		}
	}

	void BaseSceneManager::_UpdatePreListeners(double delta_time)
	{
		std::vector<SceneManagerListenerWeakPtr>::iterator iter = m_PreListeners.begin();

		while (iter != m_PreListeners.end())
		{
			SceneManagerListenerPtr listener = (*iter).lock();
			if (listener)
			{
				++iter;
			}
			else
				iter = m_PreListeners.erase(iter);
		}

		for (size_t i = 0; i < m_PreListeners.size(); i++)
		{
			SceneManagerListenerPtr listener = m_PreListeners[i].lock();
			if (listener)
				listener->SceneManagerTick(delta_time);
		}
	}
	

	void BaseSceneManager::RegisterPreUpdate(SceneManagerListenerPtr listener)
	{
		m_PreListeners.push_back(listener);
	}

	void BaseSceneManager::RegisterPostUpdate(SceneManagerListenerPtr listener)
	{
		m_PostListeners.push_back(listener);
	}

	/*void BaseSceneManager::Unregister(SceneManagerListenerPtr listener)
	{
		std::vector<SceneManagerListenerWeakPtr>::iterator iter = m_Listeners.begin();
		while(iter != m_Listeners.end())
		{
			
			if((*iter).lock() == listener)
				iter = m_Listeners.erase(iter);
			else
				++iter;
		}
	}*/

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




