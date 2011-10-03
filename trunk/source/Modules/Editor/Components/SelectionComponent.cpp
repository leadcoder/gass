#include "../EditorMessages.h"
#include "../EditorManager.h"
#include "../ToolSystem/MouseToolController.h"
#include "SelectionComponent.h"
#include "EditorComponent.h"

#include "Sim/Scenario/Scene/Messages/CoreSceneObjectMessages.h"
#include "Sim/Scenario/Scene/SceneObjectManager.h"

#include "Sim/Scenario/Scene/SceneObject.h"
#include "Sim/Systems/SimSystemManager.h"

#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/ComponentSystem/ComponentFactory.h"
#include "Core/MessageSystem/MessageManager.h"
#include "Sim/Components/Graphics/ILocationComponent.h"
#include "Sim/Components/Graphics/Geometry/IGeometryComponent.h"


#include "Core/Utils/Log.h"



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
		//RegisterProperty<float>("Size",&SelectionComponent::GetSize, &SelectionComponent::SetSize);
		RegisterProperty<Vec4>("Color",&SelectionComponent::GetColor, &SelectionComponent::SetColor);
		RegisterProperty<std::string>("Type",&SelectionComponent::GetType, &SelectionComponent::SetType);
	}

	void SelectionComponent::OnCreate()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(SelectionComponent::OnLoad,LoadCoreComponentsMessage,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(SelectionComponent::OnUnload,UnloadComponentsMessage,0));
		EditorManager::GetPtr()->GetMessageManager()->RegisterForMessage(REG_TMESS(SelectionComponent::OnNewCursorInfo, CursorMoved3DMessage, 1000));
		EditorManager::GetPtr()->GetMessageManager()->RegisterForMessage(REG_TMESS(SelectionComponent::OnSceneObjectSelected,ObjectSelectedMessage,0));
		
	}

	void SelectionComponent::OnUnload(UnloadComponentsMessagePtr message)
	{
		EditorManager::GetPtr()->GetMessageManager()->UnregisterForMessage(UNREG_TMESS(SelectionComponent::OnNewCursorInfo, CursorMoved3DMessage));
		EditorManager::GetPtr()->GetMessageManager()->UnregisterForMessage(UNREG_TMESS(SelectionComponent::OnSceneObjectSelected,ObjectSelectedMessage));
		
		SceneObjectPtr  selected(m_SelectedObject,boost::detail::sp_nothrow_tag());
		if(selected)
		{
			selected->UnregisterForMessage(UNREG_TMESS(SelectionComponent::OnSelectedTransformation,TransformationNotifyMessage));
		}
	}


	void SelectionComponent::OnSceneObjectSelected(ObjectSelectedMessagePtr message)
	{
		//Unregister form previous selection
		SceneObjectPtr  previous_selected(m_SelectedObject,boost::detail::sp_nothrow_tag());
		if(previous_selected)
		{
			previous_selected->UnregisterForMessage(UNREG_TMESS(SelectionComponent::OnSelectedTransformation,TransformationNotifyMessage));
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

	void SelectionComponent::OnLoad(LoadCoreComponentsMessagePtr message)
	{
		
	}

	void SelectionComponent::BuildMesh()
	{
		const Vec3 size= m_BBox.GetSize()*0.5;
		const Vec3 offset = (m_BBox.m_Max + m_BBox.m_Min)*0.5;
		ManualMeshDataPtr mesh_data(new ManualMeshData());
		MeshVertex vertex;
		mesh_data->Material = "WhiteTransparentNoLighting";

		vertex.TexCoord.Set(0,0);
		vertex.Color = m_Color;
		mesh_data->Type = LINE_LIST;
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

	void SelectionComponent::OnNewCursorInfo(CursorMoved3DMessagePtr message)
	{
	
	}
}
