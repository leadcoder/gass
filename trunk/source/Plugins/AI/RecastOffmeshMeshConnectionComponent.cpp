/****************************************************************************
*                                                                           *
* GASS                                                                *
* Copyright (C)2003 - 2005 Johan Hedstrom                                   *
* Email: GASS@gmail.com                                               *
* Web page: http://n00b.dyndns.org/GASS                               *
*                                                                           *
* GASS is only used with knowledge from the author. This software     *
* is not allowed to redistribute without permission from the author.        *
* For further license information, please turn to the product home page or  *
* contact author. Abuse against the GASS license is prohibited by law.*
*                                                                           *
*****************************************************************************/ 

#include "RecastOffmeshMeshConnectionComponent.h"
#include "Recast/Recast.h"
#include "Recast/RecastAlloc.h"
#include "Recast/RecastAssert.h"
#include "Detour/DetourAssert.h"
#include "Detour/DetourNavMesh.h"
#include "Detour/DetourNavMeshBuilder.h"
#include "Detour/DetourNavMeshQuery.h"
#include "Detour/DetourCommon.h"
#include "Detour/DetourTileCache.h"
#include "InputGeom.h"
#include "tinyxml.h"

namespace GASS
{
	RecastOffmeshMeshConnectionComponent::RecastOffmeshMeshConnectionComponent() : m_ConnectionLine(new ManualMeshData()),
		m_Initialized(false),
		m_Visible(true),
		m_Radius(1.0),
		m_Mode("BIDIRECTIONAL")
	{

	}

	RecastOffmeshMeshConnectionComponent::~RecastOffmeshMeshConnectionComponent()
	{
		
	}

	void RecastOffmeshMeshConnectionComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("RecastOffmeshMeshConnectionComponent",new Creator<RecastOffmeshMeshConnectionComponent, IComponent>);
		RegisterProperty<float>("Radius", &GetRadius, &SetRadius);
		RegisterProperty<std::string>("Mode", &GetMode, &SetMode);
	}

	void RecastOffmeshMeshConnectionComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(RecastOffmeshMeshConnectionComponent::OnStartNodeTransformation,TransformationNotifyMessage,0));
		SceneObjectPtr end_node = GetSceneObject()->GetChildByID("AI_OFF_MESH_CHILD_OBJECT");
		if(!end_node)
			GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"Failed to get child by ID, no child with AI_OFF_MESH_CHILD_OBJECT found", "RecastOffmeshMeshConnectionComponent::OnInitialize");
		end_node->RegisterForMessage(REG_TMESS(RecastOffmeshMeshConnectionComponent::OnEndNodeTransformation,TransformationNotifyMessage,0));
		m_EndNode = end_node;

		//initlize visibility
		SetVisible(m_Visible);
		m_Initialized = true;

	}

	void RecastOffmeshMeshConnectionComponent::OnStartNodeTransformation(TransformationNotifyMessagePtr message)
	{
		m_StartPos = message->GetPosition();
		UpdateConnectionLine();
	}

	void RecastOffmeshMeshConnectionComponent::OnEndNodeTransformation(TransformationNotifyMessagePtr message)
	{
		m_EndPos = message->GetPosition();
		UpdateConnectionLine();
	}

	void RecastOffmeshMeshConnectionComponent::SetRadius(float value)
	{
		m_Radius = value;
		if(m_Initialized)
			UpdateConnectionLine();
	}

	float RecastOffmeshMeshConnectionComponent::GetRadius() const
	{
		return m_Radius;
	}

	bool RecastOffmeshMeshConnectionComponent::GetVisible()  const
	{
		return m_Visible;
	}

	void RecastOffmeshMeshConnectionComponent::SetVisible(bool value) 
	{
		m_Visible = value;
		if(GetSceneObject())
		{
			GetSceneObject()->PostMessage(MessagePtr(new VisibilityMessage(value)));
			if(m_Visible)
				UpdateConnectionLine();
		}
	}

	void RecastOffmeshMeshConnectionComponent::UpdateConnectionLine()
	{
		m_ConnectionLine->VertexVector.clear();
		m_ConnectionLine->IndexVector.clear();
		
		m_ConnectionLine->Material = "WhiteTransparentNoLighting";

		MeshVertex vertex;
		vertex.TexCoord.Set(0,0);
		vertex.Color.Set(0.2,0.2,1,1);
		vertex.Normal = Vec3(0,1,0);

		m_ConnectionLine->Type = LINE_LIST;
		//draw circles
		const float samples = 24;
		const float rad = 2*MY_PI/samples;
		float x,y;
		vertex.Pos = Vec3(0,0,0);
		m_ConnectionLine->VertexVector.push_back(vertex);
		for(float i = 0 ;i <= samples; i++)
		{
			x = sin(rad*i)*m_Radius;
			y = cos(rad*i)*m_Radius;
			vertex.Pos.Set(x,0,y);
			m_ConnectionLine->VertexVector.push_back(vertex);
		}
		vertex.Pos = Vec3(0,0,0);
		m_ConnectionLine->VertexVector.push_back(vertex);

		//draw circles
		Vec3 end_center = m_EndPos - m_StartPos;
		vertex.Pos = end_center;
		m_ConnectionLine->VertexVector.push_back(vertex);
		m_ConnectionLine->VertexVector.push_back(vertex);

		for(float i = 0 ;i <= samples; i++)
		{
			x = sin(rad*i)*m_Radius;
			y = cos(rad*i)*m_Radius;
			vertex.Pos.Set(x,0,y);
			vertex.Pos += end_center;
			m_ConnectionLine->VertexVector.push_back(vertex);

		}

		MessagePtr mesh_message(new ManualMeshDataMessage(m_ConnectionLine));
		GetSceneObject()->PostMessage(mesh_message);
	}
}
