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

#include "Core/ComponentSystem/GASSComponentContainerFactory.h"

namespace GASS
{
	ComponentContainerFactory::ComponentContainerFactory()
	{

	}

	ComponentContainerFactory::~ComponentContainerFactory()
	{

	}

	ComponentContainerFactory* ComponentContainerFactory::m_Instance = 0;
	ComponentContainerFactory* ComponentContainerFactory::GetPtr(void)
	{
		//assert(m_Instance);
		if(m_Instance == NULL)
			m_Instance = new ComponentContainerFactory();
		return m_Instance;
	}
	ComponentContainerFactory& ComponentContainerFactory::Get(void)
	{
		//assert(m_Instance);
		if(m_Instance == NULL)
			m_Instance = new ComponentContainerFactory();
		return *m_Instance;
	}
}
