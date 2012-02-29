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
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Core/MessageSystem/IMessage.h"
#include "Core/Utils/Log.h"
#include "Sim/Scenario/Scenario.h"
#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/SimEngine.h"
#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"
#include "Sim/Components/Graphics/MeshData.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"
#include "Sim/Scenario/Scene/Messages/GraphicsSceneObjectMessages.h"


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

	void BoxGeometryComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(typeid(LoadCoreComponentsMessage),MESSAGE_FUNC(BoxGeometryComponent::OnLoad),0);
	}

	Vec3 BoxGeometryComponent::GetSize() const
	{
		return m_Size;
	}

	void BoxGeometryComponent::SetSize(const Vec3 &value)
	{
		m_Size = value;
	}
	
	void BoxGeometryComponent::OnLoad(MessagePtr message)
	{

		Vec3 size= m_Size*0.5;
		ManualMeshDataPtr mesh_data(new ManualMeshData());
		MeshVertex vertex;
		mesh_data->Material = "WhiteTransparentNoLighting";

		vertex.TexCoord.Set(0,0);
		vertex.Color = Vec4(1,1,1,1);
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
}
