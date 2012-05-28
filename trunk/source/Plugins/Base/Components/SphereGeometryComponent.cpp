/****************************************************************************
*                                                                           *
* HiFiEngine                                                                *
* Copyright (C)2003 - 2005 Johan Hedstrom                                   *
* Email: hifiengine@gmail.com                                               *
* Web page: http://n00b.dyndns.org/HiFiEngine                               *
*                                                                           *
* HiFiEngine is only used with knowledge from the author. This software     *
* is not allowed to redistribute without permission from the author.        *
* For further license information, please turn to the product home page or  *
* contact author. Abuse against the HiFiEngine license is prohibited by law.*
*                                                                           *
*****************************************************************************/

#include "SphereGeometryComponent.h"
#include "Plugins/Base/CoreMessages.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/MessageSystem/GASSIMessage.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/Scene/GASSScene.h"
#include "Sim/Scene/GASSSceneObject.h"
#include "Sim/GASSSimEngine.h"
#include "Sim/Components/Graphics/Geometry/GASSIGeometryComponent.h"
#include "Sim/Components/Graphics/GASSMeshData.h"
#include "Sim/Components/Graphics/GASSILocationComponent.h"

#include "Sim/Scene/GASSGraphicsSceneObjectMessages.h"


namespace GASS
{
	SphereGeometryComponent::SphereGeometryComponent(void) : m_Radius(1)
	{

	}

	SphereGeometryComponent::~SphereGeometryComponent(void)
	{
		
	}

	void SphereGeometryComponent::RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("SphereGeometryComponent",new GASS::Creator<SphereGeometryComponent, IComponent>);
		RegisterProperty<Float>("Radius", &GASS::SphereGeometryComponent::GetRadius, &GASS::SphereGeometryComponent::SetRadius);
	}

	void SphereGeometryComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(typeid(LoadComponentsMessage),MESSAGE_FUNC(SphereGeometryComponent::OnLoad),0);
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

	void SphereGeometryComponent::UpdateMesh()
	{
		ManualMeshDataPtr mesh_data(new ManualMeshData());
		MeshVertex vertex;
		mesh_data->Material = "WhiteTransparentNoLighting";

		vertex.TexCoord.Set(0,0);
		vertex.Color = Vec4(0,0,1,1);
		mesh_data->Type = LINE_LIST;
		

		//Vec3 up = GetSceneObject()->GetSceneObjectManager()->GetScene()->GetSceneUp();
		float samples = 30;
		float rad = 2*MY_PI/samples;

		float x,y,z;
		for(float i = 0 ;i <= samples; i++)
		{
			x = cos(rad*i)*m_Radius;
			y = sin(rad*i)*m_Radius;
			vertex.Pos.Set(x,y,0);
			mesh_data->VertexVector.push_back(vertex);
		}
		mesh_data->VertexVector.push_back(vertex);

		for(float i = 0 ;i <= samples; i++)
		{
			x = cos(rad*i)*m_Radius;
			z = sin(rad*i)*m_Radius;
			vertex.Pos.Set(x,0,z);
			mesh_data->VertexVector.push_back(vertex);
		}
		mesh_data->VertexVector.push_back(vertex);

		for(float i = 0 ;i <= samples; i++)
		{
			y = cos(rad*i)*m_Radius;
			z = sin(rad*i)*m_Radius;
			vertex.Pos.Set(0,y,z);
			mesh_data->VertexVector.push_back(vertex);
		}
		mesh_data->VertexVector.push_back(vertex);
		MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
		GetSceneObject()->PostMessage(mesh_message);
	}
	
	void SphereGeometryComponent::OnLoad(MessagePtr message)
	{
		UpdateMesh();
	}


	bool SphereGeometryComponent::IsPointInside(const Vec3 &point) const
	{
		return true;
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
		Float l = sqrt(m_Radius * rand2);

		location.x = p1.x + cos(theta) * l;
		location.z = p1.z + sin(theta) * l;
		location.y = p1.y;

		return location;
	}

}
