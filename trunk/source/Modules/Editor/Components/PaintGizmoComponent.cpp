#include "Modules/Editor/EditorSystem.h"
#include "Modules/Editor/EditorMessages.h"
#include "Modules/Editor/ToolSystem/MouseToolController.h"
#include "PaintGizmoComponent.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Core/Utils/GASSLogManager.h"
#include "Sim/Interface/GASSITerrainComponent.h"



//#define GIZMO_SENDER 999

namespace GASS
{

	PaintGizmoComponent::PaintGizmoComponent() : m_MeshData(new ManualMeshData), m_Color(0,1,0,1),
		m_Size(30),
		m_InnerSize(20),
		m_Type("follow_height"),
		m_Active(false)
	{

	}

	PaintGizmoComponent::~PaintGizmoComponent()
	{

	}

	void PaintGizmoComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PaintGizmoComponent",new Creator<PaintGizmoComponent, IComponent>);
		RegisterProperty<float>("Size",&PaintGizmoComponent::GetSize, &PaintGizmoComponent::SetSize);
		RegisterProperty<Vec4>("Color",&PaintGizmoComponent::GetColor, &PaintGizmoComponent::SetColor);
		RegisterProperty<std::string>("Type",&PaintGizmoComponent::GetType, &PaintGizmoComponent::SetType);
	}

	void PaintGizmoComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(PaintGizmoComponent::OnTransformation,TransformationNotifyMessage,0));
		BuildMesh();
	}

	void PaintGizmoComponent::OnDelete()
	{

	}

	void PaintGizmoComponent::OnTransformation(TransformationNotifyMessagePtr message)
	{
		//rebulid each frame
		m_Pos = message->GetPosition();
		BuildMesh();

	}

	void PaintGizmoComponent::BuildMesh()
	{
		if(!m_HMTerrain)
			m_HMTerrain = GetSceneObject()->GetScene()->GetRootSceneObject()->GetFirstComponentByClass<IHeightmapTerrainComponent>(true);

		m_MeshData->VertexVector.clear();
		m_MeshData->IndexVector.clear();

		MeshVertex vertex;
		vertex.TexCoord.Set(0,0);
		vertex.Color = m_Color;
		m_MeshData->Type = LINE_STRIP;
		m_MeshData->Material = "PaintGizmoMat";

		const float samples = 60;
		const float rad = 2*MY_PI/samples;
		float x,y;
		for(float i = 0 ;i <= samples; i++)
		{
			x = cos(rad*i)*m_Size;
			y = sin(rad*i)*m_Size;
			Float h = 0;
			if(m_HMTerrain)
			{
				h = m_HMTerrain->GetHeightAtWorldLocation(x+m_Pos.x,y+m_Pos.z)-m_Pos.y + 0.1;
			}

			vertex.Pos.Set(x,h,y);
			m_MeshData->VertexVector.push_back(vertex);
		}
		m_MeshData->VertexVector.push_back(vertex);

		for(float i = 0 ;i <= samples; i++)
		{
			x = cos(rad*i)*m_InnerSize;
			y = sin(rad*i)*m_InnerSize;
			Float h = 0;
			if(m_HMTerrain)
			{
				h = m_HMTerrain->GetHeightAtWorldLocation(x+m_Pos.x,y+m_Pos.z)-m_Pos.y + 0.1;
			}
			vertex.Pos.Set(x,h,y);
			m_MeshData->VertexVector.push_back(vertex);
		}
		m_MeshData->VertexVector.push_back(vertex);


		MessagePtr mesh_message(new ManualMeshDataMessage(m_MeshData));
		GetSceneObject()->PostMessage(mesh_message);
	}
}
