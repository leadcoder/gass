#include "Modules/Editor/EditorSystem.h"
#include "Modules/Editor/EditorMessages.h"
#include "Modules/Editor/ToolSystem/MouseToolController.h"
#include "SelectionComponent.h"
#include "EditorComponent.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Interface/GASSIGeometryComponent.h"


#include "Core/Utils/GASSLogManager.h"

#define MOVMENT_EPSILON 0.0000001
#define SELECTION_COMP_SENDER 998

namespace GASS
{

	SelectionComponent::SelectionComponent() : m_Color(1,1,1,1), m_Active(false)
	{

	}

	SelectionComponent::~SelectionComponent()
	{

	}

	void SelectionComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("SelectionComponent",new Creator<SelectionComponent, Component>);
		RegisterProperty<ColorRGBA>("Color",&SelectionComponent::GetColor, &SelectionComponent::SetColor);
		RegisterProperty<std::string>("Type",&SelectionComponent::GetType, &SelectionComponent::SetType);
	}

	void SelectionComponent::OnInitialize()
	{
		GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS(SelectionComponent::OnSceneObjectSelected,ObjectSelectionChangedEvent,0));
	}

	void SelectionComponent::OnDelete()
	{
		GetSceneObject()->GetScene()->UnregisterForMessage(UNREG_TMESS(SelectionComponent::OnSceneObjectSelected,ObjectSelectionChangedEvent));
	}

	void SelectionComponent::OnSceneObjectSelected(ObjectSelectionChangedEventPtr message)
	{
		//Unregister form previous selection
		SceneObjectPtr  previous_selected = m_SelectedObject.lock();
		if(previous_selected)
		{
			previous_selected->UnregisterForMessage(UNREG_TMESS(SelectionComponent::OnSelectedTransformation,TransformationChangedEvent));
			previous_selected->UnregisterForMessage(UNREG_TMESS(SelectionComponent::OnGeometryChanged,GeometryChangedEvent));
		}

		SceneObjectPtr  new_selected = message->GetSceneObject();
		if(new_selected)
		{
			//move gismo to position
			LocationComponentPtr lc = new_selected->GetFirstComponentByClass<ILocationComponent>();
			EditorComponentPtr ec = new_selected->GetFirstComponentByClass<EditorComponent>();
			bool show_bb = true;
			if(ec)
			{
				show_bb = ec->GetShowBBWhenSelected();
			}
			if(lc && show_bb)
			{
				//move to selected location
				GetSceneObject()->PostRequest(WorldPositionRequestPtr(new WorldPositionRequest(lc->GetWorldPosition(),SELECTION_COMP_SENDER)));
				GetSceneObject()->PostRequest(WorldRotationRequestPtr(new WorldRotationRequest(lc->GetWorldRotation(),SELECTION_COMP_SENDER)));
				GetSceneObject()->PostRequest(ScaleRequestPtr(new ScaleRequest(lc->GetScale(),SELECTION_COMP_SENDER)));

				new_selected->RegisterForMessage(REG_TMESS(SelectionComponent::OnSelectedTransformation,TransformationChangedEvent,1));
				new_selected->RegisterForMessage(REG_TMESS(SelectionComponent::OnGeometryChanged,GeometryChangedEvent ,0));
				m_SelectedObject = new_selected;

				GeometryComponentPtr gc = new_selected->GetFirstComponentByClass<IGeometryComponent>();
				if(gc)
				{
					m_BBox = gc->GetBoundingBox();
					BuildMesh();
					GetSceneObject()->PostRequest(LocationVisibilityRequestPtr(new LocationVisibilityRequest(true)));
				}
				else
					GetSceneObject()->PostRequest(LocationVisibilityRequestPtr(new LocationVisibilityRequest(false)));
			}
			else
				GetSceneObject()->PostRequest(LocationVisibilityRequestPtr(new LocationVisibilityRequest(false)));

		}
		else
		{
			m_SelectedObject.reset();
			GetSceneObject()->PostRequest(LocationVisibilityRequestPtr(new LocationVisibilityRequest(false)));
		}
	}

	void SelectionComponent::OnSelectedTransformation(TransformationChangedEventPtr message)
	{
		//move gizmo
		LocationComponentPtr lc = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		if(lc &&  ((lc->GetWorldPosition() - message->GetPosition()).Length()) > MOVMENT_EPSILON)
		{
			//move to selecetd location
			GetSceneObject()->SendImmediateRequest(WorldPositionRequestPtr(new WorldPositionRequest(message->GetPosition(),SELECTION_COMP_SENDER)));
		}
		GetSceneObject()->SendImmediateRequest(WorldRotationRequestPtr(new WorldRotationRequest(message->GetRotation(),SELECTION_COMP_SENDER)));
		GetSceneObject()->SendImmediateRequest(ScaleRequestPtr(new ScaleRequest(message->GetScale(),SELECTION_COMP_SENDER)));
	}

	void SelectionComponent::OnGeometryChanged(GeometryChangedEventPtr message)
	{
		m_BBox = message->GetGeometry()->GetBoundingBox();
		BuildMesh();
	}
	
	void SelectionComponent::BuildMesh()
	{
		const Vec3 size= m_BBox.GetSize()*0.5;
		const Vec3 offset = (m_BBox.m_Max + m_BBox.m_Min)*0.5;
		
		
		GraphicsMeshPtr mesh_data(new GraphicsMesh());
		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		mesh_data->SubMeshVector.push_back(sub_mesh_data);
		Vec3 pos(0,0,0);

		sub_mesh_data->MaterialName = "WhiteTransparentNoLighting";

		sub_mesh_data->Type = LINE_LIST;
		std::vector<Vec3> conrners;

		conrners.push_back(Vec3( size.x ,size.y , size.z)+offset);
		conrners.push_back(Vec3(-size.x ,size.y , size.z)+offset);
		conrners.push_back(Vec3(-size.x ,size.y ,-size.z)+offset);
		conrners.push_back(Vec3( size.x ,size.y ,-size.z)+offset);

		conrners.push_back(Vec3( size.x ,-size.y , size.z)+offset);
		conrners.push_back(Vec3(-size.x ,-size.y , size.z)+offset);
		conrners.push_back(Vec3(-size.x ,-size.y ,-size.z)+offset);
		conrners.push_back(Vec3( size.x ,-size.y ,-size.z)+offset);

		for(int i = 0; i < 4; i++)
		{
			pos = conrners[i];
			sub_mesh_data->PositionVector.push_back(pos);
			sub_mesh_data->ColorVector.push_back(m_Color);
			pos = conrners[(i+1)%4];
			sub_mesh_data->PositionVector.push_back(pos);
			sub_mesh_data->ColorVector.push_back(m_Color);

			pos = conrners[i];
			sub_mesh_data->PositionVector.push_back(pos);
			sub_mesh_data->ColorVector.push_back(m_Color);
			pos = conrners[i+4];
			sub_mesh_data->PositionVector.push_back(pos);
			sub_mesh_data->ColorVector.push_back(m_Color);
		}

		for(int i = 0; i < 4; i++)
		{
			pos = conrners[4 + i];
			sub_mesh_data->PositionVector.push_back(pos);
			sub_mesh_data->ColorVector.push_back(m_Color);
			pos = conrners[4 + ((i+1)%4)];
			sub_mesh_data->PositionVector.push_back(pos);
			sub_mesh_data->ColorVector.push_back(m_Color);
		}
		GetSceneObject()->PostRequest(ManualMeshDataRequestPtr(new ManualMeshDataRequest(mesh_data)));
	}
}
