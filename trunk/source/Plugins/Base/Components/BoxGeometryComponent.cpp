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

#include "BoxGeometryComponent.h"
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
	BoxGeometryComponent::BoxGeometryComponent(void) : m_Size(1,1,1)
	{

	}

	BoxGeometryComponent::~BoxGeometryComponent(void)
	{
		
	}

	void BoxGeometryComponent::RegisterReflection()
	{
		GASS::ComponentFactory::GetPtr()->Register("BoxGeometryComponent",new GASS::Creator<BoxGeometryComponent, IComponent>);
		RegisterProperty<Vec3>("Size", &GASS::BoxGeometryComponent::GetSize, &GASS::BoxGeometryComponent::SetSize);
	}

	void BoxGeometryComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(typeid(LoadComponentsMessage),MESSAGE_FUNC(BoxGeometryComponent::OnLoad),0);
	}

	Vec3 BoxGeometryComponent::GetSize() const
	{
		return m_Size;
	}

	void BoxGeometryComponent::SetSize(const Vec3 &value)
	{
		m_Size = value;
		if(GetSceneObject())
			UpdateMesh();
	}


	void BoxGeometryComponent::UpdateMesh()
	{
		Vec3 size= m_Size*0.5;
		ManualMeshDataPtr mesh_data(new ManualMeshData());
		MeshVertex vertex;
		mesh_data->Material = "WhiteTransparentNoLighting";

		vertex.TexCoord.Set(0,0);
		vertex.Color = Vec4(0,0,1,1);
		mesh_data->Type = LINE_LIST;
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
			vertex.Pos = conrners[i];
			mesh_data->VertexVector.push_back(vertex);
			vertex.Pos = conrners[(i+1)%4];
			mesh_data->VertexVector.push_back(vertex);

			vertex.Pos = conrners[i];
			mesh_data->VertexVector.push_back(vertex);
			vertex.Pos = conrners[i+4];
			mesh_data->VertexVector.push_back(vertex);
		}

		for(int i = 0; i < 4; i++)
		{
			vertex.Pos = conrners[4 + i];
			mesh_data->VertexVector.push_back(vertex);
			vertex.Pos = conrners[4 + ((i+1)%4)];
			mesh_data->VertexVector.push_back(vertex);
		}
		MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
		GetSceneObject()->PostMessage(mesh_message);
	}
	
	
	void BoxGeometryComponent::OnLoad(MessagePtr message)
	{
		UpdateMesh();
	}


	bool BoxGeometryComponent::IsPointInside(const Vec3 &point) const
	{
		return true;
	}

	Vec3 BoxGeometryComponent::GetRandomPoint() const
	{
		Vec3 location;
		LocationComponentPtr loc_comp = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();

		assert(loc_comp);

		Vec3 pos = loc_comp->GetWorldPosition();
		Quaternion rot = loc_comp->GetWorldRotation();

		Float rand1 = rand() / Float(RAND_MAX);
		Float rand2 = rand() / Float(RAND_MAX);
		Float rand3 = rand() / Float(RAND_MAX);

		location.x = (2*rand1-1) * m_Size.x*0.5;
		location.y = (2*rand2-1) * m_Size.y*0.5;
		location.z = (2*rand3-1) * m_Size.z*0.5;
		
		Mat4 trans;
		trans.SetTransformation(pos,rot,Vec3(1,1,1));
		location = trans*location;

		return location;
	}
}
