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

#include "Sim/Scenario/Scene/GASSBaseSceneManager.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "Core/Common.h"
#include "Core/System/GASSSystemFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "tinyxml.h"

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


	void BaseSceneManager::OnCreate() 
	{

	}

	void BaseSceneManager::SystemTick(double delta_time) 
	{
		for(size_t i = 0; i < m_Listeners.size(); i++)
		{
			m_Listeners[i]->SceneManagerTick(delta_time);
		}
	}

	void BaseSceneManager::Register(SceneManagerListenerPtr listener)
	{
		//tbb::spin_mutex::scoped_lock lock(m_Mutex);
		m_Listeners.push_back(listener);
	}

	void BaseSceneManager::Unregister(SceneManagerListenerPtr listener)
	{
		//tbb::spin_mutex::scoped_lock lock(m_Mutex);
		std::vector<SceneManagerListenerPtr>::iterator iter = m_Listeners.begin();
		while(iter != m_Listeners.end())
		{
			if(*iter == listener)
				iter = m_Listeners.erase(iter);
			else
				iter++;
		}
	}

	void BaseSceneManager::LoadXML(TiXmlElement *xml_elem)
	{
		LoadProperties(xml_elem);
	}


	void BaseSceneManager::SaveXML(TiXmlElement *xml_elem)
	{
		TiXmlElement * this_elem;
		this_elem = new TiXmlElement( GetRTTI()->GetClassName().c_str());  
		xml_elem->LinkEndChild( this_elem );  
		SaveProperties(this_elem);
	}
	
}




