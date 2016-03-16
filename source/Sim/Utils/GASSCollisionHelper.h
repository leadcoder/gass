/****************************************************************************
* This file is part of GASS.                                                *
* See https://github.com/leadcoder/gass                                     *
*                                                                           *
* Copyright (c) 2008-2016 GASS team. See Contributors.txt for details.      *
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

#ifndef GASS_COLLISION_HELEPR_H
#define GASS_COLLISION_HELEPR_H

#include "Sim/GASSCommon.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSGeometryFlags.h"
#include "Core/Utils/GASSTimer.h"

namespace GASS
{
	class GASSExport CollisionHelper
	{
	public:
		CollisionHelper();
		~CollisionHelper();
		static Float GetHeightAtPosition(ScenePtr scene, const Vec3 &pos, GeometryFlags flags, bool absolute = true);
	protected:
	};
}

#endif
