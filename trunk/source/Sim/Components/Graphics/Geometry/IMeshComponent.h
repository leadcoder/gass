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
#include "Sim/Components/Graphics/MeshData.h"

namespace GASS
{
	/**
		Interface that all mesh components should be derived from.
		Note that all runtime interaction should be done through 
		messages if running multi-threaded.
		
	*/
	class GASSExport IMeshComponent
	{
	public:
		virtual ~IMeshComponent(){}

		/**
		Should if possible return the filename of the mesh if available.
		If the mesh is prodecural created just return empty string
		*/
		virtual std::string GetFilename() const=0;

		/**
		Should return actual mesh data (polygons), see MeshData for more info
		*/
		virtual void GetMeshData(MeshDataPtr mesh_data) const =0;
	};
	typedef boost::shared_ptr<IMeshComponent> MeshComponentPtr;
}

