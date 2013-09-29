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
		ComponentFactory::GetPtr()->Register("SelectionComponent",new Creator<SelectionComponent, IComponent>);
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
		SceneObjectPtr  previous_selected(m_SelectedObject,NO_THROW);
		if(previous_selected)
		{
			previous_selected->UnregisterForMessage(UNREG_TMESS(SelectionComponent::OnSelectedTransformation,TransformationNotifyMessage));
			previous_selected->UnregisterForMessage(UNREG_TMESS(SelectionComponent::OnGeometryChanged,GeometryChangedMessage));
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
				//move to selecetd location
				GetSceneObject()->PostMessage(MessagePtr(new WorldPositionMessage(lc->GetWorldPosition(),SELECTION_COMP_SENDER)));
				GetSceneObject()->PostMessage(MessagePtr(new WorldRotationMessage(lc->GetWorldRotation(),SELECTION_COMP_SENDER)));
				GetSceneObject()->PostMessage(MessagePtr(new ScaleMessage(lc->GetScale(),SELECTION_COMP_SENDER)));

				new_selected->RegisterForMessage(REG_TMESS(SelectionComponent::OnSelectedTransformation,TransformationNotifyMessage,1));
				new_selected->RegisterForMessage(REG_TMESS(SelectionComponent::OnGeometryChanged,GeometryChangedMessage ,0));
				m_SelectedObject = new_selected;

				GeometryComponentPtr gc = new_selected->GetFirstComponentByClass<IGeometryComponent>();
				if(gc)
				{
					m_BBox = gc->GetBoundingBox();
					BuildMesh();
					GetSceneObject()->PostMessage(MessagePtr(new VisibilityMessage(true)));
				}
				else
					GetSceneObject()->PostMessage(MessagePtr(new VisibilityMessage(false)));
			}
			else
				GetSceneObject()->PostMessage(MessagePtr(new VisibilityMessage(false)));

		}
		else
		{
			m_SelectedObject.reset();
			GetSceneObject()->PostMessage(MessagePtr(new VisibilityMessage(false)));
		}
	}

	void SelectionComponent::OnSelectedTransformation(TransformationNotifyMessagePtr message)
	{
		//move gizmo
		LocationComponentPtr lc = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		if(lc &&  ((lc->GetWorldPosition() - message->GetPosition()).Length()) > MOVMENT_EPSILON)
		{
			//move to selecetd location
			GetSceneObject()->SendImmediate(MessagePtr(new WorldPositionMessage(message->GetPosition(),SELECTION_COMP_SENDER)));
		}
		GetSceneObject()->SendImmediate(MessagePtr(new WorldRotationMessage(message->GetRotation(),SELECTION_COMP_SENDER)));
		GetSceneObject()->SendImmediate(MessagePtr(new ScaleMessage(message->GetScale(),SELECTION_COMP_SENDER)));
	}

	void SelectionComponent::OnGeometryChanged(GeometryChangedMessagePtr message)
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
		MessagePtr mesh_message(new ManualMeshDataMessage(mesh_data));
		GetSceneObject()->PostMessage(mesh_message);
	}
}
