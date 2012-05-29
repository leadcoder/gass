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

#include "PlaneGeometryComponent.h"
#include "Plugins/Base/CoreMessages.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/Scene/GASSScene.h"
#include "Sim/Scene/GASSSceneObject.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Systems/Resource/GASSIResourceSystem.h"
#include "Sim/Systems/GASSSimSystemManager.h"
#include "Sim/Components/Graphics/Geometry/GASSIGeometryComponent.h"
#include "Sim/Components/Graphics/GASSMeshData.h"

#include "Sim/Scene/GASSGraphicsSceneObjectMessages.h"


namespace GASS
{
	PlaneGeometryComponent::PlaneGeometryComponent(void) : m_Size(1,1),m_Transparency(1)
	{

	}

	PlaneGeometryComponent::~PlaneGeometryComponent(void)
	{
		
	}

	void PlaneGeometryComponent::RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("PlaneGeometryComponent",new GASS::Creator<PlaneGeometryComponent, IComponent>);
		RegisterProperty<Vec2>("Size", &GASS::PlaneGeometryComponent::GetSize, &GASS::PlaneGeometryComponent::SetSize);
		RegisterProperty<float>("Transparency", &GASS::PlaneGeometryComponent::GetTransparency, &GASS::PlaneGeometryComponent::SetTransparency);
		RegisterProperty<std::string>("Texture", &GASS::PlaneGeometryComponent::GetTexture, &GASS::PlaneGeometryComponent::SetTexture);
	}

	void PlaneGeometryComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(typeid(LoadComponentsMessage),MESSAGE_FUNC(PlaneGeometryComponent::OnLoad),0);
	}

	Vec2 PlaneGeometryComponent::GetSize() const
	{
		return m_Size;
	}

	void PlaneGeometryComponent::SetSize(const Vec2 &value)
	{
		m_Size = value;
		if(GetSceneObject()) //initlized
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
			//MessagePtr material_message(new MaterialMessage(Vec4(1,1,1,value),Vec3(1,1,1)));
			//GetSceneObject()->PostMessage(material_message);
			GenerateMesh();
			SetTexture(m_Texture);
		}
	}

	void PlaneGeometryComponent::GenerateMesh()
	{
		Vec2 size(m_Size.x*0.5,m_Size.y*0.5);
		ManualMeshDataPtr mesh_data(new ManualMeshData());
		MeshVertex v1,v2,v3,v4;
		
		mesh_data->Material = "PlaneGeometry";//"WhiteTransparentNoLighting";
		
		Vec4 color(1,1,1,m_Transparency);
		mesh_data->Type = TRIANGLE_LIST;
		
		v1.Pos = Vec3( size.x ,0 , size.y);
		v1.TexCoord.Set(1,1);
		v1.Color = color;
		v2.Pos = Vec3(-size.x ,0 , size.y);
		v2.TexCoord.Set(0,1);
		v2.Color = color;
		v3.Pos = Vec3(-size.x ,0 ,-size.y);
		v3.TexCoord.Set(0,0);
		v3.Color = color;
		v4.Pos = Vec3( size.x ,0 ,-size.y);
		v4.TexCoord.Set(1,0);
		v4.Color = color;

		mesh_data->VertexVector.push_back(v1);
		mesh_data->VertexVector.push_back(v2);
		mesh_data->VertexVector.push_back(v3);
		mesh_data->VertexVector.push_back(v4);

		mesh_data->IndexVector.push_back(2);
		mesh_data->IndexVector.push_back(1);
		mesh_data->IndexVector.push_back(0);

		mesh_data->IndexVector.push_back(3);
		mesh_data->IndexVector.push_back(2);
		mesh_data->IndexVector.push_back(0);

		
		
		MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
		GetSceneObject()->PostMessage(mesh_message);

	}
	
	void PlaneGeometryComponent::OnLoad(MessagePtr message)
	{
		GenerateMesh();
		SetTexture(m_Texture);
	}

	void PlaneGeometryComponent::SetTexture(const std::string &texture_name)
	{
		m_Texture = texture_name;
		if(GetSceneObject())
		{
			MessagePtr texture_message(new TextureMessage(texture_name));
			GetSceneObject()->PostMessage(texture_message);
		}
	}

	std::string PlaneGeometryComponent::GetTexture() const
	{
		return m_Texture;
	}
}
