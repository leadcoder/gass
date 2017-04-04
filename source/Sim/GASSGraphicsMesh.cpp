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

#include "Sim/GASSGraphicsMesh.h"
#include "Core/Math/GASSMath.h"
#include "Core/Math/GASSMatrix.h"
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

	GraphicsSubMeshPtr GraphicsSubMesh::GenerateWireframeEllipsoid(const Vec3 &radius, const ColorRGBA &vertex_color, const std::string &material, int segments)
	{
		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		sub_mesh_data->Type = LINE_LIST;
		sub_mesh_data->MaterialName = material;
		sub_mesh_data->AddWireframeEllipsoid(radius,vertex_color , segments);
		return sub_mesh_data;
	}

	void  GraphicsSubMesh::AddWireframeEllipsoid(const Vec3 &radius, const ColorRGBA &vertex_color, int segments)
	{
		Float samples = segments;
		Float rad = 2*GASS_PI/samples;

		Vec3 pos(0,0,0);
		Float x,y,z;
		for(float i = 0 ;i <= samples; i++)
		{
			x = cos(rad*i)*radius.x;
			y = sin(rad*i)*radius.y;
			pos.Set(x,y,0);
			PositionVector.push_back(pos);
			ColorVector.push_back(vertex_color);

			x = cos(rad*(i+1))*radius.x;
			y = sin(rad*(i+1))*radius.y;
			pos.Set(x,y,0);
			PositionVector.push_back(pos);
			ColorVector.push_back(vertex_color);
		}

		for(float i = 0 ;i <= samples; i++)
		{
			x = cos(rad*i)*radius.x;
			z = sin(rad*i)*radius.z;
			pos.Set(x,0,z);
			PositionVector.push_back(pos);
			ColorVector.push_back(vertex_color);

			x = cos(rad*(i+1))*radius.x;
			z = sin(rad*(i+1))*radius.z;
			pos.Set(x,0,z);
			PositionVector.push_back(pos);
			ColorVector.push_back(vertex_color);

		}

		for(float i = 0 ; i <= samples; i++)
		{
			y = cos(rad*i)*radius.y;
			z = sin(rad*i)*radius.z;
			pos.Set(0,y,z);
			PositionVector.push_back(pos);
			ColorVector.push_back(vertex_color);

			y = cos(rad*(i+1))*radius.y;
			z = sin(rad*(i+1))*radius.z;
			pos.Set(0,y,z);
			PositionVector.push_back(pos);
			ColorVector.push_back(vertex_color);
		}
	}

	GraphicsSubMeshPtr GraphicsSubMesh::GenerateWireframeEllipse(const Vec2 &radius, const ColorRGBA &vertex_color, const std::string &material, int segments)
	{
		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		sub_mesh_data->Type = LINE_LIST;
		sub_mesh_data->MaterialName = material;
		sub_mesh_data->AddWireframeEllipse(radius, vertex_color, segments);
		return sub_mesh_data;
	}



	void GraphicsSubMesh::AddWireframeEllipse(const Vec2 &radius, const ColorRGBA &vertex_color, int segments)
	{
		Float samples = segments;
		Float rad = 2*GASS_PI/samples;

		Vec3 pos(0,0,0);
		Float x,z;

		for(float i = 0 ;i <= samples; i++)
		{
			x = cos(rad*i)*radius.x;
			z = sin(rad*i)*radius.y;
			pos.Set(x,0,z);
			PositionVector.push_back(pos);
			ColorVector.push_back(vertex_color);

			x = cos(rad*(i+1))*radius.x;
			z = sin(rad*(i+1))*radius.y;
			pos.Set(x,0,z);
			PositionVector.push_back(pos);
			ColorVector.push_back(vertex_color);
		}
	}

	GraphicsSubMeshPtr GraphicsSubMesh::GenerateSolidEllipsoid(const Vec3 &radius, const ColorRGBA &vertex_color,const std::string &material, int segments)
	{
		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		sub_mesh_data->Type = TRIANGLE_LIST;
		sub_mesh_data->MaterialName = material;
		sub_mesh_data->AddSolidEllipsoid(radius, vertex_color,segments);
		return sub_mesh_data;
	}

	void GraphicsSubMesh::AddSolidEllipsoid(const Vec3 &radius, const ColorRGBA &vertex_color,int segments)
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
		Vec3 pos(0,0,0);
		//Vertex
		for(i = 0;i <= nSlice;i++)
		{
			phi = 2.0 * GASS_PI * static_cast<double>(i) / static_cast<double>(nSlice);
			for(j = 0;j <= nStack;j++)
			{
				theta = GASS_PI * static_cast<double>(j) / static_cast<double>(nStack);
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
			PositionVector.push_back(pos);
			ColorVector.push_back(vertex_color);
			NormalVector.push_back(normal);

			pos.Set(p2[0],p2[1],p2[2]);
			normal = pos;
			normal.Normalize();
			PositionVector.push_back(pos);
			ColorVector.push_back(vertex_color);
			NormalVector.push_back(normal);

			pos.Set(p3[0],p3[1],p3[2]);
			normal = pos;
			normal.Normalize();
			PositionVector.push_back(pos);
			ColorVector.push_back(vertex_color);
			NormalVector.push_back(normal);
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
			PositionVector.push_back(pos);
			ColorVector.push_back(vertex_color);
			NormalVector.push_back(normal);

			pos.Set(p2[0],p2[1],p2[2]);
			normal = pos;
			normal.Normalize();
			PositionVector.push_back(pos);
			ColorVector.push_back(vertex_color);
			NormalVector.push_back(normal);

			pos.Set(p3[0],p3[1],p3[2]);
			normal = pos;
			normal.Normalize();
			PositionVector.push_back(pos);
			ColorVector.push_back(vertex_color);
			NormalVector.push_back(normal);

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
				PositionVector.push_back(pos);
				ColorVector.push_back(vertex_color);
				NormalVector.push_back(normal);

				pos.Set(p2[0],p2[1],p2[2]);
				normal = pos;
				normal.Normalize();
				PositionVector.push_back(pos);
				ColorVector.push_back(vertex_color);
				NormalVector.push_back(normal);


				pos.Set(p3[0],p3[1],p3[2]);
				normal = pos;
				normal.Normalize();
				PositionVector.push_back(pos);
				ColorVector.push_back(vertex_color);
				NormalVector.push_back(normal);



				pos.Set(p1[0],p1[1],p1[2]);
				normal = pos;
				normal.Normalize();
				PositionVector.push_back(pos);
				ColorVector.push_back(vertex_color);
				NormalVector.push_back(normal);

				pos.Set(p3[0],p3[1],p3[2]);
				normal = pos;
				normal.Normalize();
				PositionVector.push_back(pos);
				ColorVector.push_back(vertex_color);
				NormalVector.push_back(normal);

				pos.Set(p4[0],p4[1],p4[2]);
				normal = pos;
				normal.Normalize();
				PositionVector.push_back(pos);
				ColorVector.push_back(vertex_color);
				NormalVector.push_back(normal);
			}
		}
	}

	GraphicsSubMeshPtr GraphicsSubMesh::GenerateWireframeRectangle(const Vec2 &box_size, const ColorRGBA &vertex_color,const std::string &material)
	{
		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		sub_mesh_data->Type = LINE_LIST;
		sub_mesh_data->MaterialName = material;
		sub_mesh_data->AddWireframeRectangle(box_size, vertex_color);
		return sub_mesh_data;
	}

	void GraphicsSubMesh::AddWireframeRectangle(const Vec2 &box_size, const ColorRGBA &vertex_color)
	{
		Vec3 size;
		size.x = box_size.x*0.5;
		size.y = box_size.y*0.5;
		std::vector<Vec3> conrners;
		conrners.push_back(Vec3( size.x ,0 , size.y));
		conrners.push_back(Vec3(-size.x ,0 , size.y));
		conrners.push_back(Vec3(-size.x ,0 ,-size.y));
		conrners.push_back(Vec3( size.x ,0 ,-size.y));



		for(int i = 0; i < 4; i++)
		{
			Vec3 pos =conrners[i];
			PositionVector.push_back(pos);
			pos =conrners[(i+1)%4];
			PositionVector.push_back(pos);

			pos =conrners[i];
			PositionVector.push_back(pos);
			pos =conrners[i+4];
			PositionVector.push_back(pos);
		}
		for(size_t i = 0; i < PositionVector.size(); i++)
		{
			ColorVector.push_back(vertex_color);
		}
	}

	GraphicsSubMeshPtr GraphicsSubMesh::GenerateWireframeBox(const Vec3 &box_size, const ColorRGBA &vertex_color,const std::string &material)
	{
		//Vec3 size = box_size*0.5;
		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		sub_mesh_data->MaterialName = material;
		sub_mesh_data->Type = LINE_LIST;
		sub_mesh_data->AddWireframeBox(box_size, vertex_color);
		return sub_mesh_data;
	}

	void GraphicsSubMesh::AddWireframeBox(const Vec3 &box_size, const ColorRGBA &vertex_color)
	{
		Vec3 size = box_size*0.5;
		std::vector<Vec3> conrners;
		conrners.push_back(Vec3( size.x ,size.y , size.z));
		conrners.push_back(Vec3(-size.x ,size.y , size.z));
		conrners.push_back(Vec3(-size.x ,size.y ,-size.z));
		conrners.push_back(Vec3( size.x ,size.y ,-size.z));

		conrners.push_back(Vec3( size.x ,-size.y , size.z));
		conrners.push_back(Vec3(-size.x ,-size.y , size.z));
		conrners.push_back(Vec3(-size.x ,-size.y ,-size.z));
		conrners.push_back(Vec3( size.x ,-size.y ,-size.z));


		for(int i = 0; i < 4; i++)
		{
			Vec3 pos =conrners[i];
			PositionVector.push_back(pos);
			pos =conrners[(i+1)%4];
			PositionVector.push_back(pos);

			pos =conrners[i];
			PositionVector.push_back(pos);
			pos =conrners[i+4];
			PositionVector.push_back(pos);
		}

		for(int i = 0; i < 4; i++)
		{
			Vec3 pos = conrners[4 + i];
			PositionVector.push_back(pos);
			pos =conrners[4 + ((i+1)%4)];
			PositionVector.push_back(pos);
		}
		for(size_t i = 0; i < PositionVector.size(); i++)
		{
			ColorVector.push_back(vertex_color);
		}
	}

	GraphicsSubMeshPtr GraphicsSubMesh::GenerateSolidBox(const Vec3 &box_size, const ColorRGBA &vertex_color,const std::string &material)
	{
		//Vec3 size= box_size*0.5;
		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		sub_mesh_data->MaterialName = material;
		sub_mesh_data->Type = TRIANGLE_LIST;
		sub_mesh_data->AddSolidBox(box_size, vertex_color);
		return sub_mesh_data;
	}


	void GraphicsSubMesh::AddSolidBox(const Vec3 &box_size, const ColorRGBA &vertex_color)
	{
		Vec3 size= box_size*0.5;
		std::vector<Vec4> tex_coords;

		PositionVector.push_back(Vec3( -size.x ,-size.y , -size.z));
		PositionVector.push_back(Vec3( -size.x ,-size.y , -size.z));
		PositionVector.push_back(Vec3( -size.x ,-size.y , -size.z));
		tex_coords.push_back(Vec4( 0, 0 , 0,0));
		tex_coords.push_back(Vec4( 0, 0 , 0,0));
		tex_coords.push_back(Vec4( 0, 0 , 0,0));

		PositionVector.push_back(Vec3( size.x ,-size.y , -size.z));
		PositionVector.push_back(Vec3( size.x ,-size.y , -size.z));
		PositionVector.push_back(Vec3( size.x ,-size.y , -size.z));
		tex_coords.push_back(Vec4( 1, 0 , 0,0));
		tex_coords.push_back(Vec4( 1, 0 , 0,0));
		tex_coords.push_back(Vec4( 1, 0 , 0,0));


		PositionVector.push_back(Vec3( size.x ,size.y , -size.z));
		PositionVector.push_back(Vec3( size.x ,size.y , -size.z));
		PositionVector.push_back(Vec3( size.x ,size.y , -size.z));
		tex_coords.push_back(Vec4( 1, 1 , 0,0));
		tex_coords.push_back(Vec4( 1, 1 , 0,0));
		tex_coords.push_back(Vec4( 1, 1 , 0,0));


		PositionVector.push_back(Vec3( -size.x ,size.y , -size.z));
		PositionVector.push_back(Vec3( -size.x ,size.y , -size.z));
		PositionVector.push_back(Vec3( -size.x ,size.y , -size.z));
		tex_coords.push_back(Vec4( 0, 1 , 0,0));
		tex_coords.push_back(Vec4( 0, 1 , 0,0));
		tex_coords.push_back(Vec4( 0, 1 , 0,0));


		PositionVector.push_back(Vec3( -size.x ,-size.y , size.z));
		PositionVector.push_back(Vec3( -size.x ,-size.y , size.z));
		PositionVector.push_back(Vec3( -size.x ,-size.y , size.z));
		tex_coords.push_back(Vec4( 1, 0 , 0,0));
		tex_coords.push_back(Vec4( 1, 0 , 0,0));
		tex_coords.push_back(Vec4( 1, 0 , 0,0));


		PositionVector.push_back(Vec3( size.x ,-size.y , size.z));
		PositionVector.push_back(Vec3( size.x ,-size.y , size.z));
		PositionVector.push_back(Vec3( size.x ,-size.y , size.z));
		tex_coords.push_back(Vec4( 0, 0 , 0,0));
		tex_coords.push_back(Vec4( 0, 0 , 0,0));
		tex_coords.push_back(Vec4( 0, 0 , 0,0));

		PositionVector.push_back(Vec3( size.x ,size.y , size.z));
		PositionVector.push_back(Vec3( size.x ,size.y , size.z));
		PositionVector.push_back(Vec3( size.x ,size.y , size.z));
		tex_coords.push_back(Vec4( 0, 1 , 0,0));
		tex_coords.push_back(Vec4( 0, 1 , 0,0));
		tex_coords.push_back(Vec4( 0, 1 , 0,0));


		PositionVector.push_back(Vec3( -size.x ,size.y , size.z));
		PositionVector.push_back(Vec3( -size.x ,size.y , size.z));
		PositionVector.push_back(Vec3( -size.x ,size.y , size.z));
		tex_coords.push_back(Vec4( 1, 1 , 0,0));
		tex_coords.push_back(Vec4( 1, 1 , 0,0));
		tex_coords.push_back(Vec4( 1, 1 , 0,0));

		NormalVector.push_back(Vec3(0,0,-1));
		NormalVector.push_back(Vec3(-1,0,0));
		NormalVector.push_back(Vec3(0,-1,0));
		NormalVector.push_back(Vec3(0,0,-1));
		NormalVector.push_back(Vec3(1,0,0));
		NormalVector.push_back(Vec3(0,-1,0));
		NormalVector.push_back(Vec3(0,0,-1));
		NormalVector.push_back(Vec3(1,0,0));
		NormalVector.push_back(Vec3(0,1,0));
		NormalVector.push_back(Vec3(0,0,-1));
		NormalVector.push_back(Vec3(-1,0,0));
		NormalVector.push_back(Vec3(0,1,0));
		NormalVector.push_back(Vec3(0,0,1));
		NormalVector.push_back(Vec3(-1,0,0));
		NormalVector.push_back(Vec3(0,-1,0));
		NormalVector.push_back(Vec3(0,0,1));
		NormalVector.push_back(Vec3(1,-0,0));
		NormalVector.push_back(Vec3(0,-1,0));
		NormalVector.push_back(Vec3(0,0,1));
		NormalVector.push_back(Vec3(1,0,0));
		NormalVector.push_back(Vec3(0,1,0));
		NormalVector.push_back(Vec3(-0,0,1));
		NormalVector.push_back(Vec3(-1,-0,-0));
		NormalVector.push_back(Vec3(0,1,0));

		//bottom
		IndexVector.push_back(0);
		IndexVector.push_back(9);
		IndexVector.push_back(3);
		IndexVector.push_back(9);
		IndexVector.push_back(6);
		IndexVector.push_back(3);

		// top
		IndexVector.push_back(21);  //face 3
		IndexVector.push_back(12);
		IndexVector.push_back(18);
		IndexVector.push_back(12);  //face 4
		IndexVector.push_back(15);
		IndexVector.push_back(18);
		// left
		IndexVector.push_back(22);  //face 5
		IndexVector.push_back(10);
		IndexVector.push_back(13);
		IndexVector.push_back(10);  //face 6
		IndexVector.push_back(1);
		IndexVector.push_back(13);
		// right
		IndexVector.push_back(16);  //face 7
		IndexVector.push_back(4);
		IndexVector.push_back(19);
		IndexVector.push_back(4);  //face 8
		IndexVector.push_back(7);
		IndexVector.push_back(19);
		// front
		IndexVector.push_back(14);  //face 9
		IndexVector.push_back(2);
		IndexVector.push_back(17);
		IndexVector.push_back(2);   //face 10
		IndexVector.push_back(5);
		IndexVector.push_back(17);
		// back
		IndexVector.push_back(20);  //face 11
		IndexVector.push_back(8);
		IndexVector.push_back(23);
		IndexVector.push_back(8);   //face 12
		IndexVector.push_back(11);
		IndexVector.push_back(23);
		TexCoordsVector.push_back(tex_coords);

		for(size_t i = 0; i < PositionVector.size(); i++)
		{
			ColorVector.push_back(vertex_color);
		}
	}




	GraphicsSubMeshPtr GraphicsSubMesh::GenerateArrow(const Vec3 &start, const Vec3 &end, Float pointer_size, const ColorRGBA &vertex_color, const std::string &material)
	{
		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		sub_mesh_data->Type = LINE_LIST;
		sub_mesh_data->MaterialName = material;
		sub_mesh_data->AddArrow(start, end, pointer_size, vertex_color);
		return sub_mesh_data;
	}

	void GraphicsSubMesh::AddArrow(const Vec3 &start, const Vec3 &end, Float pointer_size, const ColorRGBA &vertex_color)
	{
		PositionVector.push_back(start);
		PositionVector.push_back(end);

		Vec3 dir = end -start;
		dir.Normalize();
		Vec3 left = dir;
		left.x = dir.z;
		left.z = -dir.x;

		Vec3 p1 = end + (left - dir*2.0)*pointer_size;
		Vec3 p2 = end + (-left - dir*2.0)*pointer_size;

		PositionVector.push_back(p1);
		PositionVector.push_back(end);

		PositionVector.push_back(p2);
		PositionVector.push_back(end);

		for(size_t i = 0; i < PositionVector.size(); i++)
		{
			ColorVector.push_back(vertex_color);
		}
	}

	GraphicsSubMeshPtr GraphicsSubMesh::GenerateLines(const std::vector<Vec3> &lines, const ColorRGBA &vertex_color, const std::string &material, bool strip)
	{
		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		sub_mesh_data->Type = (strip) ?  LINE_STRIP: LINE_LIST;
		sub_mesh_data->MaterialName = material;
		sub_mesh_data->AddLines(lines, vertex_color);
		return sub_mesh_data;
	}

	void GraphicsSubMesh::AddLines(const std::vector<Vec3> &lines, const ColorRGBA &vertex_color)
	{
		PositionVector = lines;
		for(size_t i = 0; i < lines.size(); i++)
		{
			ColorVector.push_back(vertex_color);
		}
	}
}
