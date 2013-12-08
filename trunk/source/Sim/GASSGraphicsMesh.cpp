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

#include "Sim/GASSGraphicsMesh.h"

namespace GASS
{
	GraphicsMesh::GraphicsMesh() 
	{

	}

	GraphicsMesh::~GraphicsMesh()
	{

	}


	void GraphicsMesh::Transform(const Mat4 &transformation )
	{
		Mat4 rot_mat = transformation;
		rot_mat.SetTranslation(0,0,0);
		for(size_t i = 0; i < SubMeshVector.size() ;i++)
		{
			//Transform all positions
			for(size_t j = 0; j < SubMeshVector[i]->PositionVector.size() ; j++)
			{
				SubMeshVector[i]->PositionVector[j] = transformation * SubMeshVector[i]->PositionVector[j];
			}

			//Transform all normals, only rotation
			for(size_t j = 0; j < SubMeshVector[i]->NormalVector.size() ; j++)
			{
				SubMeshVector[i]->NormalVector[j] = rot_mat * SubMeshVector[i]->NormalVector[j];
			}
			//Transform all tangents, only rotation
			for(size_t j = 0; j < SubMeshVector[i]->TangentVector.size() ; j++)
			{
				SubMeshVector[i]->TangentVector[j] = rot_mat * SubMeshVector[i]->TangentVector[j];
			}
		}
	}

	GraphicsSubMeshPtr GraphicsMesh::GenerateWireframeEllipsoid(const Vec3 &radius, const ColorRGBA &vertex_color, const std::string &material, int segments)
	{
		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		sub_mesh_data->Type = LINE_LIST;
		sub_mesh_data->MaterialName = "WhiteTransparentNoLighting";

		Float samples = segments;
		Float rad = 2*MY_PI/samples;

		Vec3 pos(0,0,0);
		Float x,y,z;
		for(float i = 0 ;i <= samples; i++)
		{
			x = cos(rad*i)*radius.x;
			y = sin(rad*i)*radius.y;
			pos.Set(x,y,0);
			sub_mesh_data->PositionVector.push_back(pos);
			sub_mesh_data->ColorVector.push_back(vertex_color);

			x = cos(rad*(i+1))*radius.x;
			y = sin(rad*(i+1))*radius.y;
			pos.Set(x,y,0);
			sub_mesh_data->PositionVector.push_back(pos);
			sub_mesh_data->ColorVector.push_back(vertex_color);
		}
		//sub_mesh_data->PositionVector.push_back(pos);
		//sub_mesh_data->ColorVector.push_back(vertex_color);

		for(float i = 0 ;i <= samples; i++)
		{
			x = cos(rad*i)*radius.x;
			z = sin(rad*i)*radius.z;
			pos.Set(x,0,z);
			sub_mesh_data->PositionVector.push_back(pos);
			sub_mesh_data->ColorVector.push_back(vertex_color);

			x = cos(rad*(i+1))*radius.x;
			z = sin(rad*(i+1))*radius.z;
			pos.Set(x,0,z);
			sub_mesh_data->PositionVector.push_back(pos);
			sub_mesh_data->ColorVector.push_back(vertex_color);

		}
		//sub_mesh_data->PositionVector.push_back(pos);
		//sub_mesh_data->ColorVector.push_back(vertex_color);

		for(float i = 0 ; i <= samples; i++)
		{
			y = cos(rad*i)*radius.y;
			z = sin(rad*i)*radius.z;
			pos.Set(0,y,z);
			sub_mesh_data->PositionVector.push_back(pos);
			sub_mesh_data->ColorVector.push_back(vertex_color);

			y = cos(rad*(i+1))*radius.y;
			z = sin(rad*(i+1))*radius.z;
			pos.Set(0,y,z);
			sub_mesh_data->PositionVector.push_back(pos);
			sub_mesh_data->ColorVector.push_back(vertex_color);
		}
		//sub_mesh_data->PositionVector.push_back(pos);
		//sub_mesh_data->ColorVector.push_back(vertex_color);
		return sub_mesh_data;
	}

	GraphicsSubMeshPtr GraphicsMesh::GenerateSolidEllipsoid(const Vec3 &radius, const ColorRGBA &vertex_color,const std::string &material, int segments)
	{
		int i, j;
		Float phi; //
		Float theta; //long
		Float p[31][31][3]; //Vertex
		Float *p1,*p2,*p3,*p4;
		int  nSlice = segments;
		int  nStack = segments;

		if(nSlice > 30) nSlice = 30;
		if(nStack > 30) nStack = 30;

		GraphicsMeshPtr mesh_data(new GraphicsMesh());
		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		mesh_data->SubMeshVector.push_back(sub_mesh_data);
		sub_mesh_data->Type = TRIANGLE_LIST;
		sub_mesh_data->MaterialName = "WhiteTransparentNoLighting";
		Vec3 pos(0,0,0);
		//Vertex
		for(i = 0;i <= nSlice;i++)
		{   
			phi = 2.0 * MY_PI * (double)i / (double)nSlice;
			for(j = 0;j <= nStack;j++)
			{   
				theta = MY_PI * (double)j / (double)nStack;
				p[i][j][0] = (radius.x * sin(theta) * cos(phi));//x
				p[i][j][1] = (radius.y * sin(theta) * sin(phi));//y
				p[i][j][2] = (radius.z * cos(theta));           //z
			}
		}

		Vec3 normal;
		//Top(j=0)
		for(i = 0;i < nSlice; i++)
		{
			p1 = p[i][0];     p2 = p[i][1];
			p3 = p[i+1][1]; 

			pos.Set(p1[0],p1[1],p1[2]);
			normal = pos;
			normal.Normalize();
			sub_mesh_data->PositionVector.push_back(pos);
			sub_mesh_data->ColorVector.push_back(vertex_color);
			sub_mesh_data->NormalVector.push_back(normal);

			pos.Set(p2[0],p2[1],p2[2]);
			normal = pos;
			normal.Normalize();
			sub_mesh_data->PositionVector.push_back(pos);
			sub_mesh_data->ColorVector.push_back(vertex_color);
			sub_mesh_data->NormalVector.push_back(normal);

			pos.Set(p3[0],p3[1],p3[2]);
			normal = pos;
			normal.Normalize();
			sub_mesh_data->PositionVector.push_back(pos);
			sub_mesh_data->ColorVector.push_back(vertex_color);
			sub_mesh_data->NormalVector.push_back(normal);
		}
		//Bottom
		j=nStack-1;
		for(i = 0;i < nSlice; i++)
		{
			p1 = p[i][j];     p2 = p[i][j+1];
			p3 = p[i+1][j]; 

			pos.Set(p1[0],p1[1],p1[2]);
			normal = pos;
			normal.Normalize();
			sub_mesh_data->PositionVector.push_back(pos);
			sub_mesh_data->ColorVector.push_back(vertex_color);
			sub_mesh_data->NormalVector.push_back(normal);

			pos.Set(p2[0],p2[1],p2[2]);
			normal = pos;
			normal.Normalize();
			sub_mesh_data->PositionVector.push_back(pos);
			sub_mesh_data->ColorVector.push_back(vertex_color);
			sub_mesh_data->NormalVector.push_back(normal);

			pos.Set(p3[0],p3[1],p3[2]);
			normal = pos;
			normal.Normalize();
			sub_mesh_data->PositionVector.push_back(pos);
			sub_mesh_data->ColorVector.push_back(vertex_color);
			sub_mesh_data->NormalVector.push_back(normal);

		}

		for(i = 0;i < nSlice;i++)
		{
			for(j = 1;j < nStack-1; j++)
			{
				p1 = p[i][j];     p2 = p[i][j+1];
				p3 = p[i+1][j+1]; p4 = p[i+1][j];


				pos.Set(p1[0],p1[1],p1[2]);
				normal = pos;
				normal.Normalize();
				sub_mesh_data->PositionVector.push_back(pos);
				sub_mesh_data->ColorVector.push_back(vertex_color);
				sub_mesh_data->NormalVector.push_back(normal);

				pos.Set(p2[0],p2[1],p2[2]);
				normal = pos;
				normal.Normalize();
				sub_mesh_data->PositionVector.push_back(pos);
				sub_mesh_data->ColorVector.push_back(vertex_color);
				sub_mesh_data->NormalVector.push_back(normal);


				pos.Set(p3[0],p3[1],p3[2]);
				normal = pos;
				normal.Normalize();
				sub_mesh_data->PositionVector.push_back(pos);
				sub_mesh_data->ColorVector.push_back(vertex_color);
				sub_mesh_data->NormalVector.push_back(normal);



				pos.Set(p1[0],p1[1],p1[2]);
				normal = pos;
				normal.Normalize();
				sub_mesh_data->PositionVector.push_back(pos);
				sub_mesh_data->ColorVector.push_back(vertex_color);
				sub_mesh_data->NormalVector.push_back(normal);

				pos.Set(p3[0],p3[1],p3[2]);
				normal = pos;
				normal.Normalize();
				sub_mesh_data->PositionVector.push_back(pos);
				sub_mesh_data->ColorVector.push_back(vertex_color);
				sub_mesh_data->NormalVector.push_back(normal);

				pos.Set(p4[0],p4[1],p4[2]);
				normal = pos;
				normal.Normalize();
				sub_mesh_data->PositionVector.push_back(pos);
				sub_mesh_data->ColorVector.push_back(vertex_color);
				sub_mesh_data->NormalVector.push_back(normal);
			}
		}
		return sub_mesh_data;
	}


	GraphicsSubMeshPtr GraphicsMesh::GenerateWireframeBox(const Vec3 &box_size, const ColorRGBA &vertex_color,const std::string &material)
	{
		Vec3 size = box_size*0.5;
		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		sub_mesh_data->MaterialName = material;
		std::vector<Vec3> conrners;
		conrners.push_back(Vec3( size.x ,size.y , size.z));
		conrners.push_back(Vec3(-size.x ,size.y , size.z));
		conrners.push_back(Vec3(-size.x ,size.y ,-size.z));
		conrners.push_back(Vec3( size.x ,size.y ,-size.z));

		conrners.push_back(Vec3( size.x ,-size.y , size.z));
		conrners.push_back(Vec3(-size.x ,-size.y , size.z));
		conrners.push_back(Vec3(-size.x ,-size.y ,-size.z));
		conrners.push_back(Vec3( size.x ,-size.y ,-size.z));

		sub_mesh_data->Type = LINE_LIST;

		for(int i = 0; i < 4; i++)
		{
			Vec3 pos =conrners[i];
			sub_mesh_data->PositionVector.push_back(pos);
			pos =conrners[(i+1)%4];
			sub_mesh_data->PositionVector.push_back(pos);

			pos =conrners[i];
			sub_mesh_data->PositionVector.push_back(pos);
			pos =conrners[i+4];
			sub_mesh_data->PositionVector.push_back(pos);
		}

		for(int i = 0; i < 4; i++)
		{
			Vec3 pos = conrners[4 + i];
			sub_mesh_data->PositionVector.push_back(pos);
			pos =conrners[4 + ((i+1)%4)];
			sub_mesh_data->PositionVector.push_back(pos);
		}
		for(size_t i = 0; i < sub_mesh_data->PositionVector.size(); i++)
		{
			sub_mesh_data->ColorVector.push_back(vertex_color);
		}
		return sub_mesh_data;
	}

	GraphicsSubMeshPtr GraphicsMesh::GenerateSolidBox(const Vec3 &box_size, const ColorRGBA &vertex_color,const std::string &material)
	{
		Vec3 size= box_size*0.5;
		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		sub_mesh_data->MaterialName = material;

		sub_mesh_data->Type = TRIANGLE_LIST;
		std::vector<Vec4> tex_coords;

		sub_mesh_data->PositionVector.push_back(Vec3( -size.x ,-size.y , -size.z));
		sub_mesh_data->PositionVector.push_back(Vec3( -size.x ,-size.y , -size.z));
		sub_mesh_data->PositionVector.push_back(Vec3( -size.x ,-size.y , -size.z));
		tex_coords.push_back(Vec4( 0, 0 , 0,0));
		tex_coords.push_back(Vec4( 0, 0 , 0,0));
		tex_coords.push_back(Vec4( 0, 0 , 0,0));

		sub_mesh_data->PositionVector.push_back(Vec3( size.x ,-size.y , -size.z));
		sub_mesh_data->PositionVector.push_back(Vec3( size.x ,-size.y , -size.z));
		sub_mesh_data->PositionVector.push_back(Vec3( size.x ,-size.y , -size.z));
		tex_coords.push_back(Vec4( 1, 0 , 0,0));
		tex_coords.push_back(Vec4( 1, 0 , 0,0));
		tex_coords.push_back(Vec4( 1, 0 , 0,0));


		sub_mesh_data->PositionVector.push_back(Vec3( size.x ,size.y , -size.z));
		sub_mesh_data->PositionVector.push_back(Vec3( size.x ,size.y , -size.z));
		sub_mesh_data->PositionVector.push_back(Vec3( size.x ,size.y , -size.z));
		tex_coords.push_back(Vec4( 1, 1 , 0,0));
		tex_coords.push_back(Vec4( 1, 1 , 0,0));
		tex_coords.push_back(Vec4( 1, 1 , 0,0));


		sub_mesh_data->PositionVector.push_back(Vec3( -size.x ,size.y , -size.z));
		sub_mesh_data->PositionVector.push_back(Vec3( -size.x ,size.y , -size.z));
		sub_mesh_data->PositionVector.push_back(Vec3( -size.x ,size.y , -size.z));
		tex_coords.push_back(Vec4( 0, 1 , 0,0));
		tex_coords.push_back(Vec4( 0, 1 , 0,0));
		tex_coords.push_back(Vec4( 0, 1 , 0,0));


		sub_mesh_data->PositionVector.push_back(Vec3( -size.x ,-size.y , size.z));
		sub_mesh_data->PositionVector.push_back(Vec3( -size.x ,-size.y , size.z));
		sub_mesh_data->PositionVector.push_back(Vec3( -size.x ,-size.y , size.z));
		tex_coords.push_back(Vec4( 1, 0 , 0,0));
		tex_coords.push_back(Vec4( 1, 0 , 0,0));
		tex_coords.push_back(Vec4( 1, 0 , 0,0));


		sub_mesh_data->PositionVector.push_back(Vec3( size.x ,-size.y , size.z));
		sub_mesh_data->PositionVector.push_back(Vec3( size.x ,-size.y , size.z));
		sub_mesh_data->PositionVector.push_back(Vec3( size.x ,-size.y , size.z));
		tex_coords.push_back(Vec4( 0, 0 , 0,0));
		tex_coords.push_back(Vec4( 0, 0 , 0,0));
		tex_coords.push_back(Vec4( 0, 0 , 0,0));

		sub_mesh_data->PositionVector.push_back(Vec3( size.x ,size.y , size.z));
		sub_mesh_data->PositionVector.push_back(Vec3( size.x ,size.y , size.z));
		sub_mesh_data->PositionVector.push_back(Vec3( size.x ,size.y , size.z));
		tex_coords.push_back(Vec4( 0, 1 , 0,0));
		tex_coords.push_back(Vec4( 0, 1 , 0,0));
		tex_coords.push_back(Vec4( 0, 1 , 0,0));


		sub_mesh_data->PositionVector.push_back(Vec3( -size.x ,size.y , size.z));
		sub_mesh_data->PositionVector.push_back(Vec3( -size.x ,size.y , size.z));
		sub_mesh_data->PositionVector.push_back(Vec3( -size.x ,size.y , size.z));
		tex_coords.push_back(Vec4( 1, 1 , 0,0));
		tex_coords.push_back(Vec4( 1, 1 , 0,0));
		tex_coords.push_back(Vec4( 1, 1 , 0,0));

		sub_mesh_data->NormalVector.push_back(Vec3(0,0,-1));
		sub_mesh_data->NormalVector.push_back(Vec3(-1,0,0));
		sub_mesh_data->NormalVector.push_back(Vec3(0,-1,0));
		sub_mesh_data->NormalVector.push_back(Vec3(0,0,-1));
		sub_mesh_data->NormalVector.push_back(Vec3(1,0,0));
		sub_mesh_data->NormalVector.push_back(Vec3(0,-1,0));
		sub_mesh_data->NormalVector.push_back(Vec3(0,0,-1));
		sub_mesh_data->NormalVector.push_back(Vec3(1,0,0));
		sub_mesh_data->NormalVector.push_back(Vec3(0,1,0));
		sub_mesh_data->NormalVector.push_back(Vec3(0,0,-1));
		sub_mesh_data->NormalVector.push_back(Vec3(-1,0,0));
		sub_mesh_data->NormalVector.push_back(Vec3(0,1,0));
		sub_mesh_data->NormalVector.push_back(Vec3(0,0,1));
		sub_mesh_data->NormalVector.push_back(Vec3(-1,0,0));
		sub_mesh_data->NormalVector.push_back(Vec3(0,-1,0));
		sub_mesh_data->NormalVector.push_back(Vec3(0,0,1));
		sub_mesh_data->NormalVector.push_back(Vec3(1,-0,0));
		sub_mesh_data->NormalVector.push_back(Vec3(0,-1,0));
		sub_mesh_data->NormalVector.push_back(Vec3(0,0,1));
		sub_mesh_data->NormalVector.push_back(Vec3(1,0,0));
		sub_mesh_data->NormalVector.push_back(Vec3(0,1,0));
		sub_mesh_data->NormalVector.push_back(Vec3(-0,0,1));
		sub_mesh_data->NormalVector.push_back(Vec3(-1,-0,-0));
		sub_mesh_data->NormalVector.push_back(Vec3(0,1,0));

		//bottom
		sub_mesh_data->IndexVector.push_back(0);
		sub_mesh_data->IndexVector.push_back(9);
		sub_mesh_data->IndexVector.push_back(3);
		sub_mesh_data->IndexVector.push_back(9);
		sub_mesh_data->IndexVector.push_back(6);
		sub_mesh_data->IndexVector.push_back(3);

		// top
		sub_mesh_data->IndexVector.push_back(21);  //face 3
		sub_mesh_data->IndexVector.push_back(12);
		sub_mesh_data->IndexVector.push_back(18);
		sub_mesh_data->IndexVector.push_back(12);  //face 4
		sub_mesh_data->IndexVector.push_back(15);
		sub_mesh_data->IndexVector.push_back(18);
		// left
		sub_mesh_data->IndexVector.push_back(22);  //face 5
		sub_mesh_data->IndexVector.push_back(10);
		sub_mesh_data->IndexVector.push_back(13);
		sub_mesh_data->IndexVector.push_back(10);  //face 6
		sub_mesh_data->IndexVector.push_back(1);
		sub_mesh_data->IndexVector.push_back(13);
		// right
		sub_mesh_data->IndexVector.push_back(16);  //face 7
		sub_mesh_data->IndexVector.push_back(4);
		sub_mesh_data->IndexVector.push_back(19);
		sub_mesh_data->IndexVector.push_back(4);  //face 8
		sub_mesh_data->IndexVector.push_back(7);
		sub_mesh_data->IndexVector.push_back(19);
		// front
		sub_mesh_data->IndexVector.push_back(14);  //face 9
		sub_mesh_data->IndexVector.push_back(2);
		sub_mesh_data->IndexVector.push_back(17);
		sub_mesh_data->IndexVector.push_back(2);   //face 10
		sub_mesh_data->IndexVector.push_back(5);
		sub_mesh_data->IndexVector.push_back(17);
		// back
		sub_mesh_data->IndexVector.push_back(20);  //face 11
		sub_mesh_data->IndexVector.push_back(8);
		sub_mesh_data->IndexVector.push_back(23);
		sub_mesh_data->IndexVector.push_back(8);   //face 12
		sub_mesh_data->IndexVector.push_back(11);
		sub_mesh_data->IndexVector.push_back(23);
		sub_mesh_data->TexCoordsVector.push_back(tex_coords);

		for(size_t i = 0; i < sub_mesh_data->PositionVector.size(); i++)
		{
			sub_mesh_data->ColorVector.push_back(vertex_color);
		}
		return sub_mesh_data;
	}
}
