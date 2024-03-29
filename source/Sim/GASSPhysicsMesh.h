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

#pragma once

#include "Sim/GASSCommon.h"
#include "Sim/GASSGraphicsMesh.h"
#include "Core/Math/GASSVector.h"

namespace GASS
{
	/**
		Class that hold physics mesh data used by physics systems.
		This class is used to send mesh data to and from physics systems.
	*/
	class GASSExport PhysicsMesh
	{
		public:
		PhysicsMesh();
		PhysicsMesh(const GraphicsMesh &mesh);
		void AddMesh(const GraphicsMesh &mesh);
		std::vector<Vec3> PositionVector;
		std::vector<unsigned int> IndexVector;
		std::vector<unsigned int> MaterialIDVector;
	};
	using PhysicsMeshPtr = std::shared_ptr<PhysicsMesh>;
}
