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

#pragma once


#include "Sim/Common.h"
#include "Sim/Utils/EnumBinder.h"
#include <OgreCommon.h>

namespace GASS
{
	class PolygonModeWrapper : public EnumBinder<Ogre::PolygonMode,PolygonModeWrapper>
	{
	public:
		PolygonModeWrapper();
		PolygonModeWrapper(Ogre::PolygonMode type);
		virtual ~PolygonModeWrapper();
		static void Register();
	};
	typedef boost::shared_ptr<PolygonModeWrapper> PolygonModeWrapperPtr;
}

