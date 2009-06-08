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

#ifndef NULLCLASS_HH
#define NULLCLASS_HH

#include "Core/Common.h"

namespace GASS
{
	class RTTI;

	class GASSCoreExport NullClass
	{
	protected :

		// It is necessary to implement this to avoid compilation errors in the templatized RTTI code
		// In any case no RTTI support is provided from this class
		static inline RTTI* GetClassRTTI()			{ return NULL;};
	};
}

#endif // NULLCLASS_HH
