#include "Modules/Editor/EditorSystem.h"
#include "Modules/Editor/EditorMessages.h"
#include "Modules/Editor/ToolSystem/MouseToolController.h"
#include "PaintGizmoComponent.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Math/GASSMath.h"
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
		ComponentFactory::Get().Register<PaintGizmoComponent>();
		RegisterGetSet("Size",&PaintGizmoComponent::GetSize, &PaintGizmoComponent::SetSize);
		RegisterGetSet("Color",&PaintGizmoComponent::GetColor, &PaintGizmoComponent::SetColor);
		RegisterGetSet("Type",&PaintGizmoComponent::GetType, &PaintGizmoComponent::SetType);
	}

	void PaintGizmoComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(PaintGizmoComponent::OnTransformation,TransformationChangedEvent,0));
		BuildMesh();
	}

	void PaintGizmoComponent::OnDelete()
	{

	}

	void PaintGizmoComponent::OnTransformation(TransformationChangedEventPtr message)
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
		const Float samples = 60;
		const Float rad = 2*GASS_PI/samples;
		Float x,y;
		for(Float i = 0 ;i <= samples; i++)
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
		
		
		GetSceneObject()->PostRequest(ManualMeshDataRequestPtr(new ManualMeshDataRequest(m_MeshData)));
	}
}
