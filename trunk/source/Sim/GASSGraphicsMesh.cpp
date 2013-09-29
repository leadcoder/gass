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
			for(size_t j = 0; j < SubMeshVector[i]->PositionVector.size() ; j++)
			{
				SubMeshVector[i]->PositionVector[j] = transformation * SubMeshVector[i]->PositionVector[j];
			}

			for(size_t j = 0; j < SubMeshVector[i]->NormalVector.size() ; j++)
			{
				SubMeshVector[i]->NormalVector[j] = rot_mat * SubMeshVector[i]->NormalVector[j];
			}

			for(size_t j = 0; j < SubMeshVector[i]->TangentVector.size() ; j++)
			{
				SubMeshVector[i]->TangentVector[j] = rot_mat * SubMeshVector[i]->TangentVector[j];
			}
		}

		//Transform all normals, only rotation
	}

	PhysicsMesh::PhysicsMesh(const GraphicsMesh &mesh)
	{
		AddMesh(mesh);
	}

	PhysicsMesh::~PhysicsMesh()
	{

	}

	void PhysicsMesh::AddMesh(const GraphicsMesh &mesh)
	{
		for(size_t i = 0; i < mesh.SubMeshVector.size() ;i++)
		{
			//only support triangle data
			if(mesh.SubMeshVector[i]->Type == TRIANGLE_LIST)
			{
				unsigned int offset = PositionVector.size();

				for(size_t j = 0; j < mesh.SubMeshVector[i]->IndexVector.size() ; j++)
				{
					IndexVector.push_back(mesh.SubMeshVector[i]->IndexVector[j] + offset);
				}

				for(size_t j = 0; j < mesh.SubMeshVector[i]->PositionVector.size() ; j++)
				{
					PositionVector.push_back(mesh.SubMeshVector[i]->PositionVector[j]);
				}

				for(size_t j = 0; j < mesh.SubMeshVector[i]->IndexVector.size()/3 ; j++)
				{
					MaterialIDVector.push_back(0);
				}
			}
		}
	}
}
