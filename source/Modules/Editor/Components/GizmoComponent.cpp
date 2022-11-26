#include "Modules/Editor/EditorSystem.h"
#include "Modules/Editor/EditorSceneManager.h"
#include "Modules/Editor/EditorMessages.h"
#include "Modules/Editor/ToolSystem/MouseToolController.h"
#include "GizmoComponent.h"
#include "Sim/Messages/GASSCoreSceneObjectMessages.h"
#include "Sim/GASSScene.h"
#include "Sim/GASSSceneObject.h"
#include "Sim/GASSComponentFactory.h"
#include "Sim/GASSSimSystemManager.h"
#include "Sim/GASSSimEngine.h"
#include "Core/MessageSystem/GASSMessageManager.h"
#include "Core/Utils/GASSException.h"
#include "Core/Math/GASSRay.h"
#include "Core/Math/GASSPlane.h"
#include "Sim/Interface/GASSILocationComponent.h"
#include "Sim/Interface/GASSIGeometryComponent.h"
#include "Sim/Interface/GASSIManualMeshComponent.h"
#include "Sim/Interface/GASSIGraphicsSystem.h"

#define GIZMO_SENDER 999

namespace GASS
{
	GizmoComponent::GizmoComponent() : m_Color(1,0,0,1),
		m_Type(GT_AXIS)
	{

	}

	GizmoComponent::~GizmoComponent()
	{

	}

	void GizmoComponent::RegisterReflection()
	{
		ComponentFactory::Get().Register<GizmoComponent>();
		RegisterGetSet("Size",&GizmoComponent::GetSize, &GizmoComponent::SetSize);
		RegisterGetSet("Color",&GizmoComponent::GetColor, &GizmoComponent::SetColor);
		RegisterGetSet("Type",&GizmoComponent::GetType, &GizmoComponent::SetType);
	}

	void GizmoComponent::OnInitialize()
	{
		GetSceneObject()->RegisterForMessage(REG_TMESS(GizmoComponent::OnLocationLoaded,LocationLoadedEvent,0));
		GetSceneObject()->RegisterForMessage(REG_TMESS(GizmoComponent::OnTransformation,TransformationChangedEvent,0));

		GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS(GizmoComponent::OnNewCursorInfo, CursorMovedOverSceneEvent, 1000));
		GetSceneObject()->GetScene()->RegisterForMessage(REG_TMESS(GizmoComponent::OnSelectionChanged,EditorSelectionChangedEvent,0));

		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(GizmoComponent::OnEditMode,EditModeChangedEvent,0));
	
		m_EditorSceneManager = GetSceneObject()->GetScene()->GetFirstSceneManagerByClass<EditorSceneManager>();
		m_Mode = m_EditorSceneManager->GetMouseToolController()->GetEditMode();

		//Change geometry flags
		GeometryComponentPtr gc = GetSceneObject()->GetFirstComponentByClass<IGeometryComponent>();
		gc->SetGeometryFlags(GEOMETRY_FLAG_GIZMO);
	
		m_RegularMat = UnlitNoDTMaterialConfig(ColorRGBA(m_Color.r * 0.5, m_Color.g * 0.5, m_Color.b * 0.5, 1));
		
		m_HighlightMat = UnlitNoDTMaterialConfig(m_Color);
	}

	void GizmoComponent::OnLocationLoaded(LocationLoadedEventPtr message)
	{
		BuildMesh();

		SimEngine::Get().GetSimSystemManager()->RegisterForMessage(REG_TMESS(GizmoComponent::OnCameraChanged, CameraChangedEvent, 1));

		m_ActiveCameraObject = m_EditorSceneManager->GetActiveCameraObject();
		LocationComponentPtr lc = message->GetLocation();
		m_BaseRot = lc->GetEulerRotation().GetQuaternion();
		SetSelection(m_EditorSceneManager->GetSelectedObjects());
		SceneManagerListenerPtr listener = shared_from_this();
		RegisterForPostUpdate<EditorSceneManager>();
	}

	void GizmoComponent::SceneManagerTick(double /*delta_time*/)
	{
		UpdateScale();
	}


	void GizmoComponent::OnDelete()
	{
		
	}
	
	void GizmoComponent::OnTransformation(TransformationChangedEventPtr message)
	{
		const Vec3 current_pos = message->GetPosition();
		if (m_TrackTransformation)
		{
			const Vec3 offset = current_pos - m_PreviousPos;
			const Quaternion base_inverse = m_BaseRot.Inverse();
			const Quaternion new_rot = message->GetRotation() * base_inverse;

			for (size_t i = 0; i < m_Selection.size(); i++)
			{
				SceneObjectPtr selected = m_Selection[i].lock();
				if (selected)
				{
					m_TrackSelectedTransform = false;
					LocationComponentPtr selected_lc = selected->GetFirstComponentByClass<ILocationComponent>();
					selected_lc->SetWorldRotation(new_rot);
					selected_lc->SetWorldPosition(offset + selected_lc->GetWorldPosition());
					m_TrackSelectedTransform = true;
				}
			}
		}
		m_PreviousPos = current_pos;
	}

	void GizmoComponent::OnSelectedTransformation(TransformationChangedEventPtr message)
	{
		if (m_TrackSelectedTransform)
		{
			//move gizmo
			const double GIZMO_MOVMENT_EPSILON = 0.0001;
			LocationComponentPtr lc = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
			if (lc && ((lc->GetWorldPosition() - message->GetPosition()).Length()) > GIZMO_MOVMENT_EPSILON)
			{
				Move(message->GetPosition());
			}

			if (m_Mode == GM_LOCAL)
			{
				Rotate(message->GetRotation() * m_BaseRot);
			}
		}
	}

	void GizmoComponent::OnCameraChanged(CameraChangedEventPtr message)
	{
		CameraComponentPtr camera = message->GetViewport()->GetCamera();
		auto cam_obj = camera->GetSceneObject();
		m_ActiveCameraObject = cam_obj;
	}

	void GizmoComponent::OnSelectionChanged(EditorSelectionChangedEventPtr message)
	{
		SetSelection(message->m_Selection);
	}

	void GizmoComponent::OnEditMode(EditModeChangedEventPtr message)
	{
		m_Mode = message->GetEditMode();
		if(m_Mode == GM_LOCAL)
		{
			SceneObjectPtr  selected = GetFirstSelected();
			if(selected)
			{
				LocationComponentPtr selected_lc = selected->GetFirstComponentByClass<ILocationComponent>();
				if (selected_lc)
					Rotate(selected_lc->GetWorldRotation()*m_BaseRot);
			}
		}
		else if(m_Mode == GM_WORLD)
		{
			Rotate(m_BaseRot);
		}
	}

	void GizmoComponent::Move(const Vec3 &pos)
	{
		m_TrackTransformation = false;
		LocationComponentPtr lc = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		lc->SetWorldPosition(pos);
		m_TrackTransformation = true;
	}

	void GizmoComponent::Rotate(const Quaternion &rot)
	{
		m_TrackTransformation = false;
		LocationComponentPtr lc = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		lc->SetWorldRotation(rot);
		m_TrackTransformation = true;
	}


	void GizmoComponent::Scale(const Vec3 &scale)
	{
		m_TrackTransformation = false;
		LocationComponentPtr lc = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>();
		lc->SetScale(scale);
		m_TrackTransformation = true;
	}

	void GizmoComponent::SetSelection(const std::vector<SceneObjectWeakPtr> &selection)
	{

		//Unregister form previous
		if(SceneObjectPtr  previous_selected = GetFirstSelected())
		{
			previous_selected->UnregisterForMessage(UNREG_TMESS(GizmoComponent::OnSelectedTransformation,TransformationChangedEvent));
		}
		m_Selection.clear();

		//Only add objects that has location
		for(size_t i = 0 ; i < selection.size(); i++)
		{
			if(SceneObjectPtr object = selection[i].lock())
			{
				if(LocationComponentPtr lc = object->GetFirstComponentByClass<ILocationComponent>())
				{
					m_Selection.push_back(object);
				}
			}
		}

		if (SceneObjectPtr  first_selected = GetFirstSelected())
		{
			//transform gizmo to first selected object
			first_selected->RegisterForMessage(REG_TMESS(GizmoComponent::OnSelectedTransformation, TransformationChangedEvent, 1));
			LocationComponentPtr lc = first_selected->GetFirstComponentByClass<ILocationComponent>();
			if (m_Mode == GM_LOCAL)
			{
				//rotate  gizmo  to selected rotation
				Rotate(lc->GetWorldRotation()*m_BaseRot);
			}
			else
			{
				Rotate(m_BaseRot);
			}
			//move gizmo to selected location
			Move(lc->GetWorldPosition());
		}
	}

	SceneObjectPtr GizmoComponent::GetFirstSelected()
	{
		SceneObjectPtr  ret;
		if(m_Selection.size() > 0)
		{
			ret = m_Selection[0].lock();
		}
		return ret;
	}

	void GizmoComponent::UpdateScale()
	{
		SceneObjectPtr camera = m_ActiveCameraObject.lock();
		if (camera)
		{
			auto camera_comp = camera->GetFirstComponentByClass<ICameraComponent>();
			if (camera_comp)
			{
				const double CONST_SCALE_FACTOR = 0.06; //change this to control size on screen
				const double MAX_SCALE_DISTANCE = 1000000; //meters
				const double MIN_SCALE_DISTANCE = 0.1; //meters
				const Vec3 gizmo_pos = GetSceneObject()->GetWorldPosition();
				const Vec3 camera_pos = camera->GetWorldPosition();
				const double dist = Math::Clamp((gizmo_pos - camera_pos).Length(), MIN_SCALE_DISTANCE, MAX_SCALE_DISTANCE);
				double scale_factor = CONST_SCALE_FACTOR * dist;
				
				if (!camera_comp->GetOrtho()) //also scale by fov if perspective camera
					scale_factor *= tan(Math::Deg2Rad(camera_comp->GetFov()));
				
				Scale(Vec3(scale_factor, scale_factor, scale_factor));
			}
		}
	}

	void GizmoComponent::BuildMesh()
	{
		GraphicsMesh mesh;
		GraphicsSubMeshPtr sub_mesh_data(new GraphicsSubMesh());
		mesh.SubMeshVector.push_back(sub_mesh_data);
		sub_mesh_data->MaterialConfig.reset(new UnlitMaterialConfig(m_RegularMat));
		//Arrow
		if(m_Type == GT_AXIS)
		{
			sub_mesh_data->Type = TRIANGLE_LIST;

			Float box_volume = m_Size * 0.01;
			Vec3 offset(0,-box_volume,box_volume);
			Vec3 pos = Vec3(0,box_volume,box_volume)  + offset;
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(0,box_volume,-box_volume) + offset;
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(0,-box_volume,-box_volume)+ offset;;
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(0,-box_volume,box_volume) + offset;
			sub_mesh_data->PositionVector.push_back(pos);

			pos = Vec3(m_Size ,box_volume,box_volume) + offset;;
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(m_Size ,box_volume,-box_volume)  + offset;
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(m_Size ,-box_volume,-box_volume) + offset;
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(m_Size ,-box_volume,box_volume)  + offset;
			sub_mesh_data->PositionVector.push_back(pos);



			sub_mesh_data->IndexVector.push_back(0);
			sub_mesh_data->IndexVector.push_back(4);
			sub_mesh_data->IndexVector.push_back(5);
			sub_mesh_data->IndexVector.push_back(0);
			sub_mesh_data->IndexVector.push_back(5);
			sub_mesh_data->IndexVector.push_back(1);


			sub_mesh_data->IndexVector.push_back(1);
			sub_mesh_data->IndexVector.push_back(5);
			sub_mesh_data->IndexVector.push_back(6);
			sub_mesh_data->IndexVector.push_back(1);
			sub_mesh_data->IndexVector.push_back(6);
			sub_mesh_data->IndexVector.push_back(2);


			sub_mesh_data->IndexVector.push_back(2);
			sub_mesh_data->IndexVector.push_back(6);
			sub_mesh_data->IndexVector.push_back(7);
			sub_mesh_data->IndexVector.push_back(2);
			sub_mesh_data->IndexVector.push_back(7);
			sub_mesh_data->IndexVector.push_back(3);

			sub_mesh_data->IndexVector.push_back(3);
			sub_mesh_data->IndexVector.push_back(7);
			sub_mesh_data->IndexVector.push_back(4);
			sub_mesh_data->IndexVector.push_back(3);
			sub_mesh_data->IndexVector.push_back(4);
			sub_mesh_data->IndexVector.push_back(0);

			sub_mesh_data->IndexVector.push_back(0);
			sub_mesh_data->IndexVector.push_back(1);
			sub_mesh_data->IndexVector.push_back(2);
			sub_mesh_data->IndexVector.push_back(0);
			sub_mesh_data->IndexVector.push_back(2);
			sub_mesh_data->IndexVector.push_back(3);

			//hat
			box_volume = box_volume*4;
			pos = Vec3(m_Size ,box_volume,box_volume) + offset;;
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(m_Size ,box_volume,-box_volume)  + offset;
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(m_Size ,-box_volume,-box_volume) + offset;
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(m_Size ,-box_volume,box_volume)  + offset;
			sub_mesh_data->PositionVector.push_back(pos);

			pos= Vec3(m_Size + box_volume*5,0,0)  + offset;
			sub_mesh_data->PositionVector.push_back(pos);

			sub_mesh_data->IndexVector.push_back(8);
			sub_mesh_data->IndexVector.push_back(9);
			sub_mesh_data->IndexVector.push_back(10);
			sub_mesh_data->IndexVector.push_back(8);
			sub_mesh_data->IndexVector.push_back(10);
			sub_mesh_data->IndexVector.push_back(11);

			sub_mesh_data->IndexVector.push_back(8);
			sub_mesh_data->IndexVector.push_back(12);
			sub_mesh_data->IndexVector.push_back(9);

			sub_mesh_data->IndexVector.push_back(9);
			sub_mesh_data->IndexVector.push_back(12);
			sub_mesh_data->IndexVector.push_back(10);


			sub_mesh_data->IndexVector.push_back(10);
			sub_mesh_data->IndexVector.push_back(12);
			sub_mesh_data->IndexVector.push_back(11);

			sub_mesh_data->IndexVector.push_back(11);
			sub_mesh_data->IndexVector.push_back(12);
			sub_mesh_data->IndexVector.push_back(8);

		}
		else if(m_Type == GT_PLANE)
		{
			sub_mesh_data->Type = TRIANGLE_LIST;

			Float thickness = 0.01;

			Vec3 pos = Vec3(0,-thickness,0);
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(m_Size,-thickness,0);
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(m_Size,-thickness,m_Size);
			sub_mesh_data->PositionVector.push_back(pos);

			pos = Vec3(0,-thickness,0);
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(m_Size,-thickness,m_Size);
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(0,-thickness,m_Size);
			sub_mesh_data->PositionVector.push_back(pos);

			sub_mesh_data->IndexVector.push_back(0);
			sub_mesh_data->IndexVector.push_back(1);
			sub_mesh_data->IndexVector.push_back(2);
			sub_mesh_data->IndexVector.push_back(3);
			sub_mesh_data->IndexVector.push_back(4);
			sub_mesh_data->IndexVector.push_back(5);

			sub_mesh_data->IndexVector.push_back(2);
			sub_mesh_data->IndexVector.push_back(1);
			sub_mesh_data->IndexVector.push_back(0);
			sub_mesh_data->IndexVector.push_back(5);
			sub_mesh_data->IndexVector.push_back(4);
			sub_mesh_data->IndexVector.push_back(3);

			pos = Vec3(0,thickness,0);
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(m_Size,thickness,0);
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(m_Size,thickness,m_Size);
			sub_mesh_data->PositionVector.push_back(pos);

			pos = Vec3(0,-thickness,0);
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(m_Size,thickness,m_Size);
			sub_mesh_data->PositionVector.push_back(pos);
			pos = Vec3(0,thickness,m_Size);
			sub_mesh_data->PositionVector.push_back(pos);

			sub_mesh_data->IndexVector.push_back(6);
			sub_mesh_data->IndexVector.push_back(7);
			sub_mesh_data->IndexVector.push_back(8);
			sub_mesh_data->IndexVector.push_back(9);
			sub_mesh_data->IndexVector.push_back(10);
			sub_mesh_data->IndexVector.push_back(11);

			sub_mesh_data->IndexVector.push_back(8);
			sub_mesh_data->IndexVector.push_back(7);
			sub_mesh_data->IndexVector.push_back(6);
			sub_mesh_data->IndexVector.push_back(11);
			sub_mesh_data->IndexVector.push_back(10);
			sub_mesh_data->IndexVector.push_back(9);
		}
		GetSceneObject()->GetFirstComponentByClass<IManualMeshComponent>()->SetMeshData(mesh);
	}

	void GizmoComponent::OnNewCursorInfo(CursorMovedOverSceneEventPtr message)
	{
		SceneObjectPtr obj_under_cursor = message->GetSceneObjectUnderCursor();
		if(m_Active || obj_under_cursor == GetSceneObject())
		{
			if(!m_Highlight)
			{
				GetSceneObject()->GetFirstComponentByClass<IManualMeshComponent>()->SetSubMeshMaterial(&m_HighlightMat);
			}
			m_Highlight = true;
		}
		else
		{
			if(m_Highlight)
			{
				GetSceneObject()->GetFirstComponentByClass<IManualMeshComponent>()->SetSubMeshMaterial(&m_RegularMat);
			}
			m_Highlight = false;
		}
	}

	Vec3 GizmoComponent::GetPosition(const Ray &ray)
	{
		Quaternion rot = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetRotation();
		Mat4 rot_mat(rot);
		
		Vec3 c_pos = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetPosition();
		Vec3 r_vec = rot_mat.GetXAxis();
		Vec3 v_vec = rot_mat.GetZAxis();
		Vec3 up_vec = rot_mat.GetYAxis();


		//select projection plane
		if(m_Type == GT_AXIS)
		{
			Float v1 = fabs(Vec3::Dot(ray.m_Dir, up_vec));
			Float v2 = fabs(Vec3::Dot(ray.m_Dir, v_vec));
			Float value;
			if(v1 > v2)
				value = Plane::RayIsect(ray,Plane(c_pos,up_vec));
			else
				value  = Plane::RayIsect(ray,Plane(c_pos,v_vec));

			if(value > 0)
			{
				Vec3 isect_pos = ray.m_Origin + ray.m_Dir*value;

				Vec3 ret = ProjectPointOnAxis(c_pos, r_vec, isect_pos);
				return ret;

			}
		}
		else
		{
			Float value = Plane::RayIsect(ray,Plane(c_pos,up_vec));
			if(value > 0)
			{
				Vec3 isect_pos = ray.m_Origin + ray.m_Dir*value;

				Vec3 proj_r = ProjectPointOnAxis(c_pos, r_vec, isect_pos);
				Vec3 proj_v = ProjectPointOnAxis(c_pos, v_vec, isect_pos);

				Vec3 ret = c_pos + (proj_r - c_pos ) + (proj_v - c_pos);

				return ret;
			}

		}
		return c_pos;
	}

	Quaternion GizmoComponent::GetRotation(Float delta)
	{
		SceneObjectPtr selected = GetFirstSelected();
		if(selected)
		{
			LocationComponentPtr location = selected->GetFirstComponentByClass<ILocationComponent>();
			if(location)
			{
				//Quaternion selected_rot = location->GetWorldRotation();
				Quaternion rot = GetSceneObject()->GetFirstComponentByClass<ILocationComponent>()->GetWorldRotation();
				Mat4 rot_mat(rot);
				Vec3 r_vec = rot_mat.GetXAxis();
				r_vec.Normalize();

				//TODO:Check this!
				static Float rest_angle = 0;
				Quaternion final_rot;
				Float angle = delta;
				angle = m_EditorSceneManager->GetMouseToolController()->SnapAngle(angle + rest_angle);
				if(angle == 0)
				{
					rest_angle += delta;
				}
				else
				{
					rest_angle= 0;
				}
				final_rot.FromAngleAxis(angle, r_vec);
				return final_rot*rot;
			}
		}
		return Quaternion::IDENTITY;
	}

	Vec3 GizmoComponent::ProjectPointOnAxis(const Vec3 &axis_origin, const Vec3 &axis_dir, const Vec3 &p) const
	{
		Vec3 c = p-axis_origin;
		Float t = Vec3::Dot(axis_dir,c);
		if(m_Mode == GM_LOCAL)
		{
			t = m_EditorSceneManager->GetMouseToolController()->SnapPosition(t);
		}

		Vec3 point_on_axis = axis_dir*t;

		Vec3 res = (axis_origin + point_on_axis);

		if(m_Mode == GM_WORLD)
		{
			res.x = m_EditorSceneManager->GetMouseToolController()->SnapPosition(res.x);
			res.z = m_EditorSceneManager->GetMouseToolController()->SnapPosition(res.z);
			res.y = m_EditorSceneManager->GetMouseToolController()->SnapPosition(res.y);
		}

		return res;
	}

	Float GizmoComponent::SnapValue(Float value, Float snap)
	{
		Float new_value = value/snap;
		new_value = int(new_value);
		return new_value*snap;
	}
}
