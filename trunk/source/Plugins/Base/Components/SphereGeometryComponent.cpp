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

#include "SphereGeometryComponent.h"
#include "Plugins/Base/CoreMessages.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/GASSGraphicsMesh.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"

namespace GASS
{
	SphereGeometryComponent::SphereGeometryComponent(void) : m_Radius(1), 
		m_Wireframe(true),
		m_Color(0,0,1,1)
	{

	}

	SphereGeometryComponent::~SphereGeometryComponent(void)
	{
		
	}

	void SphereGeometryComponent::RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("SphereGeometryComponent",new GASS::Creator<SphereGeometryComponent, IComponent>);
		GetClassRTTI()->SetMetaData(ClassMetaDataPtr(new ClassMetaData("SphereGeometryComponent", OF_VISIBLE)));
		RegisterProperty<Float>("Radius", &GASS::SphereGeometryComponent::GetRadius, &GASS::SphereGeometryComponent::SetRadius);
		RegisterProperty<bool>("Wireframe", &GASS::SphereGeometryComponent::GetWireframe, &GASS::SphereGeometryComponent::SetWireframe);
		RegisterProperty<ColorRGBA>("Color", &GASS::SphereGeometryComponent::GetColor, &GASS::SphereGeometryComponent::SetColor);
	}

	void SphereGeometryComponent::OnInitialize()
	{
		UpdateMesh();
	}

	void SphereGeometryComponent::OnDelete()
	{
		UpdateMesh();
	}

	Float SphereGeometryComponent::GetRadius() const
	{
		return m_Radius;
	}

	void SphereGeometryComponent::SetRadius(Float value)
	{
		m_Radius = value;
		if(GetSceneObject())
			UpdateMesh();
	}


	void SphereGeometryComponent::SetColor(const ColorRGBA &value)
	{
		m_Color = value;
		if(GetSceneObject())
			UpdateMesh();
	}

	void SphereGeometryComponent::UpdateMesh()
	{
		GraphicsSubMeshPtr sub_mesh_data;
		if(m_Wireframe)
			sub_mesh_data = GraphicsSubMesh::GenerateWireframeEllipsoid(Vec3(m_Radius,m_Radius,m_Radius), m_Color, "WhiteTransparentNoLighting", 20);
		else
			sub_mesh_data = GraphicsSubMesh::GenerateWireframeEllipsoid(Vec3(m_Radius,m_Radius,m_Radius), m_Color, "WhiteTransparentNoLighting", 20);
		GraphicsMeshPtr mesh_data(new GraphicsMesh());
		mesh_data->SubMeshVector.push_back(sub_mesh_data);
		GetSceneObject()->PostRequest(ManualMeshDataMessagePtr(new ManualMeshDataMessage(mesh_data)));
	}
	
	bool SphereGeometryComponent::IsPointInside(const Vec3 &point) const
	{

		Vec3 location;
		LocationComponentPtr loc_comp = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		assert(loc_comp);
		Vec3 pos = loc_comp->GetWorldPosition();

		if((pos - point).Length() < m_Radius)
			return true;

		return false;
	}

	Vec3 SphereGeometryComponent::GetRandomPoint() const
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
