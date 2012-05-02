#include "../EditorMessages.h"
#include "../EditorManager.h"
#include "../ToolSystem/MouseToolController.h"
#include "PaintGizmoComponent.h"
#include "Sim/Scene/GASSCoreSceneObjectMessages.h"


#include "Sim/Scene/GASSSceneObject.h"
#include "Sim/Systems/GASSSimSystemManager.h"

#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Sim/Components/Graphics/GASSILocationComponent.h"
#include "Core/Utils/GASSLogManager.h"



//#define GIZMO_SENDER 999

namespace GASS
{

	PaintGizmoComponent::PaintGizmoComponent() : m_MeshData(new ManualMeshData), m_Color(1,0,0,1),
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
		GetSceneObject()->RegisterForMessage(REG_TMESS(PaintGizmoComponent::OnLoad,LoadComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PaintGizmoComponent::OnUnload,UnloadComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(PaintGizmoComponent::OnTransformation,TransformationNotifyMessage,0));
		EditorManager::GetPtr()->GetMessageManager()->RegisterForMessage(REG_TMESS(PaintGizmoComponent::OnNewCursorInfo, CursorMoved3DMessage, 1000));
	}

	void PaintGizmoComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
		EditorManager::GetPtr()->GetMessageManager()->UnregisterForMessage(UNREG_TMESS(PaintGizmoComponent::OnNewCursorInfo, CursorMoved3DMessage));
	}

	void PaintGizmoComponent::OnTransformation(TransformationNotifyMessagePtr message)
	{
		
	}

	void PaintGizmoComponent::OnLoad(LoadComponentsMessagePtr message)
	{
		BuildMesh();
	}

	void PaintGizmoComponent::BuildMesh()
	{
		m_MeshData->VertexVector.clear();
		m_MeshData->IndexVector.clear();

		MeshVertex vertex;
		vertex.TexCoord.Set(0,0);
		vertex.Color = m_Color;
		m_MeshData->Type = LINE_STRIP;
		m_MeshData->Material = "WhiteTransparentNoLighting";

		const float samples = 30;
		const float rad = 2*MY_PI/samples;
		float x,y;
		for(float i = 0 ;i <= samples; i++)
		{
			x = cos(rad*i)*m_Size;
			y = sin(rad*i)*m_Size;
			vertex.Pos.Set(x,0,y);
			m_MeshData->VertexVector.push_back(vertex);
		}
		m_MeshData->VertexVector.push_back(vertex);

		for(float i = 0 ;i <= samples; i++)
		{
			x = cos(rad*i)*m_InnerSize;
			y = sin(rad*i)*m_InnerSize;
			vertex.Pos.Set(x,0,y);
			m_MeshData->VertexVector.push_back(vertex);
		}
		m_MeshData->VertexVector.push_back(vertex);


		MessagePtr mesh_message(new ManualMeshDataMessage(m_MeshData));
		GetSceneObject()->PostMessage(mesh_message);
	}

	void PaintGizmoComponent::OnNewCursorInfo(CursorMoved3DMessagePtr message)
	{
		//SceneObjectPtr obj_under_cursor = message->GetSceneObjectUnderCursor();
		//move this object
	}
}
