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

#include "PlaneGeometryComponent.h"

#include <memory>
#include "Sim/GASSComponentFactory.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Interface/GASSIManualMeshComponent.h"
#include "Sim/GASSGraphicsMesh.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"

namespace GASS
{
	PlaneGeometryComponent::PlaneGeometryComponent(void) : m_Size(1,1)
	{

	}

	PlaneGeometryComponent::~PlaneGeometryComponent(void)
	{
		
	}

	void PlaneGeometryComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<PlaneGeometryComponent>();
		RegisterGetSet("Size", &GASS::PlaneGeometryComponent::GetSize, &GASS::PlaneGeometryComponent::SetSize);
		RegisterGetSet("Transparency", &GASS::PlaneGeometryComponent::GetTransparency, &GASS::PlaneGeometryComponent::SetTransparency);
		RegisterGetSet("Texture", &GASS::PlaneGeometryComponent::GetTexture, &GASS::PlaneGeometryComponent::SetTexture);
	}

	void PlaneGeometryComponent::OnInitialize()
	{
		GenerateMesh();
		SetTexture(m_Texture);
	}


	Vec2 PlaneGeometryComponent::GetSize() const
	{
		return m_Size;
	}

	void PlaneGeometryComponent::SetSize(const Vec2 &value)
	{
		m_Size = value;
		if(GetSceneObject()) //initialized
		{
			GenerateMesh();
			SetTexture(m_Texture);
		}
	}

	float PlaneGeometryComponent::GetTransparency() const
	{
		return m_Transparency;
	}

	void PlaneGeometryComponent::SetTransparency(float value)
	{
		m_Transparency = value;
		if(GetSceneObject()) //initlized
		{
			GenerateMesh();
			SetTexture(m_Texture);
		}
	}

	void PlaneGeometryComponent::GenerateMesh()
	{
		const Vec2 size(m_Size.x*0.5,m_Size.y*0.5);
		GraphicsMeshPtr mesh_data(new GraphicsMesh());
		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		mesh_data->SubMeshVector.push_back(sub_mesh_data);
		sub_mesh_data->MaterialConfig.reset(new PhongMaterialConfig(ColorRGBA(1,1,1,1)));
		
		const ColorRGBA color(1,1,1,m_Transparency);
		sub_mesh_data->Type = TRIANGLE_LIST;

		std::vector<Vec4> tex_coords;
		
		sub_mesh_data->PositionVector.emplace_back( size.x ,0 , size.y);
		sub_mesh_data->NormalVector.emplace_back( 0, 1, 0);
		sub_mesh_data->ColorVector.push_back(color);
		tex_coords.emplace_back(1,1,0,0);
		
		sub_mesh_data->PositionVector.emplace_back(-size.x ,0 , size.y);
		sub_mesh_data->NormalVector.emplace_back( 0, 1, 0);
		sub_mesh_data->ColorVector.push_back(color);
		tex_coords.emplace_back(0,1,0,0);

		sub_mesh_data->PositionVector.emplace_back(-size.x ,0 , -size.y);
		sub_mesh_data->NormalVector.emplace_back( 0, 1, 0);
		sub_mesh_data->ColorVector.push_back(color);
		tex_coords.emplace_back(0,0,0,0);
		
		sub_mesh_data->PositionVector.emplace_back(size.x ,0 , -size.y);
		sub_mesh_data->NormalVector.emplace_back( 0, 1, 0);
		sub_mesh_data->ColorVector.push_back(color);
		tex_coords.emplace_back(1,0,0,0);
		
		sub_mesh_data->TexCoordsVector.push_back(tex_coords);

		sub_mesh_data->IndexVector.push_back(2);
		sub_mesh_data->IndexVector.push_back(1);
		sub_mesh_data->IndexVector.push_back(0);

		sub_mesh_data->IndexVector.push_back(3);
		sub_mesh_data->IndexVector.push_back(2);
		sub_mesh_data->IndexVector.push_back(0);

		GetSceneObject()->GetFirstComponentByClass<IManualMeshComponent>()->SetMeshData(*mesh_data);
	}
	
	

	void PlaneGeometryComponent::SetTexture(const std::string &texture_name)
	{
		m_Texture = texture_name;
	}

	std::string PlaneGeometryComponent::GetTexture() const
	{
		return m_Texture;
	}
}
