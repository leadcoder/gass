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

#ifndef PROPERTYSYSTEM_HH
#define PROPERTYSYSTEM_HH

#include "Core/Common.h"
#include <list>
#include "Core/Reflection/Property.h"

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// PropertySystem
//
// Global manager of all properties. Used to release properties.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
namespace GASS
{
	class GASSCoreExport PropertySystem
	{

	public :

		//----------------------------------------------------------------------------------------------
		// Constructor
		PropertySystem();
		//----------------------------------------------------------------------------------------------
		// Destructor
		virtual ~PropertySystem();

		//----------------------------------------------------------------------------------------------
		// Returns global list of properties.
		inline	static	list<AbstractProperty*>*	GetProperties();

	private :

		static	list<AbstractProperty*>	ms_Properties;

	};

	inline list<AbstractProperty*>* PropertySystem::GetProperties()
	{
		return &ms_Properties;
	}

}
#endif // #ifndef PROPERTYSYSTEM_HH
