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

#include "CircleGeometryComponent.h"
#include "Plugins/Base/CoreMessages.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "Core/Utils/GASSColorRGBA.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/GASSGraphicsMesh.h"
#include "Sim/Interface/GASSILocationComponent.h"

#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"


namespace GASS
{
	CircleGeometryComponent::CircleGeometryComponent(void) : m_Radius(1),
		m_Color(1,1,1),
		m_Dashed(false)
	{

	}

	CircleGeometryComponent::~CircleGeometryComponent(void)
	{
		
	}

	void CircleGeometryComponent::RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("CircleGeometryComponent",new GASS::Creator<CircleGeometryComponent, Component>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("CircleGeometryComponent", OF_VISIBLE)));
		RegisterProperty<Float>("Radius", &GASS::CircleGeometryComponent::GetRadius, &GASS::CircleGeometryComponent::SetRadius,
			BasePropertyMetaDataPtr(new BasePropertyMetaData("Circle Radius",PF_VISIBLE | PF_EDITABLE)));
		RegisterProperty<Vec3>("Color", &GASS::CircleGeometryComponent::GetColor, &GASS::CircleGeometryComponent::SetColor);
		RegisterProperty<bool>("Dashed", &GASS::CircleGeometryComponent::GetDashed, &GASS::CircleGeometryComponent::SetDashed);
		
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
		sub_mesh_data->MaterialName = "WhiteTransparentNoLighting";
		
		if(m_Dashed)
			sub_mesh_data->Type = LINE_LIST;
		else
			sub_mesh_data->Type = LINE_STRIP;

		float samples = 30;
		float rad = 2*MY_PI/samples;
		float x,z;
		for(float i = 0 ;i <= samples; i++)
		{
			x = cos(rad*i)*m_Radius;
			z = sin(rad*i)*m_Radius;
			Vec3 pos(x,0,z);
			sub_mesh_data->PositionVector.push_back(pos);
			sub_mesh_data->ColorVector.push_back(ColorRGBA(m_Color.x,m_Color.y,m_Color.z,1));
		
		}
		int id = -1;
		GetSceneObject()->PostRequest(ManualMeshDataRequestPtr(new ManualMeshDataRequest(mesh_data,id, 0.1)));
	}

	bool CircleGeometryComponent::IsPointInside(const Vec3 &point) const
	{

		Vec3 location;
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

		Float theta = rand1 * MY_PI * 2;
		Float l = sqrt(rand2) * m_Radius;

		location.x = p1.x + cos(theta) * l;
		location.z = p1.z + sin(theta) * l;
		location.y = p1.y;

		return location;
	}

}
