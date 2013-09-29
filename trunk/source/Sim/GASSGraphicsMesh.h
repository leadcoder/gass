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
#include "Core/Math/GASSVector.h"
#include "Sim/GASSGraphicsMaterial.h"

namespace GASS
{
	/**
		Sub mesh type enumeration
	*/
	enum GraphicsSubMeshType
	{
		LINE_LIST,
		POINT_LIST,
		LINE_STRIP,
		TRIANGLE_FAN,
		TRIANGLE_LIST,
		TRIANGLE_STRIP,
	};
	

	/**
		\brief Class holding Sub Meshe data.
		A Mesh is separated in submeshes, each sub mesh share 
		the same material and same rendering/data mode, ie triangles, lines, points etc.
		The submesh class hold vertex data separetes into diffrent vectors,
		position vector, normal vector ect. This reflects how data is stored in many scene graoh sysstems
	*/
	class GASSExport GraphicsSubMesh
	{
	public:
		GraphicsSubMesh() 
		{

		}
		virtual ~GraphicsSubMesh()
		{

		}
		/**
			Vector holding all positions
		*/
		std::vector<Vec3> PositionVector;

		/**
			Vector holding all normal data
		*/
		std::vector<Vec3> NormalVector;
		
		/**
			Vector holding all tangent data
		*/
		std::vector<Vec3> TangentVector;
		
		/**
			Vector holding all color data
		*/
		std::vector<ColorRGBA> ColorVector;
		
		/**
			Vector holding texture coordinates. A sub mesh can have multiples
			texture coordinates for each vertex
		*/
		std::vector< std::vector<Vec4> > TexCoordsVector;

		/**
			Index vector that specify how vertex data should be  interpreted
		*/
		std::vector<unsigned int> IndexVector;

		/**
			Material of used by this sub mesh, this just a shallow
			material representation that only support som basic 
			fixed function rendering material properties.
		*/
		GraphicsMaterial Material;

		/**
			The material name used in the scene graph system,
		*/
		std::string MaterialName;

		/**
			Sub mesh type
		*/
		GraphicsSubMeshType Type;
	};
	typedef SPTR<GraphicsSubMesh> GraphicsSubMeshPtr;
	

	/**
		Class that hold graphic mesh data used by scene graph systems.
		This class is used to send data to and from rendering/scene graph systems.
	*/
	class GASSExport GraphicsMesh
	{
	public:
		GraphicsMesh();
		virtual ~GraphicsMesh();
		void Transform(const Mat4 &transformation );
		std::vector<GraphicsSubMeshPtr> SubMeshVector;
	};
	typedef SPTR<GraphicsMesh> GraphicsMeshPtr;
}
