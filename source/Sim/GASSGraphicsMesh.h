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
#include "Core/Math/GASSVector.h"
#include "Sim/GASSGraphicsMaterial.h"
#include "Core/Math/GASSMatrix.h"

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

	GASS_FORWARD_DECL(GraphicsSubMesh);
	

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
		GraphicsSubMesh() : Type(TRIANGLE_LIST)
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

		/** Add line ellipsoid shaped geometry*/
		void AddWireframeEllipsoid(const Vec3 &radius, const ColorRGBA &vertex_color, int segments = 30);
		/** Add a triangle ellipsoid shaped geometry*/
		void AddSolidEllipsoid(const Vec3 &radius, const ColorRGBA &vertex_color,int segments = 30);
		/** Add line box shaped geometry*/
		void AddWireframeBox(const Vec3 &box_size, const ColorRGBA &vertex_color);
		/** Add a triangle box shaped geometry*/
		void AddSolidBox(const Vec3 &box_size, const ColorRGBA &vertex_color);

		/** Add line rectangle shaped geometry*/
		void AddWireframeRectangle(const Vec2 &box_size, const ColorRGBA &vertex_color);

		/** Add line ellipse shaped geometry*/
		void AddWireframeEllipse(const Vec2 &radius, const ColorRGBA &vertex_color, int segments);
		
		/** Add a arrow shaped geometry*/
		void AddArrow(const Vec3 &start, const Vec3 &end, Float pointer_size, const ColorRGBA &vertex_color);

		/** Add a path geometry*/
		void AddLines(const std::vector<Vec3> &lines, const ColorRGBA &vertex_color);

		
		//	Helper function to generate allocatate and add geomtries
		static GraphicsSubMeshPtr GenerateWireframeEllipsoid(const Vec3 &radius, const ColorRGBA &vertex_color, const std::string &material="", int segments = 30);
		static GraphicsSubMeshPtr GenerateSolidEllipsoid(const Vec3 &radius, const ColorRGBA &vertex_color,const std::string &material="", int segments = 30);
		static GraphicsSubMeshPtr GenerateWireframeBox(const Vec3 &box_size, const ColorRGBA &vertex_color,const std::string &material = "");
		static GraphicsSubMeshPtr GenerateSolidBox(const Vec3 &box_size, const ColorRGBA &vertex_color,const std::string &material = "");
		static GraphicsSubMeshPtr GenerateWireframeRectangle(const Vec2 &box_size, const ColorRGBA &vertex_color,const std::string &material = "");
		static GraphicsSubMeshPtr GenerateWireframeEllipse(const Vec2 &radius, const ColorRGBA &vertex_color, const std::string &material, int segments);
		static GraphicsSubMeshPtr GenerateArrow(const Vec3 &start, const Vec3 &end, Float pointer_size, const ColorRGBA &vertex_color, const std::string &material);
		static GraphicsSubMeshPtr GenerateLines(const std::vector<Vec3> &lines, const ColorRGBA &vertex_color, const std::string &material,bool strip);
	};
	//typedef GASS_SHARED_PTR<GraphicsSubMesh> GraphicsSubMeshPtr;
	

	/**
		Class that hold graphic mesh data used by scene graph systems.
		This class is used to send data to and from rendering/scene graph systems.
	*/
	class GASSExport GraphicsMesh
	{
	public:
		GraphicsMesh();
		void Transform(const Mat4 &transformation );
		std::vector<GraphicsSubMeshPtr> SubMeshVector;
	};
	typedef GASS_SHARED_PTR<GraphicsMesh> GraphicsMeshPtr;
}
