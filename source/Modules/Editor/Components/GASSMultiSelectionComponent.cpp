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
		for(size_t i = 0 ;i < m_Selection.size(); i++) 
		{
			SceneObjectPtr so = m_Selection[i].lock();
			if(!so)
				continue;

			visible = true;
			LocationComponentPtr lc = so->GetFirstComponentByClass<ILocationComponent>();
			GeometryComponentPtr gc = so->GetFirstComponentByClass<IGeometryComponent>();
			Mat4 world_mat(lc->GetWorldPosition(), lc->GetWorldRotation(), lc->GetScale());
		
			AABox bb = gc->GetBoundingBox();

			const Vec3 size = bb.GetSize() * 0.5;
			const Vec3 offset = (bb.m_Max + bb.m_Min)*0.5;

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
				Vec3 world_pos = world_mat*conrners[i];
				sub_mesh_data->PositionVector.push_back(world_pos);
				sub_mesh_data->ColorVector.push_back(m_Color);
				world_pos = world_mat*conrners[(i+1)%4];
				sub_mesh_data->PositionVector.push_back(world_pos);
				sub_mesh_data->ColorVector.push_back(m_Color);

				world_pos = world_mat*conrners[i];
				sub_mesh_data->PositionVector.push_back(world_pos);
				sub_mesh_data->ColorVector.push_back(m_Color);
				world_pos = world_mat*conrners[i+4];
				sub_mesh_data->PositionVector.push_back(world_pos);
				sub_mesh_data->ColorVector.push_back(m_Color);
			}
		}
		GetSceneObject()->PostRequest(ManualMeshDataRequestPtr(new ManualMeshDataRequest(mesh_data)));
		GetSceneObject()->PostRequest(LocationVisibilityRequestPtr(new LocationVisibilityRequest(visible)));
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
