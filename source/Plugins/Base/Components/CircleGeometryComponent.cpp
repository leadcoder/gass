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

#include "CircleGeometryComponent.h"

#include <memory>
#include "Sim/GASSComponentFactory.h"
#include "Core/Utils/GASSColorRGBA.h"
#include "Core/Math/GASSMath.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/GASSGraphicsMesh.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Interface/GASSIManualMeshComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"


namespace GASS
{
	CircleGeometryComponent::CircleGeometryComponent(void) : 
		m_Color(1,1,1)
		
	{

	}

	CircleGeometryComponent::~CircleGeometryComponent(void)
	{
		
	}

	void CircleGeometryComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<CircleGeometryComponent>();
		GetClassRTTI()->SetMetaData(std::make_shared<ClassMetaData>("CircleGeometryComponent", OF_VISIBLE));
		RegisterGetSet("Radius", &GASS::CircleGeometryComponent::GetRadius, &GASS::CircleGeometryComponent::SetRadius,PF_VISIBLE | PF_EDITABLE,"Circle Radius");
		RegisterMember("Color", &GASS::CircleGeometryComponent::m_Color);
		RegisterMember("Dashed", &GASS::CircleGeometryComponent::m_Dashed);
		
	}

	void CircleGeometryComponent::OnInitialize()
	{
		UpdateMesh();
	}

	void CircleGeometryComponent::OnDelete()
	{
		UpdateMesh();
	}

	Float CircleGeometryComponent::GetRadius() const
	{
		return m_Radius;
	}

	void CircleGeometryComponent::SetRadius(Float value)
	{
		m_Radius = value;
		if(GetSceneObject())
			UpdateMesh();
	}

	void CircleGeometryComponent::UpdateMesh()
	{
		GraphicsMeshPtr mesh_data(new GraphicsMesh());
		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		mesh_data->SubMeshVector.push_back(sub_mesh_data);
		sub_mesh_data->MaterialName = "WhiteNoLighting";
		
		if(m_Dashed)
			sub_mesh_data->Type = LINE_LIST;
		else
			sub_mesh_data->Type = LINE_STRIP;

		Float samples = 30;
		Float rad = 2*GASS_PI/samples;
		Float x,z;
		for(float i = 0 ;i <= samples; i++)
		{
			x = cos(rad*i)*m_Radius;
			z = sin(rad*i)*m_Radius;
			Vec3 pos(x,0,z);
			sub_mesh_data->PositionVector.push_back(pos);
			sub_mesh_data->ColorVector.emplace_back(m_Color.x,m_Color.y,m_Color.z,1);
		
		}
		GetSceneObject()->GetFirstComponentByClass<IManualMeshComponent>()->SetMeshData(*mesh_data);
	}

	bool CircleGeometryComponent::IsPointInside(const Vec3 &point) const
	{
		LocationComponentPtr loc_comp = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		assert(loc_comp);
		Vec3 pos = loc_comp->GetWorldPosition();

		Vec3 dist = pos - point;
		dist.y = 0;
		if(dist.Length() < m_Radius)
			return true;
		return false;
	}

	Vec3 CircleGeometryComponent::GetRandomPoint() const
	{
		Vec3 location;
		LocationComponentPtr loc_comp = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();

		assert(loc_comp);

		Vec3 p1 = loc_comp->GetWorldPosition();

		Float rand1 = rand() / Float(RAND_MAX);
		Float rand2 = rand() / Float(RAND_MAX);

		Float theta = rand1 * GASS_PI * 2;
		Float l = sqrt(rand2) * m_Radius;

		location.x = p1.x + cos(theta) * l;
		location.z = p1.z + sin(theta) * l;
		location.y = p1.y;

		return location;
	}

}
