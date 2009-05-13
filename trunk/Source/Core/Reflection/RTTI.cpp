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

#include "Core/Reflection/RTTI.h"
#include "assert.h"

namespace GASS
{
	RTTI::RTTI(const std::string &class_name, RTTI* base_class_RTTI, 
		ClassFactoryFunc factory, RegisterReflectionFunc reflection_func ) : 
	m_BaseRTTI(base_class_RTTI),
		m_ObjectFactory(factory),
		m_ClassName(class_name)
	{
		if ( reflection_func)
			reflection_func();
	}

	void RTTI::EnumProperties( std::vector<AbstractProperty*>& o_Result )
	{
		if ( m_BaseRTTI )
			m_BaseRTTI->EnumProperties( o_Result );
		for ( std::list<AbstractProperty*>::iterator it = m_Properties.begin(); it != m_Properties.end(); ++it )
			o_Result.push_back( *it );
	}
}