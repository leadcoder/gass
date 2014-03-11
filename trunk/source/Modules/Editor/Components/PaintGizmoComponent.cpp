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

	PaintGizmoComponent::PaintGizmoComponent() : m_MeshData(new GraphicsMesh), m_Color(0,1,0,1),
		m_Size(30),
		m_InnerSize(20),
		m_Type("follow_height"),
		m_Active(false),
		m_Pos(1,0,0)
	{

	}

	PaintGizmoComponent::~PaintGizmoComponent()
	{

	}

	void PaintGizmoComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("PaintGizmoComponent",new Creator<PaintGizmoComponent, IComponent>);
		RegisterProperty<float>("Size",&PaintGizmoComponent::GetSize, &PaintGizmoComponent::SetSize);
		RegisterProperty<ColorRGBA>("Color",&PaintGizmoComponent::GetColor, &PaintGizmoComponent::SetColor);
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
		//rebulid if moved
		if(m_Pos != message->GetPosition())
		{
			m_Pos = message->GetPosition();
			BuildMesh();
		}
	}

	void PaintGizmoComponent::BuildMesh()
	{
		if(!m_HMTerrain)
			m_HMTerrain = GetSceneObject()->GetScene()->GetRootSceneObject()->GetFirstComponentByClass<IHeightmapTerrainComponent>(true);

		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		m_MeshData->SubMeshVector.clear();
		m_MeshData->SubMeshVector.push_back(sub_mesh_data);

		//vertex.Color = m_Color;
		sub_mesh_data->Type = LINE_STRIP;
		sub_mesh_data->MaterialName = "PaintGizmoMat";

		Vec3 pos(0,0,0);
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

			pos.Set(x,h,y);
			sub_mesh_data->PositionVector.push_back(pos);
			sub_mesh_data->ColorVector.push_back(m_Color);
		}
		sub_mesh_data->PositionVector.push_back(pos);
		sub_mesh_data->ColorVector.push_back(m_Color);

		for(float i = 0 ;i <= samples; i++)
		{
			x = cos(rad*i)*m_InnerSize;
			y = sin(rad*i)*m_InnerSize;
			Float h = 0;
			if(m_HMTerrain)
			{
				h = m_HMTerrain->GetHeightAtWorldLocation(x+m_Pos.x,y+m_Pos.z)-m_Pos.y + 0.1;
			}
			pos.Set(x,h,y);
			sub_mesh_data->PositionVector.push_back(pos);
			sub_mesh_data->ColorVector.push_back(m_Color);
		}
		sub_mesh_data->PositionVector.push_back(pos);
		sub_mesh_data->ColorVector.push_back(m_Color);
		
		
		GetSceneObject()->PostRequest(ManualMeshDataMessagePtr(new ManualMeshDataMessage(m_MeshData)));
	}
}
