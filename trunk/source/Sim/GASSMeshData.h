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
	enum MeshType
	{
		LINE_LIST,
		POINT_LIST,
		LINE_STRIP,
		TRIANGLE_FAN,
		TRIANGLE_LIST,
		TRIANGLE_STRIP,
	};
	
	class GASSExport SubMeshData
	{
	public:
		SubMeshData() 
		{

		}
		virtual ~SubMeshData()
		{

		}
		std::vector<Vec3> PositionVector;
		std::vector<Vec3> NormalVector;
		std::vector<Vec3> TangentVector;
		std::vector<ColorRGBA> ColorVector;
		std::vector< std::vector<Vec4> > TexCoordsVector;
		//unsigned int NumFaces;
		std::vector<unsigned int> IndexVector;
		//unsigned int MaterialID;
		GraphicsMaterial Material;
		std::string MaterialName;
		MeshType Type;
	};
	typedef SPTR<SubMeshData> SubMeshDataPtr;
	
	class GASSExport MeshData
	{
	public:
		MeshData();
		virtual ~MeshData();
		void Transform(const Mat4 &transformation );
		std::vector<SubMeshDataPtr> SubMeshVector;
		//std::vector<GraphicsMaterial> MaterialVector;
	};

	typedef SPTR<MeshData> MeshDataPtr;


	class GASSExport PhysicsMesh
	{
		public:
		PhysicsMesh();
		PhysicsMesh(const MeshData &mesh);
		virtual ~PhysicsMesh();
		void AddMesh(const MeshData &mesh);
		std::vector<Vec3> PositionVector;
		std::vector<unsigned int> IndexVector;
		std::vector<unsigned int> MaterialIDVector;
	};
	typedef SPTR<PhysicsMesh> PhysicsMeshPtr;

	/*struct GASSExport MeshData
	{
		unsigned int NumVertex;
		Vec3 *VertexVector;
		Vec3 *NormalVector;
		Vec4 *TexCoordVector;
		unsigned int NumFaces;
		unsigned int *FaceVector;
		unsigned int *FaceMaterialIDVector;
	};
	typedef MeshData* MeshDataPtr;*/


	struct MeshVertex
	{
		Vec3 Pos;
		Vec3 Normal;
		Vec3 Tangent;
		Vec4 Color;
		Vec2 TexCoord;
	};



	/**
	Used by manual mesh
	*/
	struct GASSExport ManualMeshData
	{
		std::vector<MeshVertex> VertexVector;
		std::vector<unsigned int> IndexVector;
		std::string Material;
		MeshType Type;
		bool ScreenSpace;

		ManualMeshData()
		{
			ScreenSpace = false; 
			Type =LINE_LIST;
		}
	};
	typedef SPTR<ManualMeshData> ManualMeshDataPtr;
}
