#include "Modules/Editor/EditorSystem.h"
#include "Modules/Editor/EditorMessages.h"
#include "Modules/Editor/ToolSystem/MouseToolController.h"
#include "GASSMultiSelectionComponent.h"
#include "EditorComponent.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/GASSSceneObject.h"
#include "Core/ComponentSystem/GASSComponentFactory.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Interface/GASSIGeometryComponent.h"

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
		SceneManagerListenerPtr listener = shared_from_this();
		GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<EditorSceneManager>()->Register(listener);
	}

	void MultiSelectionComponent::OnDelete()
	{

	}

	void MultiSelectionComponent::SceneManagerTick(double /*delta_time*/)
	{
		UpdateSelection();
	}

	void MultiSelectionComponent::UpdateSelection()
	{
		GraphicsMeshPtr mesh_data(new GraphicsMesh());
		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		mesh_data->SubMeshVector.push_back(sub_mesh_data);
		bool visible = false;
		Vec3 world_pos(0, 0, 0);
		for(size_t i = 0 ;i < m_Selection.size(); i++) 
		{
			SceneObjectPtr so = m_Selection[i].lock();
			if(!so)
				continue;
			
			LocationComponentPtr lc = so->GetFirstComponentByClass<ILocationComponent>();
			Vec3 object_pos = lc->GetWorldPosition();

			if (!visible) //first 
				world_pos = object_pos;
			visible = true;

			GeometryComponentPtr gc = so->GetFirstComponentByClass<IGeometryComponent>();
			Mat4 world_mat = Mat4::CreateTransformationSRT(lc->GetScale(), lc->GetWorldRotation(), object_pos - world_pos);
			AABox bb = gc->GetBoundingBox();

			const Vec3 size = bb.GetSize() * 0.5;
			const Vec3 offset = (bb.Max + bb.Min)*0.5;

			sub_mesh_data->MaterialName = "WhiteNoLighting";
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

			for(int j = 0; j< 4; j++)
			{
				Vec3 temp_world_pos = world_mat*conrners[j];
				sub_mesh_data->PositionVector.push_back(temp_world_pos);
				sub_mesh_data->ColorVector.push_back(m_Color);
				temp_world_pos = world_mat*conrners[(j +1)%4];
				sub_mesh_data->PositionVector.push_back(temp_world_pos);
				sub_mesh_data->ColorVector.push_back(m_Color);

				temp_world_pos = world_mat*conrners[j];
				sub_mesh_data->PositionVector.push_back(temp_world_pos);
				sub_mesh_data->ColorVector.push_back(m_Color);
				temp_world_pos = world_mat*conrners[j +4];
				sub_mesh_data->PositionVector.push_back(temp_world_pos);
				sub_mesh_data->ColorVector.push_back(m_Color);

				temp_world_pos = world_mat*conrners[j + 4];
				sub_mesh_data->PositionVector.push_back(temp_world_pos);
				sub_mesh_data->ColorVector.push_back(m_Color);
				temp_world_pos = world_mat*conrners[((j + 1) % 4) + 4];
				sub_mesh_data->PositionVector.push_back(temp_world_pos);
				sub_mesh_data->ColorVector.push_back(m_Color);

			}
		}
		GetSceneObject()->PostRequest(ManualMeshDataRequestPtr(new ManualMeshDataRequest(mesh_data)));
		GetSceneObject()->PostRequest(LocationVisibilityRequestPtr(new LocationVisibilityRequest(visible)));
		
		if(visible) //move to location
			GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->SetWorldPosition(world_pos);
	}

	void MultiSelectionComponent::OnSelectionChanged(EditorSelectionChangedEventPtr message)
	{
		m_Selection.clear();
		for(size_t i = 0; i < message->m_Selection.size(); i++)
		{
			SceneObjectPtr obj = message->m_Selection[i].lock();
			if(!obj)
				continue;

			LocationComponentPtr lc = obj->GetFirstComponentByClass<ILocationComponent>();
			EditorComponentPtr ec = obj->GetFirstComponentByClass<EditorComponent>();
			GeometryComponentPtr gc = obj->GetFirstComponentByClass<IGeometryComponent>();
			bool show_bb = true;

			if(ec)
			{
				show_bb = ec->GetShowBBWhenSelected();
			}

			if(lc && gc && show_bb)
			{
				m_Selection.push_back(message->m_Selection[i]);
			}
		}
	}
}
