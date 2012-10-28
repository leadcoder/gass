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

#include <boost/bind.hpp>
#include "Plugins/Ogre/OgrePolygonMode.h"



namespace GASS
{
	
	template<> std::map<std::string ,Ogre::PolygonMode> SingleEnumBinder<Ogre::PolygonMode,PolygonModeWrapper>::m_NameToEnumMap;
	template<> std::map<Ogre::PolygonMode,std::string> SingleEnumBinder<Ogre::PolygonMode,PolygonModeWrapper>::m_EnumToNameMap;

	PolygonModeWrapper::PolygonModeWrapper() : SingleEnumBinder<Ogre::PolygonMode,PolygonModeWrapper>()
	{

	}

	PolygonModeWrapper::PolygonModeWrapper(Ogre::PolygonMode type) : SingleEnumBinder<Ogre::PolygonMode,PolygonModeWrapper>(type)
	{

	}

	PolygonModeWrapper::~PolygonModeWrapper()
	{

	}

	void PolygonModeWrapper::Register()
	{
		Bind("PM_POINTS", Ogre::PM_POINTS);
		Bind("PM_WIREFRAME", Ogre::PM_WIREFRAME);
		Bind("PM_SOLID", Ogre::PM_SOLID);
	}
}
