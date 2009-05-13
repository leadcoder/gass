/****************************************************************************
* This file is part of GASS.                                                *
* See http://sourceforge.net/projects/gass/                                 *
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

#include "Sim/Scenario/Scene/BaseSceneManager.h"
#include "Sim/Systems/SimSystemManager.h"
#include "Core/Common.h"
#include "Core/System/SystemFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/Message.h"
#include "Core/Utils/Log.h"

namespace GASS
{
	BaseSceneManager::BaseSceneManager(void)
	{
		m_Scene = NULL;
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

	void BaseSceneManager::Update(double delta_time) 
	{

	}

	void BaseSceneManager::LoadXML(TiXmlElement *xml_elem)
	{
		LoadProperties(xml_elem);
	}


	void BaseSceneManager::SaveXML(TiXmlElement *xml_elem)
	{
		LoadProperties(xml_elem);
	}
	
}




