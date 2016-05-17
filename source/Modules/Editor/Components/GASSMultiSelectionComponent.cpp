#include "Modules/Editor/EditorSystem.h"
#include "Modules/Editor/EditorMessages.h"
#include "Modules/Editor/ToolSystem/MouseToolController.h"
#include "GASSMultiSelectionComponent.h"
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

	MultiSelectionComponent::MultiSelectionComponent() : m_Color(1,1,1,1), m_Active(false)
	{

	}

	MultiSelectionComponent::~MultiSelectionComponent()
	{

	}

	void MultiSelectionComponent::RegisterReflection()
	{
		ComponentFactory::GetPtr()->Register("MultiSelectionComponent",new Creator<MultiSelectionComponent, Component>);
		RegisterProperty<ColorRGBA>("Color",&MultiSelectionComponent::GetColor, &MultiSelectionComponent::SetColor);
		RegisterProperty<std::string>("Type",&MultiSelectionComponent::GetType, &MultiSelectionComponent::SetType);
	}

	void MultiSelectionComponent::OnInitialize()
	{
		GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS(MultiSelectionComponent::OnSelectionChanged, EditorSelectionChangedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(MultiSelectionComponent::OnTransformation,TransformationChangedEvent,0));
		
		SceneManagerListenerPtr listener = shared_from_this();
		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<EditorSceneManager>()->Register(listener);
	}

	void MultiSelectionComponent::OnDelete()
	{
	}

	void MultiSelectionComponent::SceneManagerTick(double delta_time)
	{
		UpdateSelection();
	}

	void MultiSelectionComponent::UpdateSelection()
	{
		GraphicsMeshPtr mesh_data(new GraphicsMesh());
		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		mesh_data->SubMeshVector.push_back(sub_mesh_data);
		bool visible = false;
		for(size_t i = 0 ;i < m_Selection.size(); i++) 
		{
			m_BBox = AABox();
			SceneObjectPtr so = m_Selection[i].lock();
			if(!so)
				continue;
			LocationComponentPtr lc = so->GetFirstComponentByClass<ILocationComponent>();
			EditorComponentPtr ec = so->GetFirstComponentByClass<EditorComponent>();
			GeometryComponentPtr gc = so->GetFirstComponentByClass<IGeometryComponent>();
			bool show_bb = true;
			if(ec)
			{
				show_bb = ec->GetShowBBWhenSelected();
			}

			if(lc && gc && show_bb)
			{
				visible = true;

				Vec3 world_pos = lc->GetWorldPosition();
				Quaternion world_rot = lc->GetWorldRotation();
				Vec3 scale = lc->GetScale();
				Mat4 world_mat;
				world_mat.SetTransformation(world_pos,world_rot,scale);
				
				m_BBox.Union(gc->GetBoundingBox());
			
				const Vec3 size = m_BBox.GetSize() * 0.5;
				const Vec3 offset = (m_BBox.m_Max + m_BBox.m_Min)*0.5;
				
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
					pos = world_mat*conrners[i];
					sub_mesh_data->PositionVector.push_back(pos);
					sub_mesh_data->ColorVector.push_back(m_Color);
					pos = world_mat*conrners[(i+1)%4];
					sub_mesh_data->PositionVector.push_back(pos);
					sub_mesh_data->ColorVector.push_back(m_Color);

					pos = world_mat*conrners[i];
					sub_mesh_data->PositionVector.push_back(pos);
					sub_mesh_data->ColorVector.push_back(m_Color);
					pos = world_mat*conrners[i+4];
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
			}
		}
		GetSceneObject()->PostRequest(ManualMeshDataRequestPtr(new ManualMeshDataRequest(mesh_data)));
		GetSceneObject()->PostRequest(LocationVisibilityRequestPtr(new LocationVisibilityRequest(visible)));
	}

	void MultiSelectionComponent::OnSelectionChanged(EditorSelectionChangedEventPtr message)
	{
		//Unregister form previous selection
		/*SceneObjectPtr previous_selected = m_SelectedObject.lock();
		if(previous_selected)
		{
			previous_selected->UnregisterForMessage(UNREG_TMESS(MultiSelectionComponent::OnSelectedTransformation,TransformationChangedEvent));
			previous_selected->UnregisterForMessage(UNREG_TMESS(MultiSelectionComponent::OnGeometryChanged,GeometryChangedEvent));
		}*/

		m_Selection = message->m_Selection;
		//UpdateSelection();
		for(size_t i = 0; i < message->m_Selection.size(); i++)
		{
			SceneObjectPtr obj = message->m_Selection[i].lock();
			if(!obj)
				continue;
			//move gizmo to position
			LocationComponentPtr lc = obj->GetFirstComponentByClass<ILocationComponent>();
			EditorComponentPtr ec = obj->GetFirstComponentByClass<EditorComponent>();
			bool show_bb = true;
			if(ec)
			{
				show_bb = ec->GetShowBBWhenSelected();
			}
			if(lc && show_bb)
			{
				//move to selected location
				//GetSceneObject()->PostRequest(WorldPositionRequestPtr(new WorldPositionRequest(lc->GetWorldPosition(),SELECTION_COMP_SENDER)));
				//GetSceneObject()->PostRequest(WorldRotationRequestPtr(new WorldRotationRequest(lc->GetWorldRotation(),SELECTION_COMP_SENDER)));
				//GetSceneObject()->PostRequest(ScaleRequestPtr(new ScaleRequest(lc->GetScale(),SELECTION_COMP_SENDER)));
				//new_selected->RegisterForMessage(REG_TMESS(MultiSelectionComponent::OnSelectedTransformation,TransformationChangedEvent,1));
				//new_selected->RegisterForMessage(REG_TMESS(MultiSelectionComponent::OnGeometryChanged,GeometryChangedEvent ,0));
				//m_Selection.push_back(obj);
				//UpdateSelection();

				/*GeometryComponentPtr gc = new_selected->GetFirstComponentByClass<IGeometryComponent>();
				if(gc)
				{
					m_BBox = gc->GetBoundingBox();
					BuildMesh();
					GetSceneObject()->PostRequest(LocationVisibilityRequestPtr(new LocationVisibilityRequest(true)));
				}
				else
					GetSceneObject()->PostRequest(LocationVisibilityRequestPtr(new LocationVisibilityRequest(false)));*/
			}
			//else
			//	GetSceneObject()->PostRequest(LocationVisibilityRequestPtr(new LocationVisibilityRequest(false)));
		}
		/*else
		{
			m_SelectedObject.reset();
			GetSceneObject()->PostRequest(LocationVisibilityRequestPtr(new LocationVisibilityRequest(false)));
		}*/
	}

	void MultiSelectionComponent::OnTransformation(TransformationChangedEventPtr message)
	{
		LocationComponentPtr lc = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		for(size_t i = 0 ;i < m_Selection.size(); i++) //reflect transformation to selection
		{
			SceneObjectPtr so = m_Selection[i].lock();
		}
	}

	/*void MultiSelectionComponent::OnGeometryChanged(GeometryChangedEventPtr message)
	{
		m_BBox = message->GetGeometry()->GetBoundingBox();
		BuildMesh();
	}*/
	
	void MultiSelectionComponent::BuildMesh()
	{
		const Vec3 size= m_BBox.GetSize() * 0.5;
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
