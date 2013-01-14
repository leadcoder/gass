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

#include "Sim/GASSCommon.h"
#include "Sim/Interface/GASSIRenderWindow.h"
#include "Core/System/GASSISystem.h"
#include <string>

namespace GASS
{
	

	/**
		Interface that all graphics systems should be derived from.

		Note that interaction with this interface during RTC update is undefined 
		if running GASS in multi-threaded mode. Interaction with systems should 
		instead be done through messages.
	*/
	class GASSExport IGraphicsSceneManager
	{
	public:
		virtual ~IGraphicsSceneManager(){}
		virtual void DrawLine(const Vec3 &start, const Vec3 &end, const Vec4 &color) = 0;
	protected:
	};
	typedef boost::shared_ptr<IGraphicsSceneManager> GraphicsSceneManagerPtr;
}
