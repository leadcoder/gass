#include "BoxTool.h"
#include <iomanip>
#include <memory>
#include "MouseToolController.h"
#include "Modules/Editor/EditorSystem.h"
#include "Modules/Editor/EditorSceneManager.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Utils/GASSException.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSComponentFactory.h"
#include "Sim/GASSGraphicsMesh.h"
#include "Sim/Messages/GASSGraphicsSceneObjectMessages.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Interface/GASSIManualMeshComponent.h"
#include "Sim/Interface/GASSICollisionSceneManager.h"


namespace GASS
{
	BoxTool::BoxTool(MouseToolController* controller) : m_MouseIsDown(false),
		m_Controller(controller)
	{

	}

	BoxTool::~BoxTool()
	{

	}

	void BoxTool::MouseMoved(const MouseData&/*data*/, const SceneCursorInfo& info)
	{
		if (m_MouseIsDown)
		{
			const Vec3 offset = Vec3(0, 0.1f, 0);
			UpdateBox(m_StartPos + offset, info.m_3DPos + offset);
		}
	}


	void BoxTool::MouseDown(const MouseData&/*data*/, const SceneCursorInfo& info)
	{
		m_MouseIsDown = true;
		m_StartPos = info.m_3DPos;
	}

	void BoxTool::MouseUp(const MouseData&/*data*/, const SceneCursorInfo&/*info*/)
	{
		m_MouseIsDown = false;
		SceneObjectPtr box = GetOrCreateBoxObject();
		box->GetFirstComponentByClass<IManualMeshComponent>()->Clear();
	}


	void BoxTool::UpdateBox(const Vec3& start, const Vec3& end)
	{
		const std::string mat_name = "BoxMaterial";
		GraphicsSystemPtr gfx_sys = SimEngine::Get().GetSimSystemManager()->GetFirstSystemByClass<IGraphicsSystem>();
		if (!gfx_sys->HasMaterial(mat_name))
		{
			GraphicsMaterial mat;
			mat.Name = mat_name;
			mat.Diffuse.Set(0, 0, 0, 1);
			mat.Ambient.Set(0, 0, 0);
			mat.SelfIllumination.Set(1.0, 0, 0);
			mat.DepthTest = false;
			mat.DepthWrite = true;
			mat.TrackVertexColor = false;
			gfx_sys->AddMaterial(mat, mat_name);
		}

		SceneObjectPtr box = GetOrCreateBoxObject();
		GraphicsMeshPtr mesh_data(new GraphicsMesh());
		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		mesh_data->SubMeshVector.push_back(sub_mesh_data);

		sub_mesh_data->MaterialName = mat_name;
		sub_mesh_data->Type = LINE_STRIP;
		ColorRGBA color(1, 0, 0, 1);

		
		Quaterniond world_rot = Quaterniond::IDENTITY;
		const CollisionSceneManagerPtr csm = m_Controller->GetEditorSceneManager()->GetScene()->GetFirstSceneManagerByClass<ICollisionSceneManager>(true);
		if (csm)
		{
			csm->GetOrientation(start, world_rot);
		}

		box->GetFirstComponentByClass<ILocationComponent>()->SetPosition(start);
		box->GetFirstComponentByClass<ILocationComponent>()->SetRotation(world_rot);
		const Mat4d transform(world_rot,start);
		const auto inv_trans = transform.GetInvert();
		const Vec3 corner = inv_trans * end;// -start;

		sub_mesh_data->PositionVector.emplace_back(0, 0, 0);
		sub_mesh_data->ColorVector.push_back(color);

		sub_mesh_data->PositionVector.emplace_back(corner.x, 0, 0);
		sub_mesh_data->ColorVector.push_back(color);

		sub_mesh_data->PositionVector.push_back(corner);
		sub_mesh_data->ColorVector.push_back(color);

		sub_mesh_data->PositionVector.emplace_back(0, 0, corner.z);
		sub_mesh_data->ColorVector.push_back(color);

		sub_mesh_data->PositionVector.emplace_back(0, 0, 0);
		sub_mesh_data->ColorVector.push_back(color);

		sub_mesh_data->IndexVector.push_back(0);
		sub_mesh_data->IndexVector.push_back(1);
		sub_mesh_data->IndexVector.push_back(2);
		sub_mesh_data->IndexVector.push_back(3);
		sub_mesh_data->IndexVector.push_back(4);

		box->GetFirstComponentByClass<IManualMeshComponent>()->SetMeshData(*mesh_data);
	}

	SceneObjectPtr BoxTool::GetOrCreateBoxObject()
	{
		SceneObjectPtr box_obj = m_BoxObject.lock();
		if (!box_obj)
		{
			box_obj = std::make_shared<SceneObject>();
			box_obj->SetName("BoxObject");
			box_obj->SetID("BOX_OBJECT");
			box_obj->SetSerialize(false);
			ComponentPtr location_comp = ComponentFactory::Get().Create("LocationComponent");
			box_obj->AddComponent(location_comp);

			ComponentPtr mesh_comp = ComponentFactory::Get().Create("ManualMeshComponent");
			mesh_comp->SetPropertyValue("GeometryFlags", GeometryFlagsBinder(GEOMETRY_FLAG_EDITOR));
			box_obj->AddComponent(mesh_comp);

			if (!box_obj)
			{
				GASS_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Failed to create in RulerObject", "BoxTool::GetOrCreateRulerObject");
			}
			m_BoxObject = box_obj;

			m_Controller->GetEditorSceneManager()->GetScene()->GetRootSceneObject()->AddChildSceneObject(box_obj, true);

		}
		return box_obj;
	}
}