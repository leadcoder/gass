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

/*
This class is based on the Game Programming Gems 5 article
"Using Templates for Reflection in C++" by Dominic Filion.
*/


#ifndef GASS_PROPERTY_TYPES_H
#define GASS_PROPERTY_TYPES_H

#include "Core/Common.h"
#include "Core/Math/GASSVector.h"
#include "Core/Utils/GASSEnumBinder.h"

namespace GASS
{
	enum PropertyType
	{
		PROP_BOOL,
		PROP_INT,
		PROP_FLOAT,
		PROP_STRING,
		PROP_VEC2,
		PROP_VEC3,
		PROP_VEC4,
		PROP_UNKNOWN,
		PROP_MAX_PROPERTY_TYPES
	};
}
#endif
